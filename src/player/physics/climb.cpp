#include "MTX.h"
#include "math.h"
#include "types.h"

#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

using namespace SME;

static void updateClimbContext(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (!playerData->isMario()) {
    playerData->mIsClimbTired = false;
    return;
  }

  bool checkClimbContext = false;

  if ((player->mState & static_cast<u32>(TMario::State::AIRBORN)) == 0 &&
      (player->mState & 0x1C0) != 320)
    playerData->mClimbTiredTimer = 0;
  else if ((player->mState & 0x1C0) == 320) {
    if ((player->mState & 0x200000) != 0 && player->mRoofTriangle &&
        player->mRoofTriangle->mCollisionType != 266)
      checkClimbContext =
          player->mState != static_cast<u32>(TMario::State::HANG);
    else if ((player->mState & 0x200000) == 0 && player->mWallTriangle &&
             player->mWallTriangle->mCollisionType != 266)
      checkClimbContext =
          player->mState != static_cast<u32>(TMario::State::HANG);

    if (checkClimbContext) {
      if (playerData->mClimbTiredTimer ==
          player->mDeParams.mNoFreezeTime.get() / 5) {
        player->mActionState |= 0x8000;
        playerData->mClimbTiredTimer = 0;
        playerData->mIsClimbTired = false;
      } else {
        if (Util::Math::lerp<f32>(
                0.0f, 1.0f,
                static_cast<f32>(playerData->mClimbTiredTimer) /
                    player->mDeParams.mNoFreezeTime.get()) > 0.9f) {
          if (!playerData->mIsClimbTired)
            startVoice__6TMarioFUl(
                player, static_cast<u32>(TMario::Voice::FALL_LEDGE_GRAB));

          playerData->mIsClimbTired = true;
        } else
          playerData->mIsClimbTired = false;

        playerData->mClimbTiredTimer += 1;
      }
    }

    if (player->mCeilingAbove >= 9999990.0f && (player->mState & 0x200000) != 0)
      player->mActionState |= 0x8000; // patch upwarps
  }
  return;
}

/* PATCHES */

/* TREE CLIMB CODE */

// extern -> SME.cpp
// 0x80261C3C
static f32 getTreeClimbMinFall() {
  TMario *player;
  SME_FROM_GPR(31, player);

  Vec size;
  player->JSGGetScaling(&size);

  return 100.0f * size.y;
}
SME_PATCH_BL(SME_PORT_REGION(0x80261C3C, 0, 0, 0), getTreeClimbMinFall);
SME_WRITE_32(SME_PORT_REGION(0x80261C40, 0, 0, 0), 0xC05F038C);
SME_WRITE_32(SME_PORT_REGION(0x80261C44, 0, 0, 0), 0xFC020040);

