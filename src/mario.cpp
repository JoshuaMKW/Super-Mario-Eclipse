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

u32 SME::Patch::Mario::carryOrTalkNPC(TBaseNPC *npc) {
  const SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(gpMarioAddress);

  if (!playerParams)
    return isNowCanTaken__8TBaseNPCCFv();

  if ((*(u32 *)(&npc->mStateFlags) & 0x840007) != 0)
    return 0;

  if (gpMarioAddress->mState == static_cast<u32>(TMario::State::IDLE))
    return 0;

  if (!playerParams->isMario())
    return 0;

  bool oldTake = npc->mStateFlags.mCanBeTaken;
  npc->mStateFlags.mCanBeTaken = playerParams->getParams()->mCanHoldNPCs.get();

  u32 ret = isNowCanTaken__8TBaseNPCCFv(npc);

  npc->mStateFlags.mCanBeTaken = oldTake;
  return ret;
}

bool SME::Patch::Mario::canGrabAtNPC() {
  TBaseNPC *npc;
  SME_FROM_GPR(30, npc);

  const SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(gpMarioAddress);

  if (!playerParams->isMario())
    return npc->mStateFlags.mCanBeTaken;

  if (npc->mStateFlags.mCanBeTaken)
    return true;

  if (gpMarioAddress->mState == static_cast<u32>(TMario::State::IDLE))
    return false;

  return playerParams->getParams()->mCanHoldNPCs.get();
}

