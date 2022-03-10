#pragma once

#define SME_STRINGIZE_(x) #x
#define SME_STRINGIZE(x) SME_STRINGIZE_(x)

#define SME_WIDE_(s) L##s
#define SME_WIDE(s) SME_WIDE_(s)

#define SME_CONCATENATE_(a, b) a##b
#define SME_CONCATENATE(a, b) SME_CONCATENATE_(a, b)

#if !defined(NTSCU) && !defined(PAL) && !defined(NTSCJ) && !defined (NTSCK)
#define PAL
#endif

#if defined(NTSCU)
#define SME_PORT_REGION(ntscu, pal, ntscj, ntsck) ntscu
#elif defined(PAL)
#define SME_PORT_REGION(ntscu, pal, ntscj, ntsck) pal
#elif defined(NTSCJ)
#define SME_PORT_REGION(ntscu, pal, ntscj, ntsck) ntscj
#elif defined(NTSCK)
#define SME_PORT_REGION(ntscu, pal, ntscj, ntsck) ntsck
#else
#error "No region provided!"
#endif

#define SME_FROM_GPR(reg, var) asm volatile("mr %0, " #reg : "=r"(var))
#define SME_TO_GPR(reg, var) asm volatile("mr " #reg ", %0" : : "r"(var))
#define SME_FROM_FPR(reg, var) asm volatile("fmr %0, " #reg : "=r"(var))
#define SME_TO_FPR(reg, var) asm volatile("fmr " #reg ", %0" : : "r"(var))

#ifdef __cplusplus
#if __cplusplus >= 201103L
#define offsetof(t, d) ((size_t) & (((t *)0)->d))
//#define offsetof(t, d) __builtin_offsetof(t, d)
#else
#define offsetof(t, d) ((size_t) & (((t *)0)->d))
#endif
#define SME_EXTERN_C extern "C"
#else
#define SME_EXTERN_C extern
#endif

#define SME_NO_INLINE __attribute__((noinline))

#define SME_ATOMIC_CODE(code) \
u32 __atomic_interrupt_state = OSDisableInterrupts(); \
code  \
OSRestoreInterrupts(__atomic_interrupt_state);

#ifdef __MWERKS__
#define SME_REGISTER register
#define SME_PURE_ASM asm
#else
#define SME_REGISTER
#define SME_PURE_ASM __attribute__((naked))
#endif

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) ||    \
    (defined(__ICC) && (__ICC >= 600))
#define SME_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define SME_FUNC_SIG __FUNCSIG__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define SME_FUNC_SIG __func__
#else
#define SME_FUNC_SIG "UNDEFINED"
#endif

#define SME_CALL_NAIVE(addr, ...) ((void(*)(...))addr)(__VA_ARGS__)