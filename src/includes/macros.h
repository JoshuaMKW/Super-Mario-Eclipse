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

#ifndef _MSC_VER
#define SME_NO_INLINE __attribute__((noinline))
#else
#define SME_NO_INLINE __declspec(noinline)
#endif

#ifdef __MWERKS__
#define SME_REGISTER register
#else
#define SME_REGISTER
#endif

#define SME_CALL_NAIVE(addr, ...) ((void(*)(...))addr)(__VA_ARGS__)