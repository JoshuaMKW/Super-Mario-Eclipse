#include "object/emario_portal.hxx"

#include <SMS/MSound/MSoundSESystem.hxx>

TEMarioPortal::TEMarioPortal(const char *name) : TMapObjGeneral(name) {}

void TEMarioPortal::openPortal(const TVec3f &at, const TVec3f &look_nrm) {
    if (!m_is_closed) {
        return;
    }

    mActorData->setBck("emario_portal_open");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.0f;
    mModelLoadFlags &= ~0x100;

    if (gpMSound->gateCheck(MSD_SE_IT_APPEAR)) {
        MSoundSE::startSoundActor(MSD_SE_IT_APPEAR, reinterpret_cast<Vec *>(&mTranslation), 0,
                                  nullptr, 0, 4);
    }

    m_is_opening = true;
}

void TEMarioPortal::closePortal() {
    if (m_is_closed) {
        return;
    }

    mActorData->setBck("emario_portal_close");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.0f;
    mModelLoadFlags &= ~0x100;

    m_is_closing = true;
}

void TEMarioPortal::control() {
    if (!m_is_closed) {
        if (gpMSound->gateCheck(MSD_SE_OBJ_QUAKE)) {
            JAISound *sound = MSoundSE::startSoundActor(
                MSD_SE_OBJ_QUAKE, reinterpret_cast<Vec *>(&mTranslation), 0, nullptr, 0, 4);
            if (sound) {
                sound->setVolume(0.5f, 0, 0);
            }
        }
    }

    if (m_is_closing) {
        if (mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
            m_is_closing = false;
            m_is_closed  = true;
        }
    } else if (m_is_opening) {
        if (mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
            m_is_opening = false;
            m_is_closed  = false;
        }
    }
}

void TEMarioPortal::initMapObj() {
    TMapObjGeneral::initMapObj();

    //if (mCollisionManager) {
    //    mCollisionManager->changeCollision(0);
    //    mCollisionManager->mCurrentMapCollision->setAllActor(this);
    //}
}

bool TEMarioPortal::receiveMessage(THitActor *sender, u32 message) {
    //if (sender->mObjectID == 0x1000002B || sender->mObjectID == 0x4000022E) {
    //    playFragmentAnim();

    //    if (PSVECDistance(gpMarioAddress->mTranslation, mTranslation) < 600.0f) {
    //        // Hurt player
    //        gpMarioAddress->decHP(gpMarioAddress->mHealth);
    //        gpMarioAddress->mAttributes.mIsVisible = true;
    //        gpMarioAddress->mTranslation.y         = mTranslation.y - 200.0f;
    //    }

    //    m_is_hit = true;

    //    return true;
    //}
    OSReport("[EMarioPortal] Received msg %lu for sender 0x%08X\n", message, sender);

    if (TMapObjGeneral::receiveMessage(sender, message)) {
        return true;
    }

    return false;
}

TBGCheckData *TEMarioPortal::getNearestSurface(const TVec3f &at, const TVec3f &look_nrm) {
    return nullptr;
}

TVec3f TEMarioPortal::getRotationFromNormal(const TVec3f &nrm) { return TVec3f(); }

static hit_data emarioPortal_hit_data{.mAttackRadius  = 300.0f,
                                      .mAttackHeight  = 300.0f,
                                      .mReceiveRadius = 300.0f,
                                      .mReceiveHeight = 300.0f};

static obj_hit_info emarioPortal_collision_data{
    ._00 = 1, .mType = 0xDC000000, ._08 = 0, .mHitData = &emarioPortal_hit_data};

ObjData emarioPortalData{.mMdlName          = "emario_portal",
                         .mObjectID         = 0x80000412 /*0x80000FFF*/,
                         .mLiveManagerName  = gLiveManagerName,
                         .mObjKey           = gUnkManagerName,
                         .mAnimInfo         = nullptr,
                         .mObjCollisionData = &emarioPortal_collision_data,
                         .mMapCollisionInfo = nullptr,
                         .mSoundInfo        = nullptr,
                         .mPhysicalInfo     = nullptr,
                         .mSinkData         = nullptr,
                         ._28               = nullptr,
                         .mBckMoveData      = nullptr,
                         ._30               = 50.0f,
                         .mUnkFlags         = 0x0 /*0x02130100*/,
                         .mKeyCode          = cexp_calcKeyCode("EMarioPortal")};