bool SME::Patch::Mario::canCarryNPC() {
  TBaseNPC *npc;
  SME_FROM_GPR(29, npc);

  const SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(gpMarioAddress);

  if (!playerParams)
    return npc->mStateFlags.mCanBeTaken;

  if (npc->mStateFlags.mCanBeTaken)
    return true;

  if (gpMarioAddress->mState == static_cast<u32>(TMario::State::IDLE))
    return false;

  if (!playerParams->isMario())
    return false;

  return playerParams->getParams()->mCanHoldNPCs.get();
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

#ifdef MARIO_MODS
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
TMapObjBase *SME::Patch::Mario::getTreeClimbMaxFall(TMapObjBase *tree,
                                                    f32 speed) {
  TMario *player;
  f32 ret;
  SME_FROM_GPR(31, player);
  SME_FROM_FPR(3, ret);

  Vec size;
  player->JSGGetScaling(&size);

  ret = tree->mReceiveHeight / ((size.y * 0.2f) + (1.0f - 0.2f));

  SME_TO_FPR(1, speed);
  SME_TO_FPR(3, ret);
  return tree;
}

// extern -> SME.cpp
// 0x80261CF4
bool SME::Patch::Mario::scaleTreeSlideSpeed(f32 _f1, f32 _f2) {
  TMario *player;
  SME_FROM_GPR(31, player);

  _f1 = 0.00195313f * player->mJumpParams.mGravity.get();

  SME_TO_FPR(1, _f1);
  SME_TO_FPR(2, _f2);

  if (_f2 < -16.0f) {
    return true;
  } else {
    player->mSpeed.y = 0.0f;
    return false;
  }
}
#endif

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

#if 0
/* ROOF HANG CODE */

static void scaleRoofClimbHeight(f32 yCoord, f32 speed) {
  TMario *player;
  __asm { mr player, r31}
  ;

  player->mPosition.y = yCoord - (160.0f * player->mSize.y);

  __asm volatile {fmr f2, speed};
}
kmCall(0x8025D66C, &scaleRoofClimbHeight);
kmWrite32(0x8025D670, 0x60000000);
kmWrite32(0x8025D674, 0x60000000);

static asm void scaleRoofSquashedHeight(){
    nofralloc lfs f3, -0xDE0(rtoc)lfs f5, 0x28(r30)fmuls f3, f5,
    f3 blr} kmCall(0x802617EC, &scaleRoofSquashedHeight);

static void scaleRoofMoveDiff() {
  TMario *player;
  f32 _f0;

  __asm { mr player, r30
            fmr _f0, f0}
  ;

  _f0 = 30.0f * player->mSize.y;

  __asm volatile {fmr f0, _f0};
}
kmCall(0x80261824, &scaleRoofMoveDiff);
#endif

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

#if 0
static TBGCheckData *canHangOnRoof(TBGCheckData *roof) {
  bool canCling;
  __asm { mr canCling, r4}
  ;

  TMario *player;
  __asm { mr player, r30}
  ;

  if (playerParams->isMario())
    canCling = (roof->mCollisionType == 266 ||
                playerParams->mParams->Attributes.mCanClimbWalls);
  else
    canCling = roof->mCollisionType == 266;

  __asm volatile {mr r4, canCling};

  return roof;
}
kmCall(0x802617C0, &canHangOnRoof);
kmWrite32(0x802617C4, 0x2C040000);
kmWrite32(0x802617C8, 0x4182000C);

/* WALL CLIMB CODE */

// 8025e560 <- possibly resize float by char size mul for height
// 8025e5bc <- gate sound?

// 80415DCC <- this float controls how far into the wall mario is placed on
// first grab 80415DD4 <- this float controls how high mario is placed from the
// floor on first grab

// 80415DEC <- this float controls the climbing speed, scale accordingly

static f32 scaleClimbSpeed(f32 speed) {
  TMario *player;
  __asm { mr player, r30}
  ;

  f32 _f0;
  f32 _f3;
  f32 _f7;

  __asm { fmr _f0, f0
            fmr _f3, f3
            fmr _f7, f7}
  ;

  f32 scale = 0.015625f;

  if (playerParams->isMario())
    scale *= playerParams->mParams->Attributes.mSpeedMultiplier;

  __asm volatile {fmr f0, _f0 fmr f3, _f3 fmr f7, _f7};

  return scale;
}
kmCall(0x8025E19C, &scaleClimbSpeed);
kmWrite32(0x8025E1A0, 0x807E0010);
kmWrite32(0x8025E1A4, 0x801E0014);
kmWrite32(0x8025E1C4, 0xC0440014);
kmCall(0x8025E218, &scaleClimbSpeed);
kmWrite32(0x8025E21C, 0x7FC3F378);

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

static TBGCheckData *canJumpClingWall(TBGCheckData *wall) {
  bool canCling;
  __asm { mr canCling, r4}
  ;

  TMario *player;
  __asm { mr player, r28}
  ;

  if (playerParams->isMario())
    canCling =
        wall->mCollisionType == 266 ||
        (playerParams->mParams->Attributes.mCanClimbWalls &&
         player->mController->mButtons.mInput & TMarioGamePad::Buttons::Z);
  else
    canCling = wall->mCollisionType == 266;

  __asm volatile {mr r4, canCling};

  return wall;
}
kmCall(0x8024C888, &canJumpClingWall);
kmWrite32(0x8024C88C, 0x2C040000);
kmWrite32(0x8024C890, 0x4182000C);

static TBGCheckData *canUnkActionWall(TBGCheckData *wall) {
  bool canCling;
  __asm { mr canCling, r4}
  ;

  TMario *player;
  __asm { mr player, r22}
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
kmCall(0x80256A3C, &canUnkActionWall);
kmWrite32(0x80256A40, 0x2C040000);
kmWrite32(0x80256A44, 0x4182000C);

static TBGCheckData *canRunClingWall(TBGCheckData *wall) {
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
kmCall(0x8025B200, &canRunClingWall);
kmWrite32(0x8025B204, 0x2C040000);
kmWrite32(0x8025B208, 0x4182000C);

static TBGCheckData *canMoveOnWall1() {
  bool canCling;
  __asm { mr canCling, r4}
  ;

  TMario *player;
  __asm { mr player, r30}
  ;

  TBGCheckData *wall;
  __asm { mr wall, r29}
  ;

  if (playerParams->isMario())
    canCling = (wall->mCollisionType == 266 ||
                playerParams->mParams->Attributes.mCanClimbWalls);
  else
    canCling = wall->mCollisionType == 266;

  __asm volatile {mr r4, canCling};

  return wall;
}
kmCall(0x8025E2F4, &canMoveOnWall1);
kmWrite32(0x8025E2F8, 0x2C040000);
kmWrite32(0x8025E2FC, 0x4182000C);

static TBGCheckData *canMoveOnWall2(TBGCheckData *wall) {
  bool canCling;
  __asm { mr canCling, r4}
  ;

  TMario *player;
  __asm { mr player, r30}
  ;

  if (playerParams->isMario())
    canCling = (wall->mCollisionType == 266 ||
                playerParams->mParams->Attributes.mCanClimbWalls);
  else
    canCling = wall->mCollisionType == 266;

  __asm volatile {mr r4, canCling};

  return wall;
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
           player->mState != static_cast<u32>(TMario::State::WALLSLIDE)) {
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

#if 0
static f32 checkGroundSpeedMulti() {
  TMario *player;
  SME_FROM_GPR(31, player);

  SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerParams(player);

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player))
    return player->mRunSlowdownFactor2 /
           playerParams->mParams->Attributes.mSpeedMultiplier;
  else
    return player->mRunSlowdownFactor2;
}
kmCall(0x8025B8F8, &checkGroundSpeedMulti);

// 0x80272FF0 - fSpeed Multplier - swim
static void checkSwimSpeedMulti(f32 max, f32 factor) {
  TMario *player;
  __asm { mr player, r31}
  ;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        ((factor * playerParams->mParams->Attributes.mSpeedMultiplier) * max) +
        player->mForwardSpeed;
  } else {
    player->mForwardSpeed = (factor * max) + player->mForwardSpeed;
  }
}
kmCall(0x80272FF0, &checkSwimSpeedMulti);
kmWrite32(0x80272FF4, 0x60000000);

static f64 checkSlideSpeedMulti(f32 max, f32 factor) {
  TMario *player;
  __asm { mr player, r30}
  ;

  f64 slowFactor = 0.5;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player)) {
    slowFactor /= playerParams->mParams->Attributes.mSpeedMultiplier;
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
kmCall(0x8025C3E0, &checkSlideSpeedMulti);
kmWrite32(0x8025C3E8, 0xFC4100F2);
kmWrite32(0x8025C3EC, 0xFC2300F2);

static f32 checkGroundSpeedCap() {
  TMario *player;
  __asm { mr player, r31}
  ;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player))
    return player->mMaxGroundSpeed *
           playerParams->mParams->Attributes.mSpeedMultiplier;
  else
    return player->mMaxGroundSpeed;
}
kmCall(0x8025BC44, &checkGroundSpeedCap);

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

