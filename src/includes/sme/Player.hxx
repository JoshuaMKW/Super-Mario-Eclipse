#pragma once

#include "sms/actor/Mario.hxx"
#include "sms/camera/PolarSubCamera.hxx"

namespace SME::Enum {
enum class Player : u8 {
  MARIO,
  LUIGI,
  IL_PIANTISSIMO,
  SHADOW_MARIO,
  DRY_BONES,
  UNKNOWN = 0xFF
};

enum class WarpKind : u8 { SPARKLES, WIPE, INSTANT };
} // namespace SME::Enum

namespace SME::Util::Mario {

bool loadParams();
bool swapBinary(SME::Enum::Player id);
void switchCharacter(TMario *player, SME::Enum::Player id, bool fadeInOut);
SME::Enum::Player getPlayerIDFromInput(u32 input);
SME::Enum::Player getPlayerIDFromInt(u8 id);
void warpToCollisionFace(TMario *player, TBGCheckData *colTriangle,
                         bool isFluid);
void rotatePlayerRelativeToCamera(TMario *player, CPolarSubCamera *other,
                                 Vec2 dir, f32 lerp);

} // namespace SME::Util::Mario