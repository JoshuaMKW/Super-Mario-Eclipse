/*========================================
||    Kuribo SDK 3.0
==========================================

Supported targets:
- Kuribo Kernel pre-release, core version 0

Supported compilers:
- Devkitpro GCC
- LLVM Clang

Provided APIs:
- KURIBO_MODULE_BEGIN(name, author, version) / KURIBO_MODULE_END()
  - KURIBO_EXECUTE_ON_LOAD {}
  - KURIBO_EXECUTE_ON_UNLOAD {}
  - KURIBO_EXECUTE_ALWAYS {}
- KURIBO_PATCH_B(addr, value)
- KURIBO_PATCH_BL(addr, value)
- KURIBO_PATCH_32(addr, value)
- KURIBO_VTABLE(vtable_pointer, function_index)
- KURIBO_EXPORT_AS(function, name)
- KURIBO_EXPORT(function)
- KURIBO_GET_PROCEDURE(name)
- kBranch, kCall, kWrite32, kVtable: Light c++ wrappers

Provided Types:
 - u32, s32: Unsigned, signed int
 - u16, s16: Unsigned, signed short
 - u8, s8:   Unsigned, signed char
 - f32, f64: 32-bit, 64-bit float

Changelog:
 - 1.0: Initial revision
 - 2.0: Added linking APIs
 - 3.0: Added C++ API

Example Usage:
  KURIBO_MODULE_BEGIN("Demo Module", "riidefi", "Beta")
  {
    // =======================
    // KURIBO_EXECUTE_* APIs
    // =======================
      // You can call regular code within these block.
      KURIBO_EXECUTE_ON_LOAD { printf("Loading our wonderful module!\n"); }
      KURIBO_EXECUTE_ON_UNLOAD { printf("Unloading our wonderful module!\n"); }

      // This block will be called both when unloading and loading the module.
      KURIBO_EXECUTE_ALWAYS { printf("Module interaction done.\nExiting!\n"); }

    // =======================
    // KURIBO_PATCH_* APIs
    // =======================
      // This directly inserts a `bl MyFunction`.
      // - Automatically reverted when the module is unloaded
      KURIBO_PATCH_BL(0x807403DC, MyFunction);

      // This directly inserts a `b <function>`.
      // - Automatically reverted when the module is unloaded
      KURIBO_PATCH_BL(0x807403DC, MyFunction);

      // Patches a virtual table entry to point to MyFunction.
      // - Automatically reverted when the module is unloaded
      KURIBO_PATCH_32(0x808CAFEC, MyFunction);

      // Achieves the same as above, but with some helpers
      enum PTA_Vtable {  // Describes the vtable layout
        PTA_DESTRUCTOR,  // ~PTA()
        PTA_NODE_ATTACH, // void attach(PTA* other)
        PTA_NODE_DETACH, // void detach(PTA* other)
        PTA_NODE_RESET   // void reset()
      };
      KURIBO_PATCH_32(KURIBO_VTABLE(0x808CAFE0, PTA_NODE_ATTACH), MyFunction);

    // =======================
    // KURIBO LINKING APIs
    // =======================
      // Export a function symbol as a specified name.
      KURIBO_EXPORT_AS(MySqrt, "sqrt");

      // Export a function symbol.
      KURIBO_EXPORT(MySqrt);

      // Get the address of a procedure.
      void* MySqrt = KURIBO_GET_PROCEDURE("MySqrt");

    // =======================
    // C++ Wrapper APIs
    // =======================
      kBranch(0x808CAFEC, (int input) {
        return input * 3;
      });
      kCall(0x808CAFEC, (int input) {
        return input + 7;
      });
      kWrite32(0x808CAFEC, (int input) -> bool {
        return input & 3 != 0;
      });
      kVtable(0x808CAFE0, PTA_NODE_ATTACH, (PTA* self, PTA* other) {
        if (self->getChildIndexOf(other) < 0) {
          self->attachChild(other);
        }
      });
  }
  KURIBO_MODULE_END();

========================================*/
#pragma once

#define KURIBO_MODULE_BEGIN(name, author, version) /* Begin a module block */

/*=========================================================*
  These commands may only be executed within a module block.
 *========================================================*/

/* Starts an anonymous function called when the module is loading. */
#define KURIBO_EXECUTE_ON_LOAD
/* Starts an anonymous function called when the module is unloading. */
#define KURIBO_EXECUTE_ON_UNLOAD
/* Starts an anonymous function called when the module is loading and
 * unloading. */
#define KURIBO_EXECUTE_ALWAYS

/* Insert at the specified address "b location". */
#define KURIBO_PATCH_B(address, location)
/* Insert at the specified address "bl location". */
#define KURIBO_PATCH_BL(address, location)
/* Write value to the four bytes starting at address. */
#define KURIBO_PATCH_32(address, value)
/* Helper for calculating a virtual table offset */
#define KURIBO_VTABLE(vtable_address, function_index)

