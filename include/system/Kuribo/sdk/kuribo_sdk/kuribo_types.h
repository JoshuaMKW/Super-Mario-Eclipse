//! @file
//! @brief Types header, not dependent on stdint.h

#pragma once

/* No guarantee of stdint.h or __has_include.
 * This works on all targets.
 */
#ifndef KURIBO_NO_TYPES
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef signed char s8;
typedef float f32;
typedef double f64;
#endif

/* Misc compiler info */
#ifdef __VERSION__
#define __KURIBO_CC_VERSION __VERSION__
#elif defined(__CWCC__)
#define STRINGIZE(x) STRINGIZE_(x)
#define STRINGIZE_(x) #x
#define __KURIBO_CC_VERSION "CWCC " STRINGIZE(__CWCC__)
#else
#define __KURIBO_CC_VERSION "Unknown"
#endif

#ifdef __cplusplus
#define KURIBO_EXTERN_C extern "C"
#else
#define KURIBO_EXTERN_C
#endif