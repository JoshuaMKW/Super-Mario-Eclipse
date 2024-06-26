#include "object/mafia_pianta.hxx"
#include <SMS/NPC/NpcBase.hxx>

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
}
void TMafiaPianta::loadAfter() { TBaseNPC::loadAfter(); }

void TMafiaPianta::control() {
    TBaseNPC::control();
    TMario *player = gpMarioAddress;
    TVec3f playerLoc = player->mTranslation;

   
    TVec3f enemyToPlayer = playerLoc - this->mTranslation;

    float dist           = enemyToPlayer.magnitude();

    if (dist < mDistance) {
    
        enemyToPlayer.normalize();
        TVec3f forward = TVec3f(sinf(DEGREES_TO_RADIANS(mRotation.y)), 0.0f,
                                cosf(DEGREES_TO_RADIANS(mRotation.y)));
        forward.normalize();
        float dot = enemyToPlayer.dot(forward);

        float angleToPlayer = RADIANS_TO_DEGREES(acosf(dot));
        OSReport("Close Enough To Mafia Pianta; MaxAngle: %f, actualAngle: %f \n", mMaxAngle, angleToPlayer);

        if (fabsf(angleToPlayer)<= (mMaxAngle/2.0f)) {
            
            gpMarioAddress->warpRequest(mTeleportLoc, 0);
    
            player->setPlayerVelocity(0);
        }
    }
}

void TMafiaPianta::perform(u32 flags, JDrama::TGraphics *graphics) {
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