// extern -> SME.cpp
// 0x802619CC
static SME_PURE_ASM void getTreeClimbMaxFall() {
  asm volatile("mflr 0                   \n\t"
               "stw 0, 0x8 (1)           \n\t"
               "stwu 1, -0x10 (1)        \n\t"
               "lfs 3, 0x5C (3)          \n\t"
               "bl _localdata            \n\t"
               ".float 0.2, 1.0          \n\t"
               "_localdata:              \n\t"
               "mflr 11                  \n\t"
               "lfs 0, 0 (11)            \n\t"
               "lfs 2, 4 (11)            \n\t"
               "lfs 4, 0x28 (31)         \n\t"
               "fmuls 4, 4, 0            \n\t"
               "fsubs 2, 2, 0            \n\t"
               "fadds 4, 4, 2            \n\t"
               "fdivs 3, 3, 4            \n\t"
               "lfs 2, 0x14 (31)         \n\t"
               "addi 1, 1, 0x10          \n\t"
               "lwz 0, 0x8 (1)           \n\t"
               "mtlr 0                   \n\t"
               "blr                      \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x802619CC, 0, 0, 0), getTreeClimbMaxFall);
SME_WRITE_32(SME_PORT_REGION(0x802619D0, 0, 0, 0), 0x60000000);

// extern -> SME.cpp
// 0x80261CF4
static SME_PURE_ASM void scaleTreeSlideSpeed() {
  // F2 IS UNSAFE
  asm volatile("mflr 0                      \n\t"
               "stw 0, 0x8 (1)              \n\t"
               "stwu 1, -0x10 (1)           \n\t"
               "lfs 3, 0x5C (3)             \n\t"
               "bl _localdata_              \n\t"
               ".float 0.00195313, -16      \n\t"
               "_localdata_:                \n\t"
               "mflr 11                     \n\t"
               "lfs 0, 0 (11)               \n\t"
               "lfs 1, 4 (11)               \n\t"
               "lfs 4, 0xB18 (31)           \n\t"
               "fmuls 4, 4, 0               \n\t"
               "fcmpo 0, 2, 1               \n\t"
               "li 3, 1                     \n\t"
               "blt _exit666                \n\t"
               "li 3, 0                     \n\t"
               "stw 3, 0xA8 (31)            \n\t"
               "_exit666:                   \n\t"
               "addi 1, 1, 0x10             \n\t"
               "lwz 0, 0x8 (1)              \n\t"
               "mtlr 0                      \n\t"
               "blr                         \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80261CF4, 0, 0, 0), scaleTreeSlideSpeed);
SME_WRITE_32(SME_PORT_REGION(0x80261CF8, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x80261CFC, 0, 0, 0), 0x41820070);

/* GLOBAL CLIMB CODE */

void getClimbingAnimSpd(TMario *player, TMario::Animation anim, f32 speed) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  if (playerData->mIsClimbTired)
    speed = 6.0f;

  setAnimation__6TMarioFif(player, anim, speed);
}
// SME_PATCH_BL(SME_PORT_REGION(0x8025D588, 0, 0, 0), getClimbingAnimSpd);
// SME_PATCH_BL(SME_PORT_REGION(0x8025D63C, 0, 0, 0), getClimbingAnimSpd);
// SME_PATCH_BL(SME_PORT_REGION(0x8025D650, 0, 0, 0), getClimbingAnimSpd);
// SME_PATCH_BL(SME_PORT_REGION(0x8025DBC4, 0, 0, 0), getClimbingAnimSpd);
// SME_PATCH_BL(SME_PORT_REGION(0x8025E38C, 0, 0, 0), getClimbingAnimSpd);

/* ROOF HANG CODE */

static SME_PURE_ASM void scaleRoofClimbHeight(f32 yCoord, f32 speed) {
  asm volatile("lfs 0, " SME_STRINGIZE(SME_PORT_REGION(
      -0xDE0, 0, 0, 0)) "(2)        \n\t"
                        "lfs 3, 0x28(31)                                \n\t"
                        "fmuls 0, 0, 3                                  \n\t"
                        "blr                                            \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x8025D66C, 0, 0, 0), scaleRoofClimbHeight);

static SME_PURE_ASM void scaleRoofSquashedHeight() {
  asm volatile("lfs 3, " SME_STRINGIZE(SME_PORT_REGION(
      -0xDE0, 0, 0, 0)) "(2)        \n\t"
                        "lfs 5, 0x28(30)                                \n\t"
                        "fmuls 3, 5, 3                                  \n\t"
                        "blr                                            \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x802617EC, 0, 0, 0), scaleRoofSquashedHeight);

static SME_PURE_ASM void scaleRoofMoveDiff() {
  asm volatile("lfs 0, " SME_STRINGIZE(SME_PORT_REGION(
      -0xD7C, 0, 0, 0)) "(2)        \n\t"
                        "lfs 10, 0x28(30)                                \n\t"
                        "fmuls 0, 0, 10                                  \n\t"
                        "blr                                             \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80261824, 0, 0, 0), scaleRoofMoveDiff);

static void scaleHangSpeed(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  player->mForwardSpeed += 1.0f;

  if (playerData->isMario())
    player->mForwardSpeed =
        Min(player->mForwardSpeed,
            4.0f * playerData->getParams()->mSpeedMultiplier.get());
  else
    player->mForwardSpeed = Min(player->mForwardSpeed, 4.0f);
}
// SME_PATCH_BL(SME_PORT_REGION(0x802615AC, 0, 0, 0), scaleHangSpeed);
// SME_WRITE_32(SME_PORT_REGION(0x802615B0, 0, 0, 0), 0x60000000);

static bool canHangOnRoof(TBGCheckData *roof /*, u16 colType*/) {
  TMario *player;
  SME_FROM_GPR(30, player);

  u16 colType;
  SME_FROM_GPR(4, colType);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (playerData->isMario() && playerData->getParams()->mCanClimbWalls.get())
    return true;

  return colType == 266;
}
SME_WRITE_32(SME_PORT_REGION(0x802617C0, 0, 0, 0), 0xA0830000);
SME_PATCH_BL(SME_PORT_REGION(0x802617C4, 0, 0, 0), canHangOnRoof);
SME_WRITE_32(SME_PORT_REGION(0x802617C8, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x802617CC, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x802617D0, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x802617D4, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x802617D8, 0, 0, 0), 0x2C030000);

/* WALL CLIMB CODE */

// 8025e560 <- possibly resize float by char size mul for height
// 8025e5bc <- gate sound?

// 80415DCC <- this float controls how far into the wall mario is placed on
// first grab 80415DD4 <- this float controls how high mario is placed from the
// floor on first grab

// 80415DEC <- this float controls the climbing speed, scale accordingly

#if 0
static f32 scaleClimbSpeed(f32 speed) {
  TMario *player;
  SME_FROM_GPR(30, player);


  f32 _f0;
  f32 _f3;
  f32 _f7;

  SME_FROM_FPR(0, _f0);
  SME_FROM_FPR(3, _f3);
  SME_FROM_FPR(7, _f7);

  Class::TPlayerData *playerData =
      TGlobals::getPlayerData(player);

  f32 scale = 0.015625f;

  if (playerData->isMario())
    scale *= playerData->getParams()->mSpeedMultiplier.get();

  SME_TO_FPR(0, _f0);
  SME_TO_FPR(3, _f3);
  SME_TO_FPR(7, _f7);

  return scale;
}
SME_PATCH_BL(SME_PORT_REGION(0x8025E19C, 0, 0, 0), scaleClimbSpeed);
SME_WRITE_32(SME_PORT_REGION(0x8025E1A0, 0, 0, 0), 0x807E0010);
SME_WRITE_32(SME_PORT_REGION(0x8025E1A4, 0, 0, 0), 0x801E0014);
SME_WRITE_32(SME_PORT_REGION(0x8025E1C4, 0, 0, 0), 0xC0440014);
SME_PATCH_BL(SME_PORT_REGION(0x8025E218, 0, 0, 0), scaleClimbSpeed);
SME_WRITE_32(SME_PORT_REGION(0x8025E21C, 0, 0, 0), 0x7FC3F378);

static TBGCheckData *checkClimbingWallPlane(TMario *player,
                                            JGeometry::TVec3<float> pos, f32 w,
                                            f32 h) {
  return (TBGCheckData *)checkWallPlane__6TMarioFP3Vecff(
      player, pos, w * player->mSize.z, h * player->mSize.y);
}
kmCall(0x8025DD84, &checkClimbingWallPlane);
kmCall(0x8025DEB8, &checkClimbingWallPlane);
kmCall(0x8025E184, &checkClimbingWallPlane);
kmCall(0x8025E2D0, &checkClimbingWallPlane);
kmCall(0x8025E2E8, &checkClimbingWallPlane);
#endif

#if 0
static bool canJumpClingWall(TBGCheckData *wall, u16 colType) {
  TMario *player;
  SME_FROM_GPR(28, player);

  if (colType == 266)
    return true;

  Class::TPlayerData *playerData =
      TGlobals::getPlayerData(player);

  if (playerData->isMario() &&
      playerData->getParams()->mCanClimbWalls.get() &&
      player->mController->mButtons.mInput & TMarioGamePad::EButtons::Z)
    return true;

  return false;
}
SME_WRITE_32(SME_PORT_REGION(0x8024C888, 0, 0, 0), 0xA0830000);
SME_PATCH_BL(SME_PORT_REGION(0x8024C88C, 0, 0, 0), canJumpClingWall);
SME_WRITE_32(SME_PORT_REGION(0x8024C890, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8024C894, 0, 0, 0), 0x807C00D8);
SME_WRITE_32(SME_PORT_REGION(0x8024C898, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8024C89C, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8024C8A0, 0, 0, 0), 0x60000000);

static bool canUnkActionWall(TBGCheckData *wall, u16 colType) {
  TMario *player;
  SME_FROM_GPR(22, player);

  if (colType == 266)
    return true;

  Class::TPlayerData *playerData =
      TGlobals::getPlayerData(player);

  if (playerData->isMario() &&
      playerData->getParams()->mCanClimbWalls.get() &&
      player->mController->mButtons.mInput & TMarioGamePad::EButtons::Z)
    return true;

  return false;
}
SME_WRITE_32(SME_PORT_REGION(0x80256A3C, 0, 0, 0), 0xA0830000);
SME_PATCH_BL(SME_PORT_REGION(0x80256A40, 0, 0, 0), canUnkActionWall);
SME_WRITE_32(SME_PORT_REGION(0x80256A44, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80256A48, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80256A4C, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80256A50, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80256A54, 0, 0, 0), 0x2C030000);

static bool canRunClingWall(TBGCheckData *wall, u16 colType) {
  TMario *player;
  SME_FROM_GPR(31, player);

  if (colType == 266)
    return true;

  Class::TPlayerData *playerData =
      TGlobals::getPlayerData(player);

  if (playerData->isMario() &&
      playerData->getParams()->mCanClimbWalls.get() &&
      player->mController->mButtons.mInput & TMarioGamePad::EButtons::Z)
    return true;

  return false;
}
SME_WRITE_32(SME_PORT_REGION(0x8025B1FC, 0, 0, 0), 0xA0830000);
SME_PATCH_BL(SME_PORT_REGION(0x8025B200, 0, 0, 0), canRunClingWall);
SME_WRITE_32(SME_PORT_REGION(0x8025B204, 0, 0, 0), 0x807F00D8);
SME_WRITE_32(SME_PORT_REGION(0x8025B208, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8025B20C, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025B210, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025B214, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025B218, 0, 0, 0), 0x60000000);

static bool canMoveOnWall1(u16 colType) {
  TMario *player;
  SME_FROM_GPR(30, player);

  TBGCheckData *wall;
  SME_FROM_GPR(29, wall);

  if (colType == 266)
    return true;

  Class::TPlayerData *playerData =
      TGlobals::getPlayerData(player);

  if (playerData->isMario() &&
      playerData->getParams()->mCanClimbWalls.get())
    return true;

  return false;
}
SME_WRITE_32(SME_PORT_REGION(0x8025E2F4, 0, 0, 0), 0xA0830000);
SME_PATCH_BL(SME_PORT_REGION(0x8025E2F8, 0, 0, 0), canRunClingWall);
SME_WRITE_32(SME_PORT_REGION(0x8025E2FC, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8025E300, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025E304, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025E308, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025E30C, 0, 0, 0), 0x60000000);

static bool canMoveOnWall2(TBGCheckData *wall, u16 colType) {
  TMario *player;
  SME_FROM_GPR(30, player);

  if (colType == 266)
    return true;

  Class::TPlayerData *playerData =
      TGlobals::getPlayerData(player);

  if (playerData->isMario() &&
      playerData->getParams()->mCanClimbWalls.get())
    return true;

  return false;
}
kmCall(0x8025E31C, &canMoveOnWall2);
kmWrite32(0x8025E320, 0x2C040000);
kmWrite32(0x8025E324, 0x4182000C);

static TBGCheckData *canClimbUnderwater(TBGCheckData *wall) {
  bool canCling;
  __asm { mr canCling, r4}
  ;

  TMario *player;
  __asm { mr player, r31}
  ;

  if (playerData->isMario())
    canCling =
        wall->mCollisionType == 266 ||
        (playerData->mParams->Attributes.mCanClimbWalls &&
         player->mController->mButtons.mInput & TMarioGamePad::EButtons::Z &&
         wall->mCollisionType != 5);
  else
    canCling = wall->mCollisionType == 266;

  __asm volatile {mr r4, canCling};

  return wall;
}
kmCall(0x80272660, &canClimbUnderwater);
kmWrite32(0x80272664, 0x2C040000);
kmWrite32(0x80272668, 0x4182000C);
#endif