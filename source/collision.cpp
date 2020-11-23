#include "includes/eclipse.hxx"

//Twirl Bounce Collision

//0x802523D4
/*
lhz r0, 0 (r3)
cmplwi r0, 16007
beq bounce
cmplwi r0, 17007
bne pass
bounce:
li r0, 7
pass:
*/

//0x80252430
/*
lwz r6, 0xE0 (r31)
lhz r6, 0 (r6)
cmplwi r6, 16007
beq bounce
cmplwi r6, 17007
bne pass
bounce:
ori r0, r0, 0x800
pass:
rlwinm. r0, r0, 0, 20, 20
*/

//0x80254750
/*
.set .LCPI3_0, 0x800034F0

        stwu 1, -16(1)
        lbz 4, 328(30)
        andi. 4, 4, 32
        beq     0, .LBB3_2
        lwz 4, 224(30)
        lis 5, 17200
        lha 4, 2(4)
        stw 5, 8(1)
        xoris 4, 4, 32768
        stw 4, 12(1)
        lis 4, .LCPI3_0@ha
        lfd 0, 8(1)
        lfs 1, .LCPI3_0@l(4)
        fsub 0, 0, 1
        frsp 0, 0
.LBB3_2:
        stfs 0, 168(30)
        li 4, 30902
        addi 1, 1, 16
*/

//0x8025246C
void checkIsGlideBounce(TMario *gpMario)
{
    if ((gpMario->mFloorTriangle->mCollisionType & 0x7FFF) == 16007 || (gpMario->mFloorTriangle->mCollisionType & 0x7FFF) == 17007)
    {
        TBGCheckData *spoofCol = (TBGCheckData *)__nw__FUli(sizeof(TBGCheckData), 32);
        memcpy(spoofCol, gpMario->mFloorTriangle, sizeof(TBGCheckData));
        spoofCol->mCollisionType = 7;
        checkEnforceJump__6TMarioFv(gpMario, spoofCol);

        gpMario->mCustomInfo->mTerminalVelocity = -20.0f * gpMario->mGravity;
        gpMario->mCustomInfo->CollisionFlags.mIsSpinBounce = true;
        changePlayerStatus__6TMarioFUlUlb(gpMario, TMario::SA_JUMPSPIN1, 0, 0);
    }
}

//0x80004A94
void checkRestoreHealth(TMario *gpMario)
{
    if (gpMario->mCustomInfo->mCollisionTimer <= 0)
    {
        incHP__6TMarioFi(gpMario, 1);
        gpMario->mCustomInfo->mCollisionTimer = gpMario->mFloorTriangle->mValue4;
    }
    else
    {
        gpMario->mCustomInfo->mCollisionTimer -= 1;
    }
}

//0x8024C558
u16 checkIsRestoreTypeNoFallDamage(TBGCheckData *floor)
{
    if ((floor->mCollisionType & 0x7FFF) == 16010 || (floor->mCollisionType & 0x7FFF) == 17010)
    {
        return 10;
    }
    else
    {
        return floor->mCollisionType;
    }
}

//0x80004A98
void checkIsCannonType(TMario *gpMario)
{
    if (!gpMario->mController->isPressed(TMarioGamePad::DPAD_UP) ||
        !gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed)
        return;

    if ((gpMario->mFloorTriangle->mCollisionType & 0x7FFF) == 16080 || (gpMario->mFloorTriangle->mCollisionType & 0x7FFF) == 17080)
    {
        changePlayerStatus__6TMarioFUlUlb(gpMario, TMario::SA_TRIPLE_J, 0, 0);
        emitParticle__6TMarioFis(gpMario, TMario::E_GROUND_SHARP_SHOCK);
        emitParticle__6TMarioFis(gpMario, TMario::E_GROUND_SMOKE_PLUME);
        gpMario->mForwardSpeed = (u8)(gpMario->mFloorTriangle->mValue4 >> 8);
        gpMario->mCoordinates.y += 1.0f;
        gpMario->mSpeed.y = (u8)gpMario->mFloorTriangle->mValue4;
        gpMario->mCustomInfo->CollisionFlags.mIsDisableInput = true;
        gpMario->mController->setEnabled(false);
        gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed = true;
    }
}

//0x80004A9C
void changeNozzleType(TMario *gpMario, u16 type)
{
    if (gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed || !isMario__6TMarioFv(gpMario))
    {
        return;
    }

    gpMario->mAttributes.mHasFludd = gpMario->mFloorTriangle->mValue4 == 1;

    TWaterGun::NOZZLETYPE nozzle = TWaterGun::Hover;
    if (type >= 17090)
        nozzle = (TWaterGun::NOZZLETYPE)(type - 17090);
    else
        nozzle = (TWaterGun::NOZZLETYPE)(type - 16090);

    changeNozzle__9TWaterGunFQ29TWaterGun11TNozzleTypeb(gpMario->mFludd, nozzle, 1);
    gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed = true;
}

