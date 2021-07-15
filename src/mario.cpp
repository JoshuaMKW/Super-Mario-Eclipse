#include "MTX.h"
#include "math.h"
#include "types.h"

#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

// 0x8024D2C4
/*
TMario* updateMario(TMario* player) {
    if (SME::TGlobals::PlayerData.mCurPlayerID[i] != prevPlayerID) {
        u32* marioVolumeData = (u32*)getVolume__13JKRFileLoaderFPCc("mario");
        u32* marioDataArray = (u32*)*(u32*)ARCBufferMario;

        if (marioDataArray[SME::TGlobals::PlayerData.mCurPlayerID[i]]) {
            __dt__13JKRMemArchiveFv(marioVolumeData);
            __ct__13JKRMemArchiveFPvUl15JKRMemBreakFlag(marioVolumeData,
marioDataArray[i], 0, 0);

            initModel__6TMarioFv(player);
            initMirrorModel__6TMarioFv(player);
        }
    }
    asm("lhz 0, 0x114 (3)");
    return player;
}
*/

// extern -> SME.cpp
// 0x802558A4
void SME::Patch::Mario::addVelocity(TMario *player, f32 velocity) {
  player->mForwardSpeed += velocity;
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (!onYoshi__6TMarioCFv(player)) {
    if (player->mForwardSpeed >
        (99.0f * playerParams->getParams()->mSpeedMultiplier.get()))
      player->mForwardSpeed =
          (99.0f * playerParams->getParams()->mSpeedMultiplier.get());
  } else {
    if (player->mForwardSpeed > 99.0f)
      player->mForwardSpeed = 99.0f;
  }
}

/* MASTER MARIO.CPP UPDATER */

u32 SME::Patch::Mario::updateContexts(TMario *player) {
  SME::Patch::Collision::updateCollisionContext(player);
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (!playerParams->isMario()) {
    playerParams->mIsClimbTired = false;
    return 1;
  }

  bool checkClimbContext = false;

  if ((player->mState & static_cast<u32>(TMario::State::AIRBORN)) == 0 &&
      (player->mState & 0x1C0) != 320)
    playerParams->mClimbTiredTimer = 0;
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
      if (playerParams->mClimbTiredTimer ==
          player->mDeParams.mNoFreezeTime.get() / 5) {
        player->mActionState |= 0x8000;
        playerParams->mClimbTiredTimer = 0;
        playerParams->mIsClimbTired = false;
      } else {
        if (SME::Util::Math::lerp<f32>(
                0.0f, 1.0f,
                static_cast<f32>(playerParams->mClimbTiredTimer) /
                    player->mDeParams.mNoFreezeTime.get()) > 0.9f) {
          if (!playerParams->mIsClimbTired)
            startVoice__6TMarioFUl(
                player, static_cast<u32>(TMario::Voice::FALL_LEDGE_GRAB));

          playerParams->mIsClimbTired = true;
        } else
          playerParams->mIsClimbTired = false;

        playerParams->mClimbTiredTimer += 1;
      }
    }

    if (player->mCeilingAbove >= 9999990.0f && (player->mState & 0x200000) != 0)
      player->mActionState |= 0x8000; // patch upwarps
  }
  return 1;
}

/* NPC CARRY CODE */

// 0x8029A87C
u32 SME::Patch::Mario::carryOrTalkNPC(TBaseNPC *npc) {
  const SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(gpMarioAddress);

  if (!playerParams->isMario())
    return isNowCanTaken__8TBaseNPCCFv();

  if ((*(u32 *)(&npc->mStateFlags) & 0x840007) != 0)
    return 0;

  if (gpMarioAddress->mState == static_cast<u32>(TMario::State::IDLE))
    return 0;

  bool oldTake = npc->mStateFlags.mCanBeTaken;
  npc->mStateFlags.mCanBeTaken = playerParams->getParams()->mCanHoldNPCs.get();

  u32 ret = isNowCanTaken__8TBaseNPCCFv(npc);

  npc->mStateFlags.mCanBeTaken = oldTake;
  return ret;
}

// 0x802815F0
bool SME::Patch::Mario::canGrabAtNPC() {
  TBaseNPC *npc;
  SME_FROM_GPR(30, npc);

  const SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(gpMarioAddress);

  if (!playerParams->isMario())
    return npc->mStateFlags.mCanBeTaken;

  return (playerParams->getParams()->mCanHoldNPCs.get() &&
          gpMarioAddress->mState == static_cast<u32>(TMario::State::IDLE)) ||
         npc->mStateFlags.mCanBeTaken;
}

