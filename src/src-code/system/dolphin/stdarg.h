#ifndef STDARG_H
#define STDARG_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct va_list_struct
	{
		char gpr, fpr, reserved[2], *input_arg_area, *reg_save_area;
	} va_list[1];

	extern void *__va_arg(void *, int);

#define va_start(ap, last) (__builtin_va_info(&ap))
#define va_arg(ap, type) (*((type *)__va_arg(ap, _var_arg_typeof(type))))
#define va_end(ap) ((void)0)

#ifdef __cplusplus
}
#endif

#endif