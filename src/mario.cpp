#include "MTX.h"
#include "math.h"
#include "types.h"

#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

using namespace SME;

// 0x8024D2C4
/*
TMario* updateMario(TMario* player) {
    if (TGlobals::PlayerData.mCurPlayerID[i] != prevPlayerID) {
        u32* marioVolumeData = (u32*)getVolume__13JKRFileLoaderFPCc("mario");
        u32* marioDataArray = (u32*)*(u32*)ARCBufferMario;

        if (marioDataArray[TGlobals::PlayerData.mCurPlayerID[i]]) {
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
void Patch::Mario::addVelocity(TMario *player, f32 velocity) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (!onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        Min(player->mForwardSpeed + velocity,
            (playerData->mMaxAddVelocity *
             playerData->getParams()->mSpeedMultiplier.get()) *
                playerData->mSlideSpeedMultiplier);
  } else {
    player->mForwardSpeed =
        Min(player->mForwardSpeed + velocity,
            playerData->mMaxAddVelocity * playerData->mSlideSpeedMultiplier);
  }
}

/* MASTER MARIO.CPP UPDATER */

u32 Patch::Mario::updateContexts(TMario *player) {
  Patch::Collision::updateCollisionContext(player);
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (!playerData->isMario()) {
    playerData->mIsClimbTired = false;
    return 1;
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
  return 1;
}

/* NPC CARRY CODE */

// 0x8029A87C
u32 Patch::Mario::carryOrTalkNPC(TBaseNPC *npc) {
  const Class::TPlayerData *playerData =
      TGlobals::getPlayerData(gpMarioAddress);

  if (!playerData->isMario())
    return isNowCanTaken__8TBaseNPCCFv(npc);

  if ((*(u32 *)(&npc->mStateFlags.asFlags) & 0x840007) != 0)
    return 0;

  if (gpMarioAddress->mState == static_cast<u32>(TMario::State::IDLE))
    return 0;

  bool oldTake = npc->mStateFlags.asFlags.mCanBeTaken;
  npc->mStateFlags.asFlags.mCanBeTaken = playerData->getParams()->mCanHoldNPCs.get();

  u32 ret = isNowCanTaken__8TBaseNPCCFv(npc);

  npc->mStateFlags.asFlags.mCanBeTaken = oldTake;
  return ret;
}

// 0x802815F0
bool Patch::Mario::canGrabAtNPC() {
  TBaseNPC *npc;
  SME_FROM_GPR(30, npc);

  const Class::TPlayerData *playerData =
      TGlobals::getPlayerData(gpMarioAddress);

  if (!playerData->isMario())
    return npc->mStateFlags.asFlags.mCanBeTaken;

  return (playerData->getParams()->mCanHoldNPCs.get() &&
          gpMarioAddress->mState != static_cast<u32>(TMario::State::IDLE)) ||
         npc->mStateFlags.asFlags.mCanBeTaken;
}

// 0x80207430
bool Patch::Mario::canCarryNPC() {
  TBaseNPC *npc;
  SME_FROM_GPR(29, npc);

  const Class::TPlayerData *playerData =
      TGlobals::getPlayerData(gpMarioAddress);

  if (!playerData->isMario())
    return npc->mStateFlags.asFlags.mCanBeTaken;

  return (playerData->getParams()->mCanHoldNPCs.get() &&
          gpMarioAddress->mState != static_cast<u32>(TMario::State::IDLE)) ||
         npc->mStateFlags.asFlags.mCanBeTaken;
}

// extern -> SME.cpp
// 0x802145F0
TMario *Patch::Mario::scaleNPCThrowLength(TMario *player, float *params) {
  f32 _f11;
  SME_FROM_FPR(11, _f11);

  _f11 = params[0x1D0 / 4];
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (playerData->isMario())
    _f11 *= playerData->getParams()->mThrowPowerMultiplier.get() *
            Util::Math::scaleLinearAtAnchor<f32>(
                playerData->getParams()->mSizeMultiplier.get(), 0.5f, 1.0f);

  if (player->mState == static_cast<u32>(TMario::State::NPC_THROW) ||
      player->mState == static_cast<u32>(TMario::State::NPC_JUMPTHROW)) {
    _f11 *= 4.0f;
  }

  SME_TO_FPR(11, _f11);
  return player;
}

// extern -> SME.cpp
// 0x8021463C
u32 Patch::Mario::scaleNPCThrowHeight(u32 _r3, f32 z, f32 y) {
  TBaseNPC *npc;
  SME_FROM_GPR(31, npc);

  TMario *player = (TMario *)npc->mPrevHolder;
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (playerData->isMario())
    y *= playerData->getParams()->mThrowPowerMultiplier.get() *
         Util::Math::scaleLinearAtAnchor<f32>(
             playerData->getParams()->mSizeMultiplier.get(), 0.5f, 1.0f);

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
f32 Patch::Mario::getTreeClimbMinFall() {
  TMario *player;
  SME_FROM_GPR(31, player);

  Vec size;
  player->JSGGetScaling(&size);

  return 100.0f * size.y;
}

// extern -> SME.cpp
// 0x802619CC
SME_PURE_ASM void Patch::Mario::getTreeClimbMaxFall() {
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
void Patch::Mario::getClimbingAnimSpd(TMario *player, TMario::Animation anim,
                                      f32 speed) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  if (playerData->mIsClimbTired)
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
                        "lfs 10, 0x28(30)                                \n\t"
                        "fmuls 0, 0, 10                                  \n\t"
                        "blr                                             \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80261824, 0, 0, 0), scaleRoofMoveDiff);

// extern -> SME.cpp
// 0x802615AC
void Patch::Mario::scaleHangSpeed(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  player->mForwardSpeed += 1.0f;

  if (playerData->isMario())
    player->mForwardSpeed =
        Min(player->mForwardSpeed,
            4.0f * playerData->getParams()->mSpeedMultiplier.get());
  else
    player->mForwardSpeed = Min(player->mForwardSpeed, 4.0f);
}

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

  Class::TPlayerData *playerData =
      TGlobals::getPlayerData(player);

  if (playerData->isMario() &&
      playerData->getParams()->mCanClimbWalls.get() &&
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

  Class::TPlayerData *playerData =
      TGlobals::getPlayerData(player);

  if (playerData->isMario() &&
      playerData->getParams()->mCanClimbWalls.get() &&
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
void Patch::Mario::checkGraffitiAffected(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  if (!playerData->isMario()) {
    checkGraffito__6TMarioFv(player);
  } else if (playerData->getParams()->mGoopAffected.get()) {
    checkGraffito__6TMarioFv(player);
  }
}

// extern -> SME.cpp
// 0x801E4118
void Patch::Mario::rescaleHeldObj(Mtx holderMatrix, Mtx destMatrix) {
  TMapObjBase *heldObj;
  SME_FROM_GPR(31, heldObj);

  Vec holderSize;
  heldObj->mHolder->JSGGetScaling(&holderSize);

  PSMTXCopy(holderMatrix, destMatrix);
  PSMTXScaleApply(destMatrix, destMatrix, 1 / holderSize.x, 1 / holderSize.y,
                  1 / holderSize.z);
}

// 0x80213314
SME_PURE_ASM void Patch::Mario::scaleNPCTalkRadius() {
  asm volatile("lis 3, gpMarioAddress@ha                \n\t"
               "lwz 3, gpMarioAddress@l (3)             \n\t"
               "lfs 0, 0x2C (3)                         \n\t"
               "fmuls 30, 30, 0                         \n\t"
               "lis 3, mPtrSaveNormal__8TBaseNPC@ha     \n\t"
               "lwz 3, mPtrSaveNormal__8TBaseNPC@l (3)  \n\t"
               "blr                                     \n\t");
}

static void doProcessJumpState(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  TMarioGamePad *controller = player->mController;

  const f32 stickMagnitude = controller->mControlStick.mLengthFromNeutral;

#if 1
  if (player->mState != static_cast<u32>(TMario::State::JUMPSPINR) &&
      player->mState != static_cast<u32>(TMario::State::JUMPSPINL))
    playerData->mCollisionFlags.mIsSpinBounce = false;

  if (playerData->mCollisionFlags.mIsSpinBounce) {
    if (stickMagnitude > 0.1f) {
      Util::Mario::rotatePlayerRelativeToCamera(
          player, gpCamera,
          {controller->mControlStick.mStickX, controller->mControlStick.mStickY}, 1.0f);
    } else {
      player->mForwardSpeed *= 0.98f;
    }
  }
#else
  Util::Mario::rotatePlayerRelativeToCamera(
      player, gpCamera,
      {controller->mControlStick.mStickX, controller->mControlStick.mStickY}, 1.0f);
#endif

  jumpMain__6TMarioFv(player);
}
SME_PATCH_BL(SME_PORT_REGION(0x80250138, 0, 0, 0), doProcessJumpState);

static f32 calcJumpPower(TMario *player, f32 factor, f32 base, f32 jumpPower) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  const Class::TPlayerParams *params = playerData->getParams();

  jumpPower *= params->mBaseJumpMultiplier.get();
  jumpPower *= Util::Math::scaleLinearAtAnchor<f32>(
      params->mSizeMultiplier.get() *
          Class::TStageParams::sStageConfig->mPlayerSizeMultiplier.get(),
      0.5f, 1.0f);
  if (player->mState & static_cast<u32>(TMario::State::AIRBORN)) {
    f32 multiplier = params->mMultiJumpMultiplier.get();
    for (u32 i = 1; i < playerData->mCurJump; ++i) {
      multiplier *= multiplier;
    }
    jumpPower *= multiplier;
    player->mForwardSpeed *= params->mMultiJumpFSpeedMulti.get();
  }
  return Max(base, (base * factor) + jumpPower);
}

// extern -> SME.cpp
// 0x8024E02C
static void manageCustomJumps(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  const s32 jumpsLeft =
      (playerData->getParams()->mMaxJumps.get() - playerData->mCurJump);

  if ((player->mState & static_cast<u32>(TMario::State::AIRBORN)) == false ||
      (player->mState & 0x800000) ||
      player->mYoshi->mState == TYoshi::MOUNTED ||
      player->mState == static_cast<u32>(TMario::State::SLIP_JUMP) ||
      player->mState == static_cast<u32>(TMario::State::THROWN) ||
      player->mAttributes.mIsGameOver) {
    playerData->mCurJump = 1;
  } else if ((player->mController->mButtons.mFrameInput &
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

    TMarioGamePad *controller = player->mController;
    const f32 stickMagnitude = controller->mControlStick.mLengthFromNeutral;

    if (stickMagnitude > 0.1f) {
      Util::Mario::rotatePlayerRelativeToCamera(
          player, gpCamera,
          {controller->mControlStick.mStickX,
           controller->mControlStick.mStickY}, 1.0f);
    }

    player->mForwardSpeed *= stickMagnitude;
    player->mSpeed.y = calcJumpPower(player, 0.25f, player->mSpeed.y, 65.0f);
    player->mPrevState = player->mState;
    player->mState = state;

    playerData->mIsLongJumping = false;
    playerData->mCurJump += 1;
  }
  stateMachine__6TMarioFv(player);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024E02C, 0, 0, 0), manageCustomJumps);

static void setJumpOrLongJump(TMario *player, u32 state, u32 unk_0) {
  constexpr u32 LongJumpSpecifier = TMarioGamePad::Buttons::Z;
  constexpr f32 LongJumpMinSpeed = 10.0f;

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  TMarioGamePad::CButton &buttons = player->mController->mButtons;

  const bool isValidState =
      !(player->mState & static_cast<u32>(TMario::State::AIRBORN)) &&
      !(player->mState & static_cast<u32>(TMario::State::WATERBORN));

  playerData->mIsLongJumping = false;
  if ((buttons.mInput & LongJumpSpecifier) == LongJumpSpecifier &&
      (buttons.mFrameInput & TMarioGamePad::Buttons::A) &&
      player->mForwardSpeed > LongJumpMinSpeed && isValidState) {
    playerData->mIsLongJumping = !player->mAttributes.mHasFludd &&
                                 playerData->isMario() &&
                                 (player->mActionState & 0x8) == 0;
    state = static_cast<u32>(TMario::State::JUMP);
  }

  setStatusToJumping__6TMarioFUlUl(player, state, unk_0);
}
SME_PATCH_BL(SME_PORT_REGION(0x802541BC, 0, 0, 0), setJumpOrLongJump);

static void processJumpOrLongJumpAnim(TMario *player, int state, int anim,
                                      int unk_0) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (playerData->mIsLongJumping) {
    anim = 0xF6;
  }

  jumpingBasic__6TMarioFiii(player, state, anim, unk_0);
}
SME_PATCH_BL(SME_PORT_REGION(0x80249554, 0, 0, 0), processJumpOrLongJumpAnim);

static void processJumpOrLongJump() {
  TMario *player;
  SME_FROM_GPR(30, player);

  constexpr f32 LongJumpSpeedForward = 36.0f;
  constexpr f32 LongJumpSpeedUp = 50.0f;

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (!playerData->mIsLongJumping) {
    player->mSpeed.y =
        calcJumpPower(player, 0.25f, player->mForwardSpeed, 42.0f);
    return;
  }

  startVoice__6TMarioFUl(player, TMario::Voice::TRIPLE_JUMP);

  player->mSpeed.y +=
      LongJumpSpeedUp * playerData->getParams()->mBaseJumpMultiplier.get();
  player->mForwardSpeed +=
      LongJumpSpeedForward * playerData->getParams()->mSpeedMultiplier.get();
  player->mPrevState = player->mState;
  player->mState = static_cast<u32>(TMario::State::JUMP);
}
SME_PATCH_BL(SME_PORT_REGION(0x80254534, 0, 0, 0), processJumpOrLongJump);
SME_WRITE_32(SME_PORT_REGION(0x80254538, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025453C, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80254540, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80254544, 0, 0, 0), 0x60000000);

static bool checkDivingWhenLongJumping(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  return onYoshi__6TMarioCFv(player) || playerData->mIsLongJumping;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024C394, 0, 0, 0), checkDivingWhenLongJumping);

static bool checkRotatingWhenLongJumping(TMario *player, int *unk_0) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  return checkStickRotate__6TMarioFPi(player, unk_0) &&
         !playerData->mIsLongJumping;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024C3F8, 0, 0, 0),
             checkRotatingWhenLongJumping);