/* Export a function symbol as a specified name. */
#define KURIBO_EXPORT_AS(function, name)
/* Export a function symbol. */
#define KURIBO_EXPORT(function)
/* Get the address of a procedure. */
#define KURIBO_GET_PROCEDURE(function)

#define KURIBO_MODULE_END() /* End a module block */

#include "kuribo_sdk_impl.h"

#if defined(__cplusplus) && __cplusplus >= 201103
// Kamek-like API wrapper
#define kBranch(address, function) KURIBO_PATCH_B(address, +[] function)
#define kCall(address, function) KURIBO_PATCH_BL(address, +[] function)
#define kWrite32(address, value) KURIBO_PATCH_32(address, value)
#define kVtable(vtable_address, function_index, function)                      \
  KURIBO_PATCH_32(KURIBO_VTABLE(vtable_address, function_index), +[] function)
#endif

#if defined(__cplusplus)

namespace pp {

struct Patch {
  u32 mAddr; // Address in memory
  u32 mVal;  // New value
  u32 mSave; // Value before apply

  bool mApplied; // If the patch has been applied
};

// Grabs data lazily
inline Patch CreatePatch(u32 addr, u32 val) {
  Patch data;
  data.mAddr = addr;
  data.mVal = val;
  data.mSave = 0xCCCCCCCC; // Acquired right before apply
  data.mApplied = false;
  return data;
}

inline void EnablePatch(Patch& patch) {
  if (patch.mApplied)
    return;

  u32* addr = (u32*)patch.mAddr;

  patch.mSave = *addr;
  *addr = patch.mVal;
  icbi(addr);

  patch.mApplied = true;
}

inline void DisablePatch(Patch& patch) {
  if (!patch.mApplied)
    return;

  u32* addr = (u32*)patch.mAddr;

  *addr = patch.mSave;
  icbi(patch.mAddr);
  patch.mSave = 0xCCCCCCCC;

  patch.mApplied = false;
}

inline u32 PPCBranchInstr(u32 delta, bool lk) {
  return 0x48000000 | (delta & 0x3fffffc) | (lk ? 1 : 0);
}

class auto_patch {
public:
  auto_patch(u32 addr, u32 val, bool by_default = true)
      : mPatch(CreatePatch(addr, val)) {
    if (by_default)
      enable();
  }
  ~auto_patch() { disable(); }

  bool is_enabled() const { return mPatch.mApplied; }

  void enable() { EnablePatch(mPatch); }
  void disable() { DisablePatch(mPatch); }

  u32 overwritten_value() const { return mPatch.mSave; }
  u32 new_value() const { return mPatch.mVal; }

private:
  Patch mPatch;
};

class togglable_ppc_b : public auto_patch {
public:
  togglable_ppc_b(u32 addr, void* target, bool by_default = true)
      : auto_patch(addr, PPCBranchInstr((u32)target - addr, false),
                   by_default) {}
};
class togglable_ppc_bl : public auto_patch {
public:
  togglable_ppc_bl(u32 addr, void* target, bool by_default = true)
      : auto_patch(addr, PPCBranchInstr((u32)target - addr, true), by_default) {
  }
};

#define PatchIdentifier MACRO_CONCAT(_patch, __COUNTER__)

#define PatchB(a, b) togglable_ppc_b PatchIdentifier((u32)a, (void*)b)
#define PatchBL(a, b) togglable_ppc_bl PatchIdentifier((u32)a, (void*)b)
#define Patch32(a, b) auto_patch PatchIdentifier((u32)a, (u32)b)

struct dummy {};

#define DefineModule(name, author, version)                                    \
  dummy module_is_defined;                                                     \
  KURIBO_MODULE_BEGIN(name, author, version)                                   \
  KURIBO_MODULE_END()

struct export_as {
  export_as(void* func, const char* name) {
    if (__kuribo_ctx.register_procedure) {
      __kuribo_ctx.register_procedure(name, (u32)func);
    }
  }
  ~export_as() {
    // There is no easy way to revoke an export, currently
    // We could re-register as nullptr
  }
};

#define ExportAs(a, b) export_as PatchIdentifier((void*)a, b)
#define Export(a) export_as PatchIdentifier((void*)a, ##a)

typedef void (*VoidFunc)();

struct on_load {
  on_load(VoidFunc fn) { fn(); }
};

struct on_unload {
  on_unload(VoidFunc fn) : _fn(fn) {}
  ~on_unload() { _fn(); }

  VoidFunc _fn;
};

#define OnLoad(fn) on_load PatchIdentifier(fn)
#define OnUnload(fn) on_unload PatchIdentifier(fn)

//! You probably don't want to use this: the linker will automatically call this
//! for you when you call an external function
inline void* Import(const char* name) { return KURIBO_GET_PROCEDURE(name); }

} // namespace pp

#endif