//0x80004AA0
void boostPadCol(TMario *gpMario)
{
    //gpMario->mForwardSpeed = gpMario->mFloorTriangle->mValue4;
    if (gpMario->mState == TMario::SA_IDLE || !gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed)
    {
        changePlayerStatus__6TMarioFUlUlb(gpMario, TMario::SA_RUNNING, 0, 0);
        startVoice__6TMarioFUl(gpMario, TMario::V_JUMP);
    }
}

//0x80004AA4
void setGravityCol(TMario *gpMario)
{
    gpMario->mGravity = (float)(gpMario->mFloorTriangle->mValue4) / 100.0f;
}

//0x80004AA8
void antiGravityCol(TMario *gpMario)
{
    if ((gpMario->mCoordinates.y - gpMario->mFloorTriangle->mMaxHeight) > gpMario->mFloorTriangle->mValue4 &&
        gpMario->mFloorTriangle->mValue4 != 0)
    {
        return;
    }
    gpMario->mSpeed.y = 10.0f;
    if ((gpMario->mState & TMario::SA_AIRBORN) == false)
    {
        gpMario->mCoordinates.y += 1.0f;
        changePlayerStatus__6TMarioFUlUlb(gpMario, TMario::SA_FALL, 0, 0);
    }
}

void warpToLinkedCol(TMario *gpMario)
{
    CPolarSubCamera *gpCamera = (CPolarSubCamera *)*(u32 *)CPolarSubCameraInstance;
    TBGCheckData *linkedCol = gInfo.mWarpColArray->resolveCollisionWarp(gpMario->mFloorTriangle);

    if (!linkedCol)
    {
        gpMario->mCustomInfo->mCollisionTimer = 0;
        return;
    }

    float speed = Vector3D::getResultant(gpMario->mSpeed);

    if (speed > 1.0f)
    {
        gpMario->mCustomInfo->mCollisionTimer = 0;
        return;
    }

    if (!gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed)
    {
        if (gpMario->mCustomInfo->mCollisionTimer > 140)
        {
            gpMario->mCoordinates = getTriCenter(linkedCol->mVertexA, linkedCol->mVertexB, linkedCol->mVertexC);
            gpMario->mFloorTriangle = linkedCol;
            gpMario->mFloorTriangleCopy = linkedCol;
            gpMario->mFloorBelow = gpMario->mCoordinates.y;
            gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed = true;
            gpMario->mCustomInfo->mCollisionTimer = 0;
            startSoundActor__6TMarioFUl(gpMario, TMario::V_JUMP);

            gpCamera->mCoordinates.x = lerp<float>(gpCamera->mCoordinates.x, gpMario->mCoordinates.x, 0.9375f);
            gpCamera->mCoordinates.y = gpMario->mCoordinates.y + 300.0f;
            gpCamera->mCoordinates.z = lerp<float>(gpCamera->mCoordinates.z, gpMario->mCoordinates.z, 0.9375f);

            gpCamera->mHorizontalAngle = getYAngleBetween<u16>(gpMario->mCoordinates, gpCamera->mCoordinates) * 182;
        }
        else if (gpMario->mCustomInfo->mCollisionTimer > 80)
        {
            gpMario->mCustomInfo->CollisionFlags.mIsDisableInput = true;
            gpMario->mController->setEnabled(false);
            emitGetEffect__6TMarioFv(gpMario);
        }
    }
    else
    {
        if (gpMario->mCustomInfo->mCollisionTimer > 60)
        {
            gpMario->mCustomInfo->CollisionFlags.mIsDisableInput = false;
            gpMario->mController->setEnabled(false);
        }
    }
    gpMario->mCustomInfo->mCollisionTimer += 1;
}

void warpToLinkedColPreserve(TMario *gpMario, bool fluid)
{
    CPolarSubCamera *gpCamera = (CPolarSubCamera *)*(u32 *)CPolarSubCameraInstance;
    TBGCheckData *linkedCol = gInfo.mWarpColArray->resolveCollisionWarp(gpMario->mFloorTriangle);

    if (!linkedCol) return;

    if (!gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed)
    {
        gpMario->mFloorTriangle = linkedCol;
        gpMario->mFloorTriangleCopy = linkedCol;
        gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed = true;

        gpMario->mCoordinates = getTriCenter(linkedCol->mVertexA, linkedCol->mVertexB, linkedCol->mVertexC);

        gpCamera->mCoordinates.x = lerp<float>(gpCamera->mCoordinates.x, gpMario->mCoordinates.x, 0.9375f);
        gpCamera->mCoordinates.y = gpMario->mCoordinates.y + 300.0f;
        gpCamera->mCoordinates.z = lerp<float>(gpCamera->mCoordinates.z, gpMario->mCoordinates.z, 0.9375f);

        gpCamera->mHorizontalAngle = getYAngleBetween<u16>(gpMario->mCoordinates, gpCamera->mCoordinates) * 182;

        JGeometry::TVec3<float> colNormal = Vector3D::getNormal(linkedCol->mVertexA, linkedCol->mVertexB, linkedCol->mVertexC);
        JGeometry::TVec3<float> colUnit = Vector3D::getUnitVector(colNormal);

        float magnitude = fabsf__Ff(gpMario->mSpeed.x) + fabsf__Ff(gpMario->mSpeed.y) + fabsf__Ff(gpMario->mSpeed.z);

        gpMario->mAngle.y = Vector3D::getNormalAngle<u16>(colNormal) * 182;
        setPlayerVelocity__6TMarioFf(gpMario, magnitude * colUnit.x + magnitude * colUnit.z);
        gpMario->mSpeed.y = magnitude * colUnit.y;

        gpMario->mCoordinates.x += 40.0f * colUnit.x;
        gpMario->mCoordinates.y += 40.0f * colUnit.y;
        gpMario->mCoordinates.z += 40.0f * colUnit.z;

        changePlayerStatus__6TMarioFUlUlb(gpMario, TMario::SA_FALL, 0, 0);
    }
    else
    {
        gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed = (!gpMario->mController->isFramePressed(TMarioGamePad::DPAD_DOWN) && !fluid);
    }
}