static bool checkQuickFallWhenLongJumping() {
  TMario *player;
  SME_FROM_GPR(30, player);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  return ((player->mActionState & 0x80) != 0) || playerData->mIsLongJumping ||
         playerData->mCollisionFlags.mIsSpinBounce;
}
SME_PATCH_BL(SME_PORT_REGION(0x802565D4, 0, 0, 0),
             checkQuickFallWhenLongJumping);
SME_WRITE_32(SME_PORT_REGION(0x802565D8, 0, 0, 0), 0x2C030000);

static bool preserveRegisterCheckQuickFall() {
  TMario *player;
  SME_FROM_GPR(31, player);

  return player->mState == static_cast<u32>(TMario::State::JUMPSPINR) ||
         player->mState == static_cast<u32>(TMario::State::JUMPSPINL);
}
SME_PATCH_BL(SME_PORT_REGION(0x80256618, 0, 0, 0),
             preserveRegisterCheckQuickFall);
SME_WRITE_32(SME_PORT_REGION(0x8025661C, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x80256620, 0, 0, 0), 0x41820024);

// extern -> SME.cpp
// 0x80256678
void Patch::Mario::checkYSpdForTerminalVelocity() {
  TMario *player;
  SME_FROM_GPR(31, player);

  float terminalVelocity;
  if (TGlobals::getPlayerData(player)->mCollisionFlags.mIsSpinBounce)
    terminalVelocity = -20.0f * player->mJumpParams.mGravity.get();
  else
    terminalVelocity = -75.0f * player->mJumpParams.mGravity.get();
  player->mSpeed.y = Max(player->mSpeed.y, terminalVelocity);
}