// 0x80207430
bool SME::Patch::Mario::canCarryNPC() {
  TBaseNPC *npc;
  SME_FROM_GPR(29, npc);

  const SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(gpMarioAddress);

  if (!playerParams->isMario())
    return npc->mStateFlags.mCanBeTaken;

  return (playerParams->getParams()->mCanHoldNPCs.get() &&
          gpMarioAddress->mState == static_cast<u32>(TMario::State::IDLE)) ||
         npc->mStateFlags.mCanBeTaken;
}

// extern -> SME.cpp
// 0x802145F0
TMario *SME::Patch::Mario::scaleNPCThrowLength(TMario *player, float *params) {
  f32 _f11;
  SME_FROM_FPR(11, _f11);

  _f11 = params[0x1D0 / 4];
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario())
    _f11 *= playerParams->getParams()->mThrowPowerMultiplier.get() *
            Util::Math::scaleLinearAtAnchor<f32>(
                playerParams->getParams()->mSizeMultiplier.get(), 0.5f, 1.0f);

  if (player->mState == static_cast<u32>(TMario::State::NPC_THROW) ||
      player->mState == static_cast<u32>(TMario::State::NPC_JUMPTHROW)) {
    _f11 *= 4.0f;
  }

  SME_TO_FPR(11, _f11);
  return player;
}

// extern -> SME.cpp
// 0x8021463C
u32 SME::Patch::Mario::scaleNPCThrowHeight(u32 _r3, f32 z, f32 y) {
  TBaseNPC *npc;
  SME_FROM_GPR(31, npc);

  TMario *player = (TMario *)npc->mPrevHolder;
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario())
    y *= playerParams->getParams()->mThrowPowerMultiplier.get() *
         Util::Math::scaleLinearAtAnchor<f32>(
             playerParams->getParams()->mSizeMultiplier.get(), 0.5f, 1.0f);

  if (player->mState == static_cast<u32>(TMario::State::NPC_THROW) ||
      player->mState == static_cast<u32>(TMario::State::NPC_JUMPTHROW))
    y *= 4.0f;

  npc->mSpeed.y = y;
  npc->mSpeed.z = z;

  return _r3;
}

/* TREE CLIMB CODE */

// extern -> SME.cpp
// 0x80261C3C
f32 SME::Patch::Mario::getTreeClimbMinFall() {
  TMario *player;
  SME_FROM_GPR(31, player);

  Vec size;
  player->JSGGetScaling(&size);

  return 100.0f * size.y;
}

// extern -> SME.cpp
// 0x802619CC
SME_PURE_ASM void SME::Patch::Mario::getTreeClimbMaxFall() {
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

// extern -> SME.cpp
// 0x80261CF4
SME_PURE_ASM void scaleTreeSlideSpeed() {
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

// extern -> SME.cpp
// 0x8025D588
// 0x8025D63C
// 0x8025D650
// 0x8025DBC4
// 0x8025E38C
void SME::Patch::Mario::getClimbingAnimSpd(TMario *player,
                                           TMario::Animation anim, f32 speed) {
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);
  if (playerParams->mIsClimbTired)
    speed = 6.0f;

  setAnimation__6TMarioFif(player, anim, speed);
}

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
                        "lfs 3, 0x28(30)                                \n\t"
                        "fmuls 0, 0, 3                                  \n\t"
                        "blr                                            \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80261824, 0, 0, 0), scaleRoofMoveDiff);

// extern -> SME.cpp
// 0x802615AC
void SME::Patch::Mario::scaleHangSpeed(TMario *player) {
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);
  player->mForwardSpeed += 1.0f;

  if (playerParams->isMario())
    player->mForwardSpeed =
        Min(player->mForwardSpeed,
            4.0f * playerParams->getParams()->mSpeedMultiplier.get());
  else
    player->mForwardSpeed = Min(player->mForwardSpeed, 4.0f);
}

