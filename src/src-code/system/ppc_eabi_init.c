#include "ppc_eabi_linker.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*funcptr) (void);
__declspec(section ".init") extern funcptr _f_ctors[];
__declspec(section ".init") extern funcptr _f_dtors[];

#ifdef __cplusplus
}
#endif

static void __init_kamek_cpp(void);

__asm void __init_kamek_user(void)
{
  fralloc

  bl __init_kamek_cpp

  frfree
  blr
}

static void __init_kamek_cpp(void)
{
	funcptr *constructor;

	for (constructor = _f_ctors; *constructor; constructor++) {
		(*constructor)();
	}
}