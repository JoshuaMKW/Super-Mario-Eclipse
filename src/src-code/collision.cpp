#include "sms/JSystem/JGeometry.hxx"
#include "sms/actor/Mario.hxx"

#include "libs/sGeometry.hxx"
#include "SME.hxx"

// 0x80258334, 0x80264CFC
// extern -> SME.cpp
void checkIsGlideBounce(TMario *player)
{
    if ((player->mFloorTriangle->mCollisionType & 0x7FFF) == 16007 || (player->mFloorTriangle->mCollisionType & 0x7FFF) == 17007)
    {
        TBGCheckData *_oldCol = player->mFloorTriangle;
        u16 _oldType = _oldCol->mCollisionType;
        _oldCol->mCollisionType = 7;

        checkEnforceJump__6TMarioFv(player);
        _oldCol->mCollisionType = _oldType;

        player->mCustomInfo->mTerminalVelocity = -20.0f * player->mGravity;
        player->mCustomInfo->CollisionFlags.mIsSpinBounce = true;
        changePlayerStatus__6TMarioFUlUlb(player, TMario::State::JUMPSPIN1, 0, 0);
    }
    else
        checkEnforceJump__6TMarioFv(player);
}

// 0x8024C558
// extern -> SME.cpp
u16 checkIsRestoreTypeNoFallDamage(TBGCheckData *floor)
{
    if ((floor->mCollisionType & 0x7FFF) == 16010 ||
        (floor->mCollisionType & 0x7FFF) == 17010)
        return 10;
    else
        return floor->mCollisionType;
}

static void checkRestoreHealth(TMario *player)
{
    if (player->mCustomInfo->mCollisionTimer <= 0)
    {
        incHP__6TMarioFi(player, 1);
        player->mCustomInfo->mCollisionTimer = player->mFloorTriangle->mValue4;
    }
    else
        player->mCustomInfo->mCollisionTimer -= 1;
}


static void checkIsCannonType(TMario *player)
{
    if (!player->mController->mButtons.mInput & TMarioGamePad::Buttons::DPAD_UP ||
        !player->mCustomInfo->CollisionFlags.mIsFaceUsed)
        return;

    if ((player->mFloorTriangle->mCollisionType & 0x7FFF) == 16080 ||
        (player->mFloorTriangle->mCollisionType & 0x7FFF) == 17080)
    {
        changePlayerStatus__6TMarioFUlUlb(player, TMario::State::TRIPLE_J, 0, 0);
        emitParticle__6TMarioFis(player, TMario::Effect::GROUND_SHARP_SHOCK);
        emitParticle__6TMarioFis(player, TMario::Effect::GROUND_SMOKE_PLUME);
        player->mForwardSpeed = (u8)(player->mFloorTriangle->mValue4 >> 8);
        player->mPosition.y += 1.0f;
        player->mSpeed.y = (u8)player->mFloorTriangle->mValue4;
        player->mCustomInfo->CollisionFlags.mIsDisableInput = true;
        player->mController->State.mReadInput = false;
        player->mCustomInfo->CollisionFlags.mIsFaceUsed = true;
    }
}

static void changeNozzleType(TMario *player, u16 type)
{
    if (player->mCustomInfo->CollisionFlags.mIsFaceUsed || !isMario__6TMarioFv(player))
        return;

    player->mAttributes.mHasFludd = player->mFloorTriangle->mValue4 == 1;

    TWaterGun::NozzleType nozzle = TWaterGun::Hover;
    if (type >= 17090)
        nozzle = (TWaterGun::NozzleType)(type - 17090);
    else
        nozzle = (TWaterGun::NozzleType)(type - 16090);

    changeNozzle__9TWaterGunFQ29TWaterGun11TNozzleTypeb(player->mFludd, nozzle, 1);
    player->mCustomInfo->CollisionFlags.mIsFaceUsed = true;
}

static void boostPadCol(TMario *player)
{
    player->mForwardSpeed = player->mFloorTriangle->mValue4;
    if (player->mState == TMario::State::IDLE || !player->mCustomInfo->CollisionFlags.mIsFaceUsed)
    {
        changePlayerStatus__6TMarioFUlUlb(player, TMario::State::RUNNING, 0, 0);
        startVoice__6TMarioFUl(player, TMario::Voice::JUMP);
    }
}

static void setGravityCol(TMario *player)
{
    player->mGravity = (f32)(player->mFloorTriangle->mValue4) / 100.0f;
}

