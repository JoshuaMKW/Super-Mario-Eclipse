#ifndef __KURIBO_SDK_IMPL
#define __KURIBO_SDK_IMPL

#include "kuribo_sdk/kuribo_types.h"
#include "kuribo_sdk/kuribo_internal_api.h"


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
extern __kuribo_module_ctx_t* __kuribo_ctx;



#ifdef KURIBO_MODULE_BEGIN
#undef KURIBO_MODULE_BEGIN
#endif

#define KURIBO_MODULE_BEGIN(name, author, version)                             \
  __kuribo_module_ctx_t* __kuribo_ctx;                                         \
  KURIBO_EXTERN_C int _start(int, __kuribo_module_ctx_t*);                     \
  int _start(int __reason, __kuribo_module_ctx_t* __ctx) {                     \
    if (!__ctx || __ctx->core_version != KURIBO_CORE_VERSION) {                \
      return KURIBO_EXIT_FAILURE;                                              \
    }                                                                          \
    __kuribo_ctx = __ctx;                                                      \
    switch (__reason) {                                                        \
    default:                                                                   \
      return KURIBO_EXIT_FAILURE;                                              \
    case KURIBO_REASON_INQUIRE_META_DESC:                                      \
      return __kuribo_fillin_metadata_v0(__ctx->udata.fillin_meta, name,       \
                                         author, version);                     \
    case KURIBO_REASON_LOAD:                                                   \
    case KURIBO_REASON_UNLOAD: {                                               \
      const int __kuribo_attach = __reason == KURIBO_REASON_LOAD ? 1 : 0;      \
      const int __kuribo_detach = __reason == KURIBO_REASON_UNLOAD ? 1 : 0;

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
#define icbi(_val) asm volatile("icbi 0, %0" : : "r"(_val))
#else
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
  icbi(addr);
}

static inline void __kuribo_multipatch_b(u32* addr, u32 value, u32* save,
                                         int attach, int lk) {
  const u32 delta = (u32)value - (u32)addr;
  __kuribo_multipatch_write(
      addr, 0x48000000 | (delta & 0x3fffffc) | (lk ? 1 : 0),
      save, attach);
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
  ((void*)__kuribo_ctx->get_procedure(function))

#undef KURIBO_VTABLE
#undef KURIBO_EXECUTE_ON_LOAD
#undef KURIBO_EXECUTE_ON_UNLOAD
#undef KURIBO_EXECUTE_ALWAYS

#define KURIBO_VTABLE(vtable, id) (u32)(u32(vtable) + 8 + u32(id) * 4)
#define KURIBO_EXECUTE_ON_LOAD if (__kuribo_attach)
#define KURIBO_EXECUTE_ON_UNLOAD if (__kuribo_detach)
#define KURIBO_EXECUTE_ALWAYS if (__kuribo_attach || __kuribo_detach)

#endif // __KURIBO_SDK_IMPL