static TMario *checkOilSlipSpeedMulti(f32 factor, f32 max) {
  TMario *player;
  __asm { mr player, r31}
  ;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        ((factor * playerParams->mParams->Attributes.mSpeedMultiplier) * max) +
        player->mForwardSpeed;
  } else {
    player->mForwardSpeed = (factor * max) + player->mForwardSpeed;
  }
  return player;
}
kmCall(0x80259368, &checkOilSlipSpeedMulti);
kmWrite32(0x8025936C, 0x60000000);

static f32 checkJumpingWallBonk(f32 f1, f32 speed) {
  TMario *player;
  __asm { mr player, r28}
  ;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player))
    return player->mWallBonkThreshold *
           playerParams->mParams->Attributes.mSpeedMultiplier;
  else
    return player->mWallBonkThreshold;
}
kmWrite32(0x8024C7C0, 0xC05C00B0);
kmCall(0x8024C7C4, &checkJumpingWallBonk);
kmWrite32(0x8024C7C8, 0xFC020840);

static f32 checkRunningWallBonk(f32 f1, f32 speed) {
  TMario *player;
  __asm { mr player, r31}
  ;

  if (playerParams->isMario() && !onYoshi__6TMarioCFv(player))
    return player->mWallBonkThreshold *
           playerParams->mParams->Attributes.mSpeedMultiplier;
  else
    return player->mWallBonkThreshold;
}
kmWrite32(0x8025B148, 0xC05F00B0);
kmCall(0x8025B14C, &checkRunningWallBonk);
kmWrite32(0x8025B150, 0xFC020840);

static void scaleDiveSpeed() {
  TMario *player;
  __asm { mr player, r30}
  ;

  f32 _f0;
  __asm { fmr _f0, f0}
  ;

  if (playerParams->isMario())
    _f0 = 48.0f * playerParams->mParams->Attributes.mSpeedMultiplier;
  else
    _f0 = 48.0f;

  __asm volatile {fmr f0, _f0};
}
kmCall(0x80254904, &scaleDiveSpeed);
kmWrite32(0x80254908, 0xC042F0DC);
kmWrite32(0x8025490C, 0xC03E00B0);

// 8024afe0 <- hover air Y spd

static asm void scaleHoverInitYSpd(){
    nofralloc lfs f0, -0x0FE0(rtoc)lfs f4, 0x28(r30)fmuls f0, f0,
    f4 blr} kmCall(0x80254A2C, &scaleHoverInitYSpd);

static f32 setBounceYSpeed() {
  TMario *player;
  __asm { mr player, r30}
  ;

  return 130.0f * player->mSize.y;
}
kmCall(0x80254720, &setBounceYSpeed);
kmWrite32(0x80254724, 0xD01E00A8);

static asm void checkGrabHeight(){
  nofralloc lfs f0,
  -0xEBC(rtoc)lfs f4,
  0x28(r29)fcmpo cr0,
  f4,
  f0 lfs f0,
  -0x0ED4(rtoc)bgt skipmul

      fmuls f0,
  f0,
  f4

  skipmul : blr
} kmCall(0x80256D34, &checkGrabHeight);

static asm void setCollisionHeight1(){
    nofralloc lfs f1, -0xEDC(rtoc)lfs f0, 0x28(r22)fmuls f1, f0,
    f1 blr} kmCall(0x8025696C, &setCollisionHeight1);

static asm void setCollisionHeight2(){
    nofralloc lfs f2, -0xEDC(rtoc)lfs f0, 0x28(r29)fmuls f2, f0,
    f2 blr} kmCall(0x80256D14, &setCollisionHeight2);

static asm void setCollisionHeight3(){
    nofralloc lfs f0, -0xEDC(rtoc)lfs f2, 0x28(r30)fmuls f0, f2,
    f0 blr} kmCall(0x802573FC, &setCollisionHeight3);

static void setCollisionWidth() {
  TMario *player;
  __asm { mr player, r29}
  ;

  f32 width;
  __asm { fmr width, f0}
  ;

  if (playerParams->isMario()) {
    player->mCollisionXZSize =
        playerParams->DefaultAttrs.mCollisionWidth * player->mSize.x;
  } else {
    player->mCollisionXZSize = width;
  }
}
kmCall(0x802505F4, &setCollisionWidth);

static f32 manageGrabLength() {
  TMario *player;
  __asm { mr player, r29}
  ;

  return 60 * player->mSize.z;
}
kmCall(0x80256CE8, &manageGrabLength);
kmWrite32(0x80256CFC, 0xEC01283C);
kmWrite32(0x80256D04, 0xC05E003C);
kmWrite32(0x80256D0C, 0xEC0100BC);
#endif

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