static void antiGravityCol(TMario *player)
{
    if ((player->mPosition.y - player->mFloorTriangle->mMaxHeight) > player->mFloorTriangle->mValue4 &&
        player->mFloorTriangle->mValue4 != 0)
        return;

    player->mSpeed.y = 10.0f;
    if ((player->mState & TMario::State::AIRBORN) == false)
    {
        player->mPosition.y += 1.0f;
        changePlayerStatus__6TMarioFUlUlb(player, TMario::State::FALL, 0, 0);
    }
}

static void warpToLinkedCol(TMario *player)
{
    TBGCheckData *linkedCol = gInfo.mWarpColArray->resolveCollisionWarp(player->mFloorTriangle);
    TVectorTriangle colTriangle(&linkedCol->mVertexA, &linkedCol->mVertexB, &linkedCol->mVertexC);

    if (!linkedCol)
    {
        player->mCustomInfo->mCollisionTimer = 0;
        return;
    }

    f32 speed = player->mSpeed.resultant();

    if (speed > 1.0f)
    {
        player->mCustomInfo->mCollisionTimer = 0;
        return;
    }

    if (!player->mCustomInfo->CollisionFlags.mIsFaceUsed)
    {
        if (player->mCustomInfo->mCollisionTimer > 140)
        {
            player->mPosition = colTriangle.center();
            player->mFloorTriangle = linkedCol;
            player->mFloorTriangleCopy = linkedCol;
            player->mFloorBelow = player->mPosition.y;
            player->mCustomInfo->CollisionFlags.mIsFaceUsed = true;
            player->mCustomInfo->mCollisionTimer = 0;
            startSoundActor__6TMarioFUl(player, TMario::Voice::JUMP);

            gpCamera->mPosition.x = lerp<f32>(gpCamera->mPosition.x, player->mPosition.x, 0.9375f);
            gpCamera->mPosition.y = player->mPosition.y + 300.0f;
            gpCamera->mPosition.z = lerp<f32>(gpCamera->mPosition.z, player->mPosition.z, 0.9375f);

            gpCamera->mHorizontalAngle = (u16)TVectorTriangle::bearingAngleY(player->mPosition, gpCamera->mPosition) * 182;
        }
        else if (player->mCustomInfo->mCollisionTimer > 80)
        {
            player->mCustomInfo->CollisionFlags.mIsDisableInput = true;
            player->mController->State.mReadInput = false;
            emitGetEffect__6TMarioFv(player);
        }
    }
    else
    {
        if (player->mCustomInfo->mCollisionTimer > 60)
        {
            player->mCustomInfo->CollisionFlags.mIsDisableInput = false;
            player->mController->State.mReadInput = false;
        }
    }
    player->mCustomInfo->mCollisionTimer += 1;
}

static void warpToLinkedColPreserve(TMario *player, bool fluid)
{
    TBGCheckData *linkedCol = gInfo.mWarpColArray->resolveCollisionWarp(player->mFloorTriangle);
    TVectorTriangle colTriangle(&linkedCol->mVertexA, &linkedCol->mVertexB, &linkedCol->mVertexC);

    if (!linkedCol) return;

    if (!player->mCustomInfo->CollisionFlags.mIsFaceUsed)
    {
        player->mFloorTriangle = linkedCol;
        player->mFloorTriangleCopy = linkedCol;
        player->mCustomInfo->CollisionFlags.mIsFaceUsed = true;

        player->mPosition = colTriangle.center();

        gpCamera->mPosition.x = lerp<f32>(gpCamera->mPosition.x, player->mPosition.x, 0.9375f);
        gpCamera->mPosition.y = player->mPosition.y + 300.0f;
        gpCamera->mPosition.z = lerp<f32>(gpCamera->mPosition.z, player->mPosition.z, 0.9375f);

        gpCamera->mHorizontalAngle =
            (u16)TVectorTriangle::bearingAngleY(player->mPosition, gpCamera->mPosition) * 182;

        JGeometry::TVec3<f32> colNormal = colTriangle.normal();
        JGeometry::TVec3<f32> colUnit = colNormal.unitVector();

        f32 magnitude = fabsf__Ff(player->mSpeed.x) +
                        fabsf__Ff(player->mSpeed.y) +
                        fabsf__Ff(player->mSpeed.z);

        player->mAngle.y = (u16)colNormal.getNormalAngle() * 182;
        setPlayerVelocity__6TMarioFf(player, magnitude * colUnit.x + magnitude * colUnit.z);
        player->mSpeed.y = magnitude * colUnit.y;

        player->mPosition.x += 40.0f * colUnit.x;
        player->mPosition.y += 40.0f * colUnit.y;
        player->mPosition.z += 40.0f * colUnit.z;

        changePlayerStatus__6TMarioFUlUlb(player, TMario::State::FALL, 0, 0);
    }
    else
        player->mCustomInfo->CollisionFlags.mIsFaceUsed =
            (!player->mController->mButtons.mFrameInput & TMarioGamePad::Buttons::DPAD_DOWN && !fluid);
}

