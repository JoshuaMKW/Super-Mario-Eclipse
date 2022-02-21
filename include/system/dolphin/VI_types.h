#pragma once

#include "types.h"
#include "GX_types.h"

/*!
 * \addtogroup vi_defines List of defines used for the VIDEO subsystem
 * @{
 */


#define VI_DISPLAY_PIX_SZ           2		/*!< multiplier to get real pixel size in bytes */

/*!
 * \addtogroup vi_modetypedef VIDEO mode types
 * @{
 */

#define VI_INTERLACE                0		/*!< Video mode INTERLACED. */
#define VI_NON_INTERLACE            1		/*!< Video mode NON INTERLACED */
#define VI_PROGRESSIVE              2		/*!< Video mode PROGRESSIVE. Special mode for higher quality */

/*!
 * @}
 */


/*!
 * \addtogroup vi_standardtypedef VIDEO standard types
 * @{
 */

#define VI_NTSC                     0		/*!< Video standard used in North America and Japan */
#define VI_PAL                      1		/*!< Video standard used in Europe */
#define VI_MPAL                     2		/*!< Video standard, similar to NTSC, used in Brazil */
#define VI_DEBUG                    3		/*!< Video standard, for debugging purpose, used in North America and Japan. Special decoder needed */
#define VI_DEBUG_PAL                4		/*!< Video standard, for debugging purpose, used in Europe. Special decoder needed */
#define VI_EURGB60                  5		/*!< RGB 60Hz, 480 lines mode (same timing and aspect ratio as NTSC) used in Europe */

/*!
 * @}
 */


#define VI_XFBMODE_SF				0
#define VI_XFBMODE_DF				1


/*!
 * \addtogroup vi_fielddef VIDEO field types
 * @{
 */

#define VI_FIELD_ABOVE              1		/*!< Upper field in DS mode */
#define VI_FIELD_BELOW              0		/*!< Lower field in DS mode */

/*!
 * @}
 */


// Maximum screen space
#define VI_MAX_WIDTH_NTSC           720
#define VI_MAX_HEIGHT_NTSC          480

#define VI_MAX_WIDTH_PAL            720
#define VI_MAX_HEIGHT_PAL           576

#define VI_MAX_WIDTH_MPAL           720
#define VI_MAX_HEIGHT_MPAL          480

#define VI_MAX_WIDTH_EURGB60        VI_MAX_WIDTH_NTSC
#define VI_MAX_HEIGHT_EURGB60       VI_MAX_HEIGHT_NTSC

#define VIDEO_PadFramebufferWidth(width)     ((u16)(((u16)(width) + 15) & ~15))			/*!< macro to pad the width to a multiple of 16 */

/*!
 * @}
 */


#define VI_TVMODE(fmt, mode)   ( ((fmt) << 2) + (mode) )

#define VI_TVMODE_NTSC_INT			VI_TVMODE(VI_NTSC,        VI_INTERLACE)
#define VI_TVMODE_NTSC_DS			VI_TVMODE(VI_NTSC,        VI_NON_INTERLACE)
#define VI_TVMODE_NTSC_PROG			VI_TVMODE(VI_NTSC,        VI_PROGRESSIVE)

#define VI_TVMODE_PAL_INT			VI_TVMODE(VI_PAL,         VI_INTERLACE)
#define VI_TVMODE_PAL_DS			VI_TVMODE(VI_PAL,         VI_NON_INTERLACE)
#define VI_TVMODE_PAL_PROG			VI_TVMODE(VI_PAL,         VI_PROGRESSIVE)

#define VI_TVMODE_EURGB60_INT		VI_TVMODE(VI_EURGB60,     VI_INTERLACE)
#define VI_TVMODE_EURGB60_DS		VI_TVMODE(VI_EURGB60,     VI_NON_INTERLACE)
#define VI_TVMODE_EURGB60_PROG		VI_TVMODE(VI_EURGB60,     VI_PROGRESSIVE)

#define VI_TVMODE_MPAL_INT			VI_TVMODE(VI_MPAL,        VI_INTERLACE)
#define VI_TVMODE_MPAL_DS			VI_TVMODE(VI_MPAL,        VI_NON_INTERLACE)
#define VI_TVMODE_MPAL_PROG			VI_TVMODE(VI_MPAL,        VI_PROGRESSIVE)

#define VI_TVMODE_DEBUG_INT			VI_TVMODE(VI_DEBUG,       VI_INTERLACE)