inline void resetValuesOnStateChange(TMario *gpMario)
{
    switch (gpMario->mPrevState)
    {
    case (TMario::SA_JUMPSPIN1):
        gpMario->mCustomInfo->mTerminalVelocity = -75.0f * gpMario->mGravity;
        gpMario->mCustomInfo->CollisionFlags.mIsSpinBounce = false;
    case (TMario::SA_TRIPLE_J):
        gpMario->mCustomInfo->CollisionFlags.mIsDisableInput = false;
        gpMario->mController->setEnabled(true);
    }
    if (gpMario->mCustomInfo->CollisionFlags.mIsDisableInput)
    { //Patches pausing/map escaping the controller lock
        gpMario->mController->setEnabled(false);
    }
}

inline void resetValuesOnGroundContact(TMario *gpMario)
{
    if ((gpMario->mState & TMario::SA_AIRBORN) == false)
    {
        gpMario->mCustomInfo->mTerminalVelocity = -75.0f * gpMario->mGravity;
        gpMario->mCustomInfo->CollisionFlags.mIsAirborn = false;
        gpMario->mCustomInfo->CollisionFlags.mIsDisableInput = false;
    }
}

inline void resetValuesOnAirborn(TMario *gpMario)
{
    if ((gpMario->mState & TMario::SA_AIRBORN) == false)
    {
        return;
    }
}

inline void resetValuesOnCollisionChange(TMario *gpMario)
{
    if (gpMario->mFloorTriangle->mCollisionType != gpMario->mCustomInfo->mPrevCollision)
    {
        gpMario->mCustomInfo->mPrevCollision = gpMario->mFloorTriangle->mCollisionType;
        gpMario->mCustomInfo->CollisionFlags.mIsFaceUsed = false;
        gpMario->mCustomInfo->mCollisionTimer = 0;
    }
}

//0x802500B8
u32 updateCollisionContext(TMario *gpMario)
{
    resetValuesOnStateChange(gpMario);
    resetValuesOnGroundContact(gpMario);
    resetValuesOnAirborn(gpMario);
    resetValuesOnCollisionChange(gpMario);

    float marioCollisionHeight = *(float *)0x80415CC4;
    if (gpMario->mCustomInfo->mParams)
        marioCollisionHeight *= gpMario->mCustomInfo->mParams->Attributes.mSizeMultiplier;

    if (gpMario->mCeilingAbove - gpMario->mFloorBelow < marioCollisionHeight &&
        gpMario->mRoofTriangle &&
        !(gpMario->mState & TMario::SA_AIRBORN))
    {
        loserExec__6TMarioFv(gpMario);
    }
    return 1;
}

u16 masterGroundCollisionHandler(TBGCheckData *colTriangle)
{
    register TMario *gpMario;
    __asm { mr gpMario, r29 };

    u16 type = colTriangle->mCollisionType & 0x7FFF;
    switch (type)
    {
    case 16007:
    case 17007:
        checkRestoreHealth(gpMario);
        break;
    case 16010:
    case 17010:
        checkRestoreHealth(gpMario);
        break;
    case 16020:
    case 17020:
        boostPadCol(gpMario);
        break;
    case 16021:
    case 17021:
        setGravityCol(gpMario);
        break;
    case 16040:
    case 17040:
        warpToLinkedCol(gpMario);
        break;
    case 16041:
    case 16042:
        warpToLinkedColPreserve(gpMario, false);
        break;
    case 17041:
    case 17042:
        warpToLinkedColPreserve(gpMario, true);
        break;
    case 16080:
    case 17080:
        checkIsCannonType(gpMario);
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
        changeNozzleType(gpMario, type);
        break;
    }
    return type;
}

u32 masterAllCollisionHandler(TMario *gpMario)
{
    u16 type = gpMario->mFloorTriangle->mCollisionType & 0x7FFF;
    switch (type)
    {
    case 16022:
    case 17022:
        setGravityCol(gpMario);
        break;
    case 16023:
    case 17023:
        antiGravityCol(gpMario);
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
        changeNozzleType(gpMario, type);
        break;
    }
    return gpMario->mState;
}

kmCall(0x80250CA0, &masterGroundCollisionHandler);
kmCall(0x8025059C, &masterAllCollisionHandler);