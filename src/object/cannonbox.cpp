#include "object/cannonbox.hxx"

#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>

TCannonBox::TCannonBox(const char *name) : TMapObjGeneral(name) {}

void TCannonBox::loadAfter() { mObjectID = 0x40000F90; }

void TCannonBox::control() {}

bool TCannonBox::receiveMessage(THitActor *sender, u32 message) {
    if (sender->mObjectID == 0x400003A1) {
        TFlagManager::smInstance->incFlag(0x60005, 1);
        playCollectAnim();
        return true;
    }

    return TMapObjGeneral::receiveMessage(sender, message);
}

void TCannonBox::initMapCollisionData() {
    mCollisionManager = new TMapCollisionManager(1, "mapObj", this);
    mCollisionManager->init("cannonbox", 1, nullptr);
}

void TCannonBox::initMapObj() {
    TMapObjGeneral::initMapObj();

    if (mCollisionManager) {
        mCollisionManager->changeCollision(0);
        mCollisionManager->mCurrentMapCollision->setAllActor(this);
    }
}

void TCannonBox::setGroundCollision() {
    auto *model = getModel();
    if (mCollisionManager) {
        mCollisionManager->mCurrentMapCollision->_5C &= 0xFFFFFFFE;  // Enable
        mCollisionManager->mCurrentMapCollision->moveMtx(*model->mJointArray);
    }
}

void TCannonBox::calc() { TMapObjGeneral::calc(); }

void TCannonBox::playCollectAnim() {
    mActorData->setBck("cannonbox_collect");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.0f;
    mModelLoadFlags &= ~0x100;

    size_t collected = TFlagManager::smInstance->getFlag(0x60005);

    if (gpMSound->gateCheck(MSD_SE_FGM_SOCCER_GOAL)) {
        JAISound *sound =
            MSoundSE::startSoundActor(MSD_SE_FGM_SOCCER_GOAL, mTranslation, 0, nullptr, 0, 4);
        if (sound) {
            sound->setPitch(1.0f + (collected - 1) * 0.2f, 0, 0);
        }
    }
}

ObjData cannonBoxData{.mMdlName  = "cannonbox",
                      .mObjectID = 0x40000F90 /*0x80000FFF*/,
                      .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                      .mObjKey           = gUnkManagerName,  // const_cast<char *>("waterballoon"),
                      .mAnimInfo         = nullptr,
                      .mObjCollisionData = nullptr,
                      .mMapCollisionInfo = nullptr,
                      .mSoundInfo        = nullptr,
                      .mPhysicalInfo     = nullptr,
                      .mSinkData         = nullptr,
                      ._28               = nullptr,
                      .mBckMoveData      = nullptr,
                      ._30               = 50.0f,
                      .mUnkFlags         = 0x4 /*0x02130100*/,
                      .mKeyCode          = cexp_calcKeyCode("CannonBox")};