// extern -> SME.cpp
// 0x8025B8C0
f32 Patch::Mario::checkGroundSpeedLimit() {
  TMario *player;
  SME_FROM_GPR(31, player);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  f32 multiplier = 1.0f;
  if (onYoshi__6TMarioCFv(player)) {
    multiplier *= player->mYoshiParams.mRunYoshiMult.get();
  } else {
    multiplier *= playerData->getParams()->mSpeedMultiplier.get();
  }
  return multiplier;
}

// extern -> SME.cpp
// 0x8024CC6C
void Patch::Mario::checkJumpSpeedLimit(f32 speed) {
  TMario *player;
  SME_FROM_GPR(31, player);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  f32 speedCap = 32.0f;
  f32 speedReducer = 0.2f;

  if (!onYoshi__6TMarioCFv(player)) {
    speedCap *= playerData->getParams()->mSpeedMultiplier.get();
    speedReducer *= Util::Math::scaleLinearAtAnchor<f32>(
        playerData->getParams()->mSpeedMultiplier.get(), 3.0f, 1.0f);
  }

  if (speed > speedCap) {
    player->mForwardSpeed = (speed - speedReducer);
  }
}

// extern -> SME.cpp
// 0x8024CC2C
TMario *Patch::Mario::checkJumpSpeedMulti(TMario *player, f32 factor, f32 max) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (playerData->isMario() && !onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        ((factor * playerData->getParams()->mSpeedMultiplier.get()) * max) +
        player->mForwardSpeed;
    return player;
  } else {
    player->mForwardSpeed = (factor * max) + player->mForwardSpeed;
    return player;
  }
}

