#ifndef __KURIBO_SDK_IMPL
#define __KURIBO_SDK_IMPL

#include "kuribo_sdk/kuribo_internal_api.h"
#include "kuribo_sdk/kuribo_types.h"

#define __kuribo_mfmsr()                                                       \
  ({                                                                           \
    u32 _rval;                                                                 \
    asm volatile("mfmsr %0" : "=r"(_rval));                                    \
    _rval;                                                                     \
  })
#define __kuribo_mtmsr(val) asm volatile("mtmsr %0" : : "r"(val))

/* @brief Fillin a simple metadata structure for a
 *        KURIBO_REASON_INQUIRE_META_DESC call.
 *
 * @return A core exit code.
 */
static inline int __kuribo_fillin_metadata_v0(
    __kuribo_simple_meta_v0*
        data,                  /* Data to fill in. Must be a valid pointer. */
    const char* module_name,   /* Name of the module */
    const char* module_author, /* Reported creator(s) of the module. */
    const char* module_version /* Reported version of the module. */
) {
  if (!data)
    return KURIBO_EXIT_FAILURE;

  data->compiler_name = __KURIBO_CC_VERSION;
  data->build_date = __DATE__ " at " __TIME__;

  data->module_name = module_name;
  data->module_author = module_author;
  data->module_version = module_version;

  return KURIBO_EXIT_SUCCESS;
}

/* Global context declaration.
 * - Not currently used.
 */
extern __kuribo_module_ctx_t __kuribo_ctx;

#ifndef KURIBO_NO_ABI_HELPERS
typedef struct __kuribo_dtor_node {
  struct __kuribo_dtor_node* pred;
  void* dtor;
  void* obj;
} __kuribo_dtor_node;

extern void** __ctor_loc;
extern void** __ctor_end;

extern __kuribo_dtor_node* __kuribo_dtor_list;

typedef void (*__kuribo_ctor_t)(void);
typedef __kuribo_ctor_t* __kuribo_ctors;

static inline void __kuribo_on_attach(u32 start_addr) {
  char* ctor_begin_rel = (char*)&__ctor_loc;
  char* ctor_end_rel = (char*)&__ctor_end;

  __kuribo_ctors ctor_begin = (__kuribo_ctors)(ctor_begin_rel + start_addr);
  __kuribo_ctors ctor_end = (__kuribo_ctors)(ctor_end_rel + start_addr);

  for (__kuribo_ctors f = ctor_begin; f < ctor_end && *f; ++f) {
    (*f)();
  }
}

static inline void __kuribo_on_detach() {
  for (__kuribo_dtor_node* it = __kuribo_dtor_list; it; it = it->pred) {
    ((void (*)(void*, int))it->dtor)(it->obj, -1);
  }
}

#define KURIBO_ATTACH_DETATCH_HOOKS_IMPL                                       \
  __kuribo_dtor_node* __kuribo_dtor_list;                                      \
  KURIBO_EXTERN_C void __register_global_object(void* obj, void* dtor,                         \
                                __kuribo_dtor_node* node) {                    \
    node->pred = __kuribo_dtor_list;                                           \
    node->dtor = dtor;                                                         \
    node->obj = obj;                                                           \
    __kuribo_dtor_list = node;                                                 \
  }                                                                            \
  /* sizeof(__kuribo_guard) == 8 */                                            \
  struct __kuribo_guard {                                                      \
    u8 is_init; /* This byte must be here, the rest are free */                \
    u8 _pad[3];                                                                \
                                                                               \
    /* On PPC, we will just disable multitasking */                            \
    u32 msr_save;                                                              \
  };                                                                           \
  KURIBO_EXTERN_C int __cxa_guard_acquire(__kuribo_guard* guard) {                             \
    const u32 msr = __kuribo_mfmsr();                                          \
    __kuribo_mtmsr(msr & ~0x8000);                                             \
                                                                               \
    guard->is_init = 0;                                                        \
    guard->msr_save = msr;                                                     \
                                                                               \
    return 1;                                                                  \
  }                                                                            \
  KURIBO_EXTERN_C void __cxa_guard_release(__kuribo_guard* guard) {                            \
    __kuribo_mtmsr(guard->msr_save);                                           \
  }

#else // KURIBO_NO_ABI_HELPERS

#define __kuribo_on_attach(start_addr)
#define __kuribo_on_detach()
#define KURIBO_ATTACH_DETATCH_HOOKS_IMPL

#endif // KURIBO_NO_ABI_HELPERS

#ifdef __CWCC__
#define _start __start
#endif

#ifdef KURIBO_MODULE_BEGIN
#undef KURIBO_MODULE_BEGIN
#endif