static bool canHangOnRoof(TBGCheckData *roof /*, u16 colType*/) {
  TMario *player;
  SME_FROM_GPR(30, player);

  u16 colType;
  SME_FROM_GPR(4, colType);

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario() &&
      playerParams->getParams()->mCanClimbWalls.get())
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

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  f32 scale = 0.015625f;

  if (playerParams->isMario())
    scale *= playerParams->getParams()->mSpeedMultiplier.get();

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

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario() &&
      playerParams->getParams()->mCanClimbWalls.get() &&
      player->mController->mButtons.mInput & TMarioGamePad::Buttons::Z)
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

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario() &&
      playerParams->getParams()->mCanClimbWalls.get() &&
      player->mController->mButtons.mInput & TMarioGamePad::Buttons::Z)
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

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario() &&
      playerParams->getParams()->mCanClimbWalls.get() &&
      player->mController->mButtons.mInput & TMarioGamePad::Buttons::Z)
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

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario() &&
      playerParams->getParams()->mCanClimbWalls.get())
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

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario() &&
      playerParams->getParams()->mCanClimbWalls.get())
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

  if (playerParams->isMario())
    canCling =
        wall->mCollisionType == 266 ||
        (playerParams->mParams->Attributes.mCanClimbWalls &&
         player->mController->mButtons.mInput & TMarioGamePad::Buttons::Z &&
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

/* GOOP WALKING CODE */

// extern -> SME.cpp
// 0x8024E288
void SME::Patch::Mario::checkGraffitiAffected(TMario *player) {
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);
  if (!playerParams->isMario()) {
    checkGraffito__6TMarioFv(player);
  } else if (playerParams->getParams()->mGoopAffected.get()) {
    checkGraffito__6TMarioFv(player);
  }
}

// extern -> SME.cpp
// 0x801E4118
void SME::Patch::Mario::rescaleHeldObj(Mtx holderMatrix, Mtx destMatrix) {
  TMapObjBase *heldObj;
  SME_FROM_GPR(31, heldObj);

  Vec holderSize;
  heldObj->mHolder->JSGGetScaling(&holderSize);

  PSMTXCopy(holderMatrix, destMatrix);
  PSMTXScaleApply(destMatrix, destMatrix, 1 / holderSize.x, 1 / holderSize.y,
                  1 / holderSize.z);
}

// 0x80213314
SME_PURE_ASM void SME::Patch::Mario::scaleNPCTalkRadius() {
  asm volatile("lis 3, gpMarioAddress@ha                \n\t"
               "lwz 3, gpMarioAddress@l (3)             \n\t"
               "lfs 0, 0x2C (3)                         \n\t"
               "fmuls 30, 30, 0                         \n\t"
               "lis 3, mPtrSaveNormal__8TBaseNPC@ha     \n\t"
               "lwz 3, mPtrSaveNormal__8TBaseNPC@l (3)  \n\t"
               "blr                                     \n\t");
}

static f32 calcJumpPower(TMario *player, f32 factor, f32 base, f32 jumpPower) {
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);
  const SME::Class::TPlayerParams *params = playerParams->getParams();

  jumpPower *= params->mBaseJumpMultiplier.get();
  jumpPower *= SME::Util::Math::scaleLinearAtAnchor<f32>(
      params->mSizeMultiplier.get() *
          SME::Class::TStageParams::sStageConfig->mPlayerSizeMultiplier.get(),
      0.5f, 1.0f);
  if (player->mState & static_cast<u32>(TMario::State::AIRBORN)) {
    f32 multiplier = params->mMultiJumpMultiplier.get();
    for (u32 i = 1; i < playerParams->mCurJump; ++i) {
      multiplier *= multiplier;
    }
    jumpPower *= multiplier;
    player->mForwardSpeed *= params->mMultiJumpFSpeedMulti.get();
  }
  return Max(base, (base * factor) + jumpPower);
}

// extern -> SME.cpp
// 0x8024E02C
void SME::Patch::Mario::manageExtraJumps(TMario *player) {
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  const s32 jumpsLeft =
      (playerParams->getParams()->mMaxJumps.get() - playerParams->mCurJump);

  if ((player->mState & static_cast<u32>(TMario::State::AIRBORN)) == false ||
      (player->mState & 0x800000) || player->mYoshi->mState == TYoshi::MOUNTED)
    playerParams->mCurJump = 1;
  else if ((player->mController->mButtons.mFrameInput &
            TMarioGamePad::Buttons::A) &&
           jumpsLeft > 0 &&
           player->mState != static_cast<u32>(TMario::State::WALLSLIDE) &&
           player->mState != static_cast<u32>(TMario::State::F_KNCK_H)) {
    u32 state = player->mState;
    u32 voiceID = 0;
    u32 animID = 0;

    if (jumpsLeft == 1) {
      state = static_cast<u32>(TMario::State::TRIPLE_J);
      voiceID = 0x78B6;
      animID = 0x6F;
    } else if (jumpsLeft % 2) {
      state = static_cast<u32>(TMario::State::JUMP);
      voiceID = 0x78B1;
      animID = 0x4D;
    } else {
      state = static_cast<u32>(TMario::State::D_JUMP);
      voiceID = 0x78B6;
      animID = 0x50;
    }

    startVoice__6TMarioFUl(player, voiceID);
    setAnimation__6TMarioFif(player, animID, 1.0f);

    player->mSpeed.y = calcJumpPower(player, 0.25f, player->mSpeed.y, 65.0f);
    player->mPrevState = player->mState;
    player->mState = state;

    playerParams->mCurJump += 1;
  }
  stateMachine__6TMarioFv(player);
}