static f32 checkSlideSpeedMulti() {
  TMario *player;
  SME_FROM_GPR(30, player);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  constexpr f32 speedCap = 100.0f;
  constexpr f32 rocketMultiplier = 1.8f;
  constexpr f32 hoverMultiplier = 1.2f;
  constexpr f32 brakeRate = 0.005f;

  if (player->mFludd && isEmitting__9TWaterGunFv(player->mFludd)) {
    if (player->mFludd->mCurrentNozzle == TWaterGun::Hover ||
        player->mFludd->mCurrentNozzle == TWaterGun::Underwater)
      playerData->mSlideSpeedMultiplier = hoverMultiplier;
    else if (player->mFludd->mCurrentNozzle == TWaterGun::Rocket) {
      const f32 multiplier =
          (rocketMultiplier *
           ((speedCap * rocketMultiplier) / player->mForwardSpeed));
      playerData->mSlideSpeedMultiplier = rocketMultiplier;
      player->mPrevSpeed.scale(multiplier);
    } else {
      playerData->mSlideSpeedMultiplier =
          Max(playerData->mSlideSpeedMultiplier - brakeRate, 1.0f);
    }
  } else {
    playerData->mSlideSpeedMultiplier =
        Max(playerData->mSlideSpeedMultiplier - brakeRate, 1.0f);
  }

  if (!onYoshi__6TMarioCFv(player)) {
    return speedCap * playerData->mSlideSpeedMultiplier;
  } else {
    return speedCap;
  }
}
SME_WRITE_32(SME_PORT_REGION(0x8025C3D8, 0, 0, 0), 0x40810028);
SME_WRITE_32(SME_PORT_REGION(0x8025C3FC, 0, 0, 0), 0xFC800018);
SME_WRITE_32(SME_PORT_REGION(0x8025C400, 0, 0, 0), 0xD09E00B0);
SME_PATCH_BL(SME_PORT_REGION(0x8025C404, 0, 0, 0), checkSlideSpeedMulti);
SME_WRITE_32(SME_PORT_REGION(0x8025C408, 0, 0, 0), 0xFC400890);
SME_WRITE_32(SME_PORT_REGION(0x8025C410, 0, 0, 0), 0x60000000);