#define KURIBO_MODULE_BEGIN(name, author, version)                             \
  __kuribo_module_ctx_t __kuribo_ctx;                                          \
  KURIBO_ATTACH_DETATCH_HOOKS_IMPL;                                            \
  KURIBO_EXTERN_C int _start(int, __kuribo_module_ctx_t*);                     \
  int _start(int __reason, __kuribo_module_ctx_t* __ctx) {                     \
    if (!__ctx || __ctx->core_version != KURIBO_CORE_VERSION) {                \
      return KURIBO_EXIT_FAILURE;                                              \
    }                                                                          \
    __kuribo_ctx = *__ctx;                                                     \
    switch (__reason) {                                                        \
    default:                                                                   \
      return KURIBO_EXIT_FAILURE;                                              \
    case KURIBO_REASON_INQUIRE_META_DESC:                                      \
      return __kuribo_fillin_metadata_v0(__ctx->udata.fillin_meta, name,       \
                                         author, version);                     \
    case KURIBO_REASON_LOAD:                                                   \
    case KURIBO_REASON_UNLOAD: {                                               \
      const int __kuribo_attach = __reason == KURIBO_REASON_LOAD ? 1 : 0;      \
      const int __kuribo_detach = __reason == KURIBO_REASON_UNLOAD ? 1 : 0;    \
      if (__kuribo_attach)                                                     \
        __kuribo_on_attach((u32)__ctx->start_address);                         \
      else if (__kuribo_detach)                                                \
        __kuribo_on_detach();

#ifdef KURIBO_MODULE_END
#undef KURIBO_MODULE_END
#endif

#define KURIBO_MODULE_END()                                                    \
  return KURIBO_EXIT_SUCCESS;                                                  \
  } /* End case */                                                             \
  } /* End switch*/                                                            \
  return KURIBO_EXIT_FAILURE;                                                  \
  } /* End function*/

#ifndef _WIN32
#define dcbf(_val) asm volatile("dcbf 0, %0" : : "r"(_val))
#define icbi(_val) asm volatile("icbi 0, %0" : : "r"(_val))
#else
#define dcbf
#define icbi
#endif // _WIN32
static inline void __kuribo_multipatch_write(u32* addr, u32 value, u32* save,
                                             int attach) {
  if (attach) {
    if (save) {
      *save = *addr;
    }
    *addr = value;
  } else if (save) {
    *addr = *save;
  } else {
    return;
  }
  dcbf(addr);
  asm("sync");
  icbi(addr);
  asm("isync");
}

static inline void __kuribo_multipatch_b(u32* addr, u32 value, u32* save,
                                         int attach, int lk) {
  const u32 delta = (u32)value - (u32)addr;
  __kuribo_multipatch_write(
      addr, 0x48000000 | (delta & 0x3fffffc) | (lk ? 1 : 0), save, attach);
}

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)

#define __KURIBO_PATCH_B_IMPL(addr, value, gen, lk)                            \
  {                                                                            \
    static u32 gen;                                                            \
    __kuribo_multipatch_b(((u32*)addr), ((u32)value), &gen, __kuribo_attach,   \
                          !!(lk));                                             \
  }
#undef KURIBO_PATCH_B
#undef KURIBO_PATCH_BL

#define KURIBO_PATCH_B(addr, value)                                            \
  __KURIBO_PATCH_B_IMPL((addr), (value),                                       \
                        MACRO_CONCAT(__kuribo_multisave, __COUNTER__), 0)
#define KURIBO_PATCH_BL(addr, value)                                           \
  __KURIBO_PATCH_B_IMPL((addr), (value),                                       \
                        MACRO_CONCAT(__kuribo_multisave, __COUNTER__), 1)

#define __KURIBO_PATCH_32_IMPL(addr, value, gen)                               \
  {                                                                            \
    static u32 gen;                                                            \
    __kuribo_multipatch_write(((u32*)addr), ((u32)value), &gen,                \
                              __kuribo_attach);                                \
  }
#undef KURIBO_PATCH_32
#define KURIBO_PATCH_32(addr, value)                                           \
  __KURIBO_PATCH_32_IMPL((addr), (value),                                      \
                         MACRO_CONCAT(__kuribo_multisave, __COUNTER__))

#ifdef KURIBO_EXPORT_AS
#undef KURIBO_EXPORT_AS
#endif

#define KURIBO_EXPORT_AS(function, name)                                       \
  if (__kuribo_attach && __ctx->register_procedure)                            \
  __ctx->register_procedure(name, (u32)&function)

#ifdef KURIBO_EXPORT
#undef KURIBO_EXPORT
#endif

#define KURIBO_EXPORT(function) KURIBO_EXPORT_AS(function, #function)

#ifdef KURIBO_GET_PROCEDURE
#undef KURIBO_GET_PROCEDURE
#endif

#define KURIBO_GET_PROCEDURE(function)                                         \
  ((void*)__kuribo_ctx.get_procedure(function))

#undef KURIBO_VTABLE
#undef KURIBO_EXECUTE_ON_LOAD
#undef KURIBO_EXECUTE_ON_UNLOAD
#undef KURIBO_EXECUTE_ALWAYS

#define KURIBO_VTABLE(vtable, id) (u32)(u32(vtable) + 8 + u32(id) * 4)
#define KURIBO_EXECUTE_ON_LOAD if (__kuribo_attach)
#define KURIBO_EXECUTE_ON_UNLOAD if (__kuribo_detach)
#define KURIBO_EXECUTE_ALWAYS if (__kuribo_attach || __kuribo_detach)

#endif // __KURIBO_SDK_IMPL