// extern -> SME.cpp
// 0x80254534
void SME::Patch::Mario::normJumpMultiplier() {
  TMario *player;
  SME_FROM_GPR(30, player);

  player->mSpeed.y = calcJumpPower(player, 0.25f, player->mForwardSpeed, 42.0f);
}

// extern -> SME.cpp
// 0x80256678
void SME::Patch::Mario::checkYSpdForTerminalVelocity() {
  TMario *player;
  SME_FROM_GPR(31, player);

  float terminalVelocity;
  if (SME::TGlobals::getPlayerParams(player)->mCollisionFlags.mIsSpinBounce)
    terminalVelocity = -20.0f * player->mJumpParams.mGravity.get();
  else
    terminalVelocity = -75.0f * player->mJumpParams.mGravity.get();
  player->mSpeed.y = Max(player->mSpeed.y, terminalVelocity);
}

// extern -> SME.cpp
// 0x8025B8C0
f32 SME::Patch::Mario::checkGroundSpeedLimit() {
  TMario *player;
  SME_FROM_GPR(31, player);

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  f32 multiplier = 1.0f;
  if (onYoshi__6TMarioCFv(player)) {
    multiplier *= player->mYoshiParams.mRunYoshiMult.get();
  } else {
    multiplier *= playerParams->getParams()->mSpeedMultiplier.get();
  }
  return multiplier;
}

// extern -> SME.cpp
// 0x8024CC6C
void SME::Patch::Mario::checkJumpSpeedLimit(f32 speed) {
  TMario *player;
  SME_FROM_GPR(31, player);

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  f32 speedCap = 32.0f;
  f32 speedReducer = 0.2f;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player)) {
    speedCap *= playerParams->getParams()->mSpeedMultiplier.get();
  }

  if (speed > speedCap) {
    player->mForwardSpeed = (speed - speedReducer);
  }
}

// extern -> SME.cpp
// 0x8024CC2C
TMario *SME::Patch::Mario::checkJumpSpeedMulti(TMario *player, f32 factor,
                                               f32 max) {
  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        ((factor * playerParams->getParams()->mSpeedMultiplier.get()) * max) +
        player->mForwardSpeed;
    return player;
  } else {
    player->mForwardSpeed = (factor * max) + player->mForwardSpeed;
    return player;
  }
}

static f64 checkSlideSpeedMulti(f32 max, f32 factor) {
  TMario *player;
  SME_FROM_GPR(30, player);

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  f64 slowFactor = 0.5;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player)) {
    slowFactor /= playerParams->getParams()->mSpeedMultiplier.get();
  }

  if (player->mFludd && isEmitting__9TWaterGunFv(player->mFludd)) {
    if (player->mFludd->mCurrentNozzle == TWaterGun::Hover)
      slowFactor /= 1.3;
    else if (player->mFludd->mCurrentNozzle == TWaterGun::Rocket) {
      slowFactor /= 2;
      addVelocity__6TMarioFf(player, 20.0f);
    }
  }
  return slowFactor;
}
SME_PATCH_BL(SME_PORT_REGION(0x8025C3E0, 0, 0, 0), &checkSlideSpeedMulti);
SME_WRITE_32(SME_PORT_REGION(0x8025C3E8, 0, 0, 0), 0xFC4100F2);
SME_WRITE_32(SME_PORT_REGION(0x8025C3EC, 0, 0, 0), 0xFC2300F2);


#if 0
static void checkHoverSpeedMulti(f32 factor, f32 max) {
  TMario *player;
  __asm { mr player, r30}
  ;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        ((factor * playerParams->mParams->Attributes.mSpeedMultiplier) * max) +
        player->mForwardSpeed;
  } else {
    player->mForwardSpeed = (factor * max) + player->mForwardSpeed;
  }
}
kmCall(0x8024AE80, &checkHoverSpeedMulti);
kmWrite32(0x8024AE84, 0x60000000);
#endif

// 8024afe0 <- hover air Y spd

static SME_PURE_ASM void scaleHoverInitYSpd() {
  asm volatile (
    "lfs 0, " SME_STRINGIZE(
      SME_PORT_REGION(-0xEDC, 0, 0, 0)) "(2)  \n\t"
    "lfs 4, 0x28(30)                          \n\t"
    "fmuls 0, 0, 4                            \n\t"
    "blr                                      \n\t"
  );
} 
SME_PATCH_BL(SME_PORT_REGION(0x80254A2C, 0, 0, 0), scaleHoverInitYSpd);

