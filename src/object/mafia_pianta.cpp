#include "object/mafia_pianta.hxx"
#include <BetterSMS/application.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/NPC/NpcBase.hxx>
#include <SMS/MarioUtil/MathUtil.hxx>
#include <BetterSMS/area.hxx>
#include <BetterSMS/module.hxx>
TMafiaPianta::TMafiaPianta(const char *name) : TBaseNPC(0x4000003, name) {}

TMafiaPianta ::~TMafiaPianta() {}
void TMafiaPianta::load(JSUMemoryInputStream &in) {
    TBaseNPC::load(in);

    in.read(&mMaxAngle, 4);
    in.read(&mDistance, 4);
    in.read(&mTimeForSwitch, 4);

    in.read(&mXTeleport, 4);
    in.read(&mYTeleport, 4);
    in.read(&mZTeleport, 4);

    mTeleportLoc = TVec3f(mXTeleport, mYTeleport, mZTeleport);

    int frame = SMSGetVSyncTimesPerSec();

    if (frame < 36) {
        frameCounter = (4 * frame) * mTimeForSwitch;
    } else {
        frameCounter = (4 * frame) * mTimeForSwitch;
    }
}
void TMafiaPianta::loadAfter() {
    TBaseNPC::loadAfter();
    playIdleAnim();
}

void TMafiaPianta::playIdleAnim() {
    mActorData->setBck("mom_wait");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::LOOP;
    OSReport("Playing Idle Anmation!\n");
}
void TMafiaPianta::control() {
    TLiveActor::control();
    TMario *player       = gpMarioAddress;
    TVec3f playerLoc     = player->mTranslation;
    TVec3f enemyToPlayer = playerLoc - this->mTranslation;

    float dist = enemyToPlayer.magnitude();

    OSReport("Distance From Mafia: %f, Max Distance:  %f \n", dist, mDistance);

    if (dist < mDistance) {

        enemyToPlayer.normalize();
        TVec3f forward = TVec3f(sinf(DEGREES_TO_RADIANS(mRotation.y)), 0.0f,
                                cosf(DEGREES_TO_RADIANS(mRotation.y)));
        forward.normalize();
        float dot = enemyToPlayer.dot(forward);

        float angleToPlayer = RADIANS_TO_DEGREES(acosf(dot));
        OSReport("Close Enough To Mafia Pianta; MaxAngle: %f, actualAngle: %f \n", mMaxAngle,
                 angleToPlayer);



        if (fabsf(angleToPlayer) <= (mMaxAngle / 2.0f)) {

            playSpottedAnim();
            gpMarioAddress->warpRequest(mTeleportLoc, 0);
            player->setPlayerVelocity(0);
        }
    }
 
    currentFrameCount++;

    if (currentFrameCount > frameCounter) {

        currentFrameCount = 0;
        rotatePianta();
    }

    if (mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        playIdleAnim();
    }
}

void TMafiaPianta::playSpottedAnim() {

   // mActorData->setBck("mom_mad");
  //  auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
  //  ctrl->mAnimState = J3DFrameCtrl::ONCE;
//
    if (gpMSound->gateCheck(MSD_SE_OBJ_BI_BELL)) {
        MSoundSE::startSoundActor(MSD_SE_OBJ_BI_BELL, mTranslation, 0, nullptr, 0, 4)
            ->setPitch(.5f, 0, 0);
    }
}

void TMafiaPianta::rotatePianta() {
    OSReport("ROTATE ME ROTATE ME HAHAAHA \n");
    *(f32 *)((u8 *)this + 0x1A4) = -(*(f32 *)((u8 *)this + 0x1A4));

    // moveRequest();
}

void TMafiaPianta::perform(u32 flags, JDrama::TGraphics *graphics) {

    OSReport("Running Perform \n");
    TBaseNPC::perform(flags, graphics);
}
bool TMafiaPianta::receiveMessage(THitActor *sender, u32 message) { return true; }
ObjData mafiaPiantaData{.mMdlName  = "maifa_pianta",
                        .mObjectID = 0x40000F82 /*0x80000FFF*/,
                        .mLiveManagerName =
                            gLiveManagerName,  // const_cast<char *>("木マネージャー")
                        .mObjKey   = gUnkManagerName,  // const_cast<char *>("waterballoon"),
                        .mAnimInfo = nullptr,
                        .mObjCollisionData = nullptr,
                        .mMapCollisionInfo = nullptr,
                        .mSoundInfo        = nullptr,
                        .mPhysicalInfo     = nullptr,
                        .mSinkData         = nullptr,
                        ._28               = nullptr,
                        .mBckMoveData      = nullptr,
                        ._30               = 50.0f,
                        .mUnkFlags         = 0x4 /*0x02130100*/,
                        .mKeyCode          = cexp_calcKeyCode("NPCMonteMafia")};




static int NpcControlFix(const TVec3f &vec1, float f1, TVec3f &vec2, float f2, float f3, float f4) {
    if (gpMarDirector->mAreaID == 66)
    {
        return 1;
    }
    else {
        return 1;

        //return MsIsInSight(vec1, f1, vec2, f2, f3, f4);
    }
}


SMS_PATCH_BL(SMS_PORT_REGION(0x8020674c, 0, 0, 0), NpcControlFix);
