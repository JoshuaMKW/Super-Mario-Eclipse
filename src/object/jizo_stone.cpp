#include "object/jizo_stone.hxx"

#include <SMS/MSound/MSoundSESystem.hxx>

TJizoStone::TJizoStone(const char *name) : TMapObjGeneral(name) {}

void TJizoStone::touchWater(THitActor *actor) { playBoingAnim(); }

void TJizoStone::initMapCollisionData() {
    mCollisionManager = new TMapCollisionManager(1, "mapObj", this);
    mCollisionManager->init("jizo_stone", 1, nullptr);
}

void TJizoStone::initMapObj() {
    TMapObjGeneral::initMapObj();

    if (mCollisionManager) {
        mCollisionManager->changeCollision(0);
        mCollisionManager->mCurrentMapCollision->setAllActor(this);
    }
}

void TJizoStone::setGroundCollision() {
    auto *model = getModel();
    if (mCollisionManager) {
        mCollisionManager->mCurrentMapCollision->_5C &= 0xFFFFFFFE;  // Enable
        mCollisionManager->mCurrentMapCollision->moveMtx(*model->mJointArray);
    }
}

void TJizoStone::playBoingAnim() {
    if (!mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        return;
    }

    if (gpMSound->gateCheck(MSD_SE_EN_SAMBOHEAD_W_HIT)) {
        MSoundSE::startSoundActor(MSD_SE_EN_SAMBOHEAD_W_HIT, reinterpret_cast<Vec *>(&mTranslation),
                                           0, nullptr, 0, 4);
    }

    mActorData->setBck("jizo_stone_boing");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.5f;
    mModelLoadFlags &= ~0x100;
}

ObjData jizoStoneData{.mMdlName  = "jizo_stone",
                     .mObjectID = 0x40000F80 /*0x80000FFF*/,
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
                     .mKeyCode          = cexp_calcKeyCode("JizoStone")};