static f32 setBounceYSpeed() {
  TMario *player;
  SME_FROM_GPR(30, player);

  Vec size;
  player->JSGGetScaling(&size);

  return 130.0f * size.y;
}
SME_PATCH_BL(SME_PORT_REGION(0x80254720, 0, 0, 0), &setBounceYSpeed);
SME_WRITE_32(SME_PORT_REGION(0x80254724, 0, 0, 0), 0xD01E00A8);

static SME_PURE_ASM void checkGrabHeight() {
  asm volatile("lfs 0, " SME_STRINGIZE(SME_PORT_REGION(
      -0xEDC, 0, 0, 0)) "(2)\n\t"
                        "lfs 4, 0x28(29)            \n\t"
                        "fcmpo 0, 4, 0              \n\t"
                        "lfs 0, " SME_STRINGIZE(SME_PORT_REGION(
                            -0xEDC, 0, 0,
                            0)) "(2)\n\t"
                                "bgt _skipmul                        \n\t"
                                "fmuls 0, 0, 4                       \n\t"
                                "_skipmul:                           \n\t"
                                "blr                                 \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80256D34, 0, 0, 0), checkGrabHeight);

static SME_PURE_ASM void setCollisionHeight1() {
  asm volatile("lfs 1, " SME_STRINGIZE(
      SME_PORT_REGION(-0xEDC, 0, 0, 0)) "(2)                        \n\t"
                                        "lfs 0, 0x28(22)            \n\t"
                                        "fmuls 1, 0, 1              \n\t"
                                        "blr                        \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x8025696C, 0, 0, 0), setCollisionHeight1);

static SME_PURE_ASM void setCollisionHeight2() {
  asm volatile("lfs 2, " SME_STRINGIZE(
      SME_PORT_REGION(-0xEDC, 0, 0, 0)) "(2)                        \n\t"
                                        "lfs 0, 0x28(29)            \n\t"
                                        "fmuls 2, 0, 2              \n\t"
                                        "blr                        \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80256D14, 0, 0, 0), setCollisionHeight2);

static SME_PURE_ASM void setCollisionHeight3() {
  asm volatile("lfs 0, " SME_STRINGIZE(
      SME_PORT_REGION(-0xEDC, 0, 0, 0)) "(2)                        \n\t"
                                        "lfs 2, 0x28(30)            \n\t"
                                        "fmuls 0, 2, 0              \n\t"
                                        "blr                        \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x802573FC, 0, 0, 0), setCollisionHeight3);

static void setCollisionWidth() {
  TMario *player;
  SME_FROM_GPR(29, player);

  f32 width = 50.0f;

  Vec size;
  player->JSGGetScaling(&size);

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);
  if (playerParams->isMario())
    width *= size.x;

  player->mCollisionXZSize = width;
}
SME_PATCH_BL(SME_PORT_REGION(0x802505F4, 0, 0, 0), setCollisionWidth);

static f32 manageGrabLength() {
  TMario *player;
  SME_FROM_GPR(29, player);

  Vec size;
  player->JSGGetScaling(&size);

  return 60.0f * size.z;
}
SME_PATCH_BL(SME_PORT_REGION(0x80256CE8, 0, 0, 0), manageGrabLength);
SME_WRITE_32(SME_PORT_REGION(0x80256CFC, 0, 0, 0), 0xEC01283C);
SME_WRITE_32(SME_PORT_REGION(0x80256D04, 0, 0, 0), 0xC05E003C);
SME_WRITE_32(SME_PORT_REGION(0x80256D0C, 0, 0, 0), 0xEC0100BC);

static JUtility::TColor getEMarioHealthBarRGBA(TEnemyMario *eMario) {
  JUtility::TColor color;
  const s16 maxHealth = ((s16 *)eMario->mEnemyManager)[0x40 / 2];

  color.set(0xFF, 0x00, 0x00, 0xFF);
  color.g = SME::Util::Math::lerp<u8>(0, 255,
                                      static_cast<float>(eMario->mEnemyHealth) /
                                          static_cast<float>(maxHealth));
  color.r -= color.g;

  return color;
}

// extern -> SME.cpp
// 0x8003FDAC
void SME::Patch::Mario::manageEMarioHealthWrapper(TEnemyMario *eMario,
                                                  Mtx *posMtx) {
  *(JUtility::TColor *)0x8040FA90 = getEMarioHealthBarRGBA(eMario);
  drawHPMeter__11TEnemyMarioFPA4_f(eMario, posMtx);
}