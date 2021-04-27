#ifndef SME_MAX_SHINES
#define SME_MAX_SHINES 120
#else
#if SME_MAX_SHINES < 0
#undef SME_MAX_SHINES
#define SME_MAX_SHINES 0
#elif SME_MAX_SHINES > 999
#undef SME_MAX_SHINES
#define SME_MAX_SHINES 999
#endif
#endif

#define SME_MAX_PLAYERS 1

/* Misc compiler info */
#define SME_VERSION "v0.4a"

#ifdef __VERSION__
#define SME_CC_VERSION __VERSION__
#elif defined(__CWCC__)
#define SME_CC_VERSION "CWCC " STRINGIZE(__CWCC__)
#else
#define SME_CC_VERSION "Unknown"
#endif