#if 0
static void checkHoverSpeedMulti(f32 factor, f32 max) {
  TMario *player;
  __asm { mr player, r30}
  ;

  if (playerData->isMario() && !onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        ((factor * playerData->mParams->Attributes.mSpeedMultiplier) * max) +
        player->mForwardSpeed;
  } else {
    player->mForwardSpeed = (factor * max) + player->mForwardSpeed;
  }
}
kmCall(0x8024AE80, &checkHoverSpeedMulti);
kmWrite32(0x8024AE84, 0x60000000);
#endif

// 8024afe0 <- hover air Y spd

static SME_PURE_ASM void scaleFluddInitYSpd() {
  asm volatile("lfs 0, " SME_STRINGIZE(SME_PORT_REGION(
      -0xFE0, 0, 0, 0)) "(2)  \n\t"
                        "lfs 4, 0x28(30)                          \n\t"
                        "fmuls 0, 0, 4                            \n\t"
                        "blr                                      \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80254A2C, 0, 0, 0), scaleFluddInitYSpd);

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
      -0xED4, 0, 0, 0)) "(2)\n\t"
                        "lfs 4, 0x28(29)                     \n\t"
                        "fmuls 0, 0, 4                       \n\t"
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

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  if (playerData->isMario())
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

static void dynamicFallDamage(TMario *player, int dmg, int type, int emitcount,
                              int tremble) {
#define floorDamageExec__6TMarioFiiii                                          \
  ((void (*)(TMario *, int, int, int, int))SME_PORT_REGION(0x8024303C, 0, 0, 0))

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  dmg *= static_cast<int>((player->mLastGroundedPos.y - player->mPosition.y) /
                          (player->mDeParams.mDamageFallHeight.get() / 1.4f));
  if (dmg > 2) {
    type = 0; // shaky
    emitcount = (dmg - 2) * 3;
  }

  if (player->mSpeed.y <= (-75.0f * player->mJumpParams.mGravity.get()) + 5.0f)
    floorDamageExec__6TMarioFiiii(player, dmg, type, emitcount, tremble);

#undef floorDamageExec__6TMarioFiiii
}
SME_PATCH_BL(SME_PORT_REGION(0x8024C73C, 0, 0, 0), dynamicFallDamage);

static u32 preventDebuggingDeath(TMario *player) {
  extern bool gInXYZMode;
  return gInXYZMode ? 0x224E0 : player->mState; // Spoof non dying value
};
SME_PATCH_BL(SME_PORT_REGION(0x80243110, 0, 0, 0), preventDebuggingDeath);

// ------------ //
// Shadow Mario //
// ------------ //

static JUtility::TColor getEMarioHealthBarRGBA(TEnemyMario *eMario) {
  JUtility::TColor color;
  const s16 maxHealth = ((s16 *)eMario->mEnemyManager)[0x40 / 2];

  color.set(0xFF, 0x00, 0x00, 0xFF);
  color.g = Util::Math::lerp<u8>(0, 255,
                                 static_cast<float>(eMario->mEnemyHealth) /
                                     static_cast<float>(maxHealth));
  color.r -= color.g;

  return color;
}

// extern -> SME.cpp
// 0x8003FDAC
void Patch::Mario::manageEMarioHealthWrapper(TEnemyMario *eMario, Mtx *posMtx) {
  *(JUtility::TColor *)0x8040FA90 = getEMarioHealthBarRGBA(eMario);
  drawHPMeter__11TEnemyMarioFPA4_f(eMario, posMtx);
}