static inline void resetValuesOnStateChange(TMario *player)
{
    switch (player->mPrevState)
    {
    case (TMario::State::JUMPSPIN1):
        player->mCustomInfo->mTerminalVelocity = -75.0f * player->mGravity;
        player->mCustomInfo->CollisionFlags.mIsSpinBounce = false;
    case (TMario::State::TRIPLE_J):
        player->mCustomInfo->CollisionFlags.mIsDisableInput = false;
        player->mController->State.mReadInput = true;
    }
    if (player->mCustomInfo->CollisionFlags.mIsDisableInput)
        //Patches pausing/map escaping the controller lock
        player->mController->State.mReadInput = false;
}

static inline void resetValuesOnGroundContact(TMario *player)
{
    if (!(player->mState & TMario::State::AIRBORN))
    {
        player->mCustomInfo->mTerminalVelocity = -75.0f * player->mGravity;
        player->mCustomInfo->CollisionFlags.mIsAirborn = false;
        player->mCustomInfo->CollisionFlags.mIsDisableInput = false;
    }
}

static inline void resetValuesOnAirborn(TMario *player)
{
    if (!(player->mState & TMario::State::AIRBORN))
        return;
}

static inline void resetValuesOnCollisionChange(TMario *player)
{
    if (player->mFloorTriangle->mCollisionType != player->mCustomInfo->mPrevCollision)
    {
        player->mCustomInfo->mPrevCollision = player->mFloorTriangle->mCollisionType;
        player->mCustomInfo->CollisionFlags.mIsFaceUsed = false;
        player->mCustomInfo->mCollisionTimer = 0;
    }
}

// 0x802500B8
// extern -> SME.cpp
u32 updateCollisionContext(TMario *player)
{
    resetValuesOnStateChange(player);
    resetValuesOnGroundContact(player);
    resetValuesOnAirborn(player);
    resetValuesOnCollisionChange(player);

    f32 marioCollisionHeight = *(f32 *)0x80415CC4;
    if (player->mCustomInfo->mParams)
        marioCollisionHeight *= player->mCustomInfo->mParams->Attributes.mSizeMultiplier;

    if (player->mCeilingAbove - player->mFloorBelow < marioCollisionHeight &&
        player->mRoofTriangle &&
        !(player->mState & TMario::State::AIRBORN))
    {
        loserExec__6TMarioFv(player);
    }
    return 1;
}

// 0x80250CA0
// extern -> SME.cpp
u16 masterGroundCollisionHandler(TBGCheckData *colTriangle)
{
    TMario *player;
    SME_FROM_GPR(r29, player);

    u16 type = colTriangle->mCollisionType & 0x7FFF;
    switch (type)
    {
    case 16007:
    case 17007:
        checkRestoreHealth(player);
        break;
    case 16010:
    case 17010:
        checkRestoreHealth(player);
        break;
    case 16020:
    case 17020:
        boostPadCol(player);
        break;
    case 16021:
    case 17021:
        setGravityCol(player);
        break;
    case 16040:
    case 17040:
        warpToLinkedCol(player);
        break;
    case 16041:
    case 16042:
        warpToLinkedColPreserve(player, false);
        break;
    case 17041:
    case 17042:
        warpToLinkedColPreserve(player, true);
        break;
    case 16080:
    case 17080:
        checkIsCannonType(player);
        break;
    case 16090:
    case 16091:
    case 16092:
    case 16093:
    case 16094:
    case 16095:
    case 17090:
    case 17091:
    case 17092:
    case 17093:
    case 17094:
    case 17095:
        changeNozzleType(player, type);
        break;
    }
    return type;
}

// 0x8025059C
// extern -> SME.cpp
u32 masterAllCollisionHandler(TMario *player)
{
    u16 type = player->mFloorTriangle->mCollisionType & 0x7FFF;
    switch (type)
    {
    case 16022:
    case 17022:
        setGravityCol(player);
        break;
    case 16023:
    case 17023:
        antiGravityCol(player);
        break;
    case 16190:
    case 16191:
    case 16192:
    case 16193:
    case 16194:
    case 16195:
    case 17190:
    case 17191:
    case 17192:
    case 17193:
    case 17194:
    case 17195:
        changeNozzleType(player, type);
        break;
    }
    return player->mState;
}