#define VI_TVMODE_DEBUG_PAL_INT		VI_TVMODE(VI_DEBUG_PAL,   VI_INTERLACE)
#define VI_TVMODE_DEBUG_PAL_DS		VI_TVMODE(VI_DEBUG_PAL,   VI_NON_INTERLACE)


/*!
 * \addtogroup vi_defines List of defines used for the VIDEO subsystem
 * @{
 */


/*!
 * \addtogroup gxrmode_obj VIDEO render modes
 * @{
 */

extern GXRenderModeObj TVNtsc240Ds;				/*!< Video and render mode configuration for 240 lines,singlefield NTSC mode */
extern GXRenderModeObj TVNtsc240DsAa;			/*!< Video and render mode configuration for 240 lines,singlefield,antialiased NTSC mode */
extern GXRenderModeObj TVNtsc240Int;				/*!< Video and render mode configuration for 240 lines,interlaced NTSC mode */
extern GXRenderModeObj TVNtsc240IntAa;			/*!< Video and render mode configuration for 240 lines,interlaced,antialiased NTSC mode */
extern GXRenderModeObj TVNtsc480Int;				/*!< Video and render mode configuration for 480 lines,interlaced NTSC mode */
extern GXRenderModeObj TVNtsc480IntDf;			/*!< Video and render mode configuration for 480 lines,interlaced,doublefield NTSC mode */
extern GXRenderModeObj TVNtsc480IntAa;			/*!< Video and render mode configuration for 480 lines,interlaced,doublefield,antialiased NTSC mode */
extern GXRenderModeObj TVNtsc480Prog;			/*!< Video and render mode configuration for 480 lines,progressive,singlefield NTSC mode */
extern GXRenderModeObj TVNtsc480ProgSoft;
extern GXRenderModeObj TVNtsc480ProgAa;
extern GXRenderModeObj TVMpal240Ds;
extern GXRenderModeObj TVMpal240DsAa;
extern GXRenderModeObj TVMpal240Int;
extern GXRenderModeObj TVMpal240IntAa;
extern GXRenderModeObj TVMpal480Int;
extern GXRenderModeObj TVMpal480IntDf;			/*!< Video and render mode configuration for 480 lines,interlaced,doublefield,antialiased MPAL mode */
extern GXRenderModeObj TVMpal480IntAa;
extern GXRenderModeObj TVMpal480Prog;
extern GXRenderModeObj TVMpal480ProgSoft;
extern GXRenderModeObj TVMpal480ProgAa;
extern GXRenderModeObj TVPal264Ds;				/*!< Video and render mode configuration for 264 lines,singlefield PAL mode */
extern GXRenderModeObj TVPal264DsAa;				/*!< Video and render mode configuration for 264 lines,singlefield,antialiased PAL mode */
extern GXRenderModeObj TVPal264Int;				/*!< Video and render mode configuration for 264 lines,interlaced PAL mode */
extern GXRenderModeObj TVPal264IntAa;			/*!< Video and render mode configuration for 264 lines,interlaced,antialiased PAL mode */
extern GXRenderModeObj TVPal528Int;				/*!< Video and render mode configuration for 528 lines,interlaced PAL mode */
extern GXRenderModeObj TVPal528IntDf;			/*!< Video and render mode configuration for 528 lines,interlaced,doublefield PAL mode */
extern GXRenderModeObj TVPal524IntAa;			/*!< Video and render mode configuration for 524 lines,interlaced,doublefield,antialiased PAL mode */
extern GXRenderModeObj TVPal576IntDfScale;
extern GXRenderModeObj TVPal528Prog;
extern GXRenderModeObj TVPal528ProgSoft;
extern GXRenderModeObj TVPal524ProgAa;
extern GXRenderModeObj TVPal576ProgScale;
extern GXRenderModeObj TVEurgb60Hz240Ds;
extern GXRenderModeObj TVEurgb60Hz240DsAa;
extern GXRenderModeObj TVEurgb60Hz240Int;
extern GXRenderModeObj TVEurgb60Hz240IntAa;
extern GXRenderModeObj TVEurgb60Hz480Int;
extern GXRenderModeObj TVEurgb60Hz480IntDf;
extern GXRenderModeObj TVEurgb60Hz480IntAa;
extern GXRenderModeObj TVEurgb60Hz480Prog;
extern GXRenderModeObj TVEurgb60Hz480ProgSoft;
extern GXRenderModeObj TVEurgb60Hz480ProgAa;