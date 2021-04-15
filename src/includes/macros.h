#pragma once

#define SME_STRINGIZE_(x) #x
#define SME_STRINGIZE(x) SME_STRINGIZE_(x)

#define SME_WIDE_(s) L##s
#define SME_WIDE(s) SME_WIDE_(s)

#define SME_CONCATENATE_(a, b) a##b
#define SME_CONCATENATE(a, b) SME_CONCATENATE_(a, b)

#if !defined(NTSCU) && !defined(PAL) && !defined(NTSCJ) && !defined (NTSCK)
#define NTSCU
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