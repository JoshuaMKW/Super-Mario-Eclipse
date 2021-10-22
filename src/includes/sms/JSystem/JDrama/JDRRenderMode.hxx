#pragma once

#include "GX_Types.h"
#include "types.h"

namespace JDrama {

bool IsEqualRenderModeVIParams(const GXRenderModeObj &,
                               const GXRenderModeObj &);
bool IsRenderModeHalfAspectRatio(const GXRenderModeObj &);
f32 GetRenderModeYScale(const GXRenderModeObj &);
void CalcRenderModeXFBHeight(GXRenderModeObj *, u16);
void CalcRenderModeVIXOrigin(GXRenderModeObj *);
void CalcRenderModeVIYOrigin(GXRenderModeObj *);
void CopyRenderModeSamplePattern(GXRenderModeObj *, const u8 *[2]);
void CopyRenderModeVFilter(GXRenderModeObj *, const u8 *);

} // namespace JDrama