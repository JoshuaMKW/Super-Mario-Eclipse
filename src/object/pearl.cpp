#include "object/pearl.hxx"

#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/raw_fn.hxx>

TPearlCracked::TPearlCracked(const char *name)
    : TMapObjGeneral(name), mState(State::STATE_NORMAL) {}

void TPearlCracked::control() {
    TMapObjGeneral::control();

    switch (mState) {
    case State::STATE_NORMAL:
        if (checkMarioPounding(gpMarioAddress)) {
            swapToCracked();
        }
        break;
    case State::STATE_BROKEN:
        if (mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
            spawnShine();
            mState = State::STATE_RESTING;
        }
        break;
    default:
        break;
    }
}

void TPearlCracked::initMapCollisionData() {
    mCollisionManager = new TMapCollisionManager(1, "mapObj", this);
    mCollisionManager->init("pearl", 1, nullptr);
}

void TPearlCracked::initMapObj() {
    TMapObjGeneral::initMapObj();

    if (mCollisionManager) {
        mCollisionManager->changeCollision(0);
        mCollisionManager->mCurrentMapCollision->setAllActor(this);
    }
}

void TPearlCracked::setGroundCollision() {
    auto *model = getModel();
    if (mCollisionManager) {
        mCollisionManager->mCurrentMapCollision->_5C &= 0xFFFFFFFE;  // Enable
        mCollisionManager->mCurrentMapCollision->moveMtx(*model->mJointArray);
    }
}

bool TPearlCracked::checkMarioRiding(TMario *player) {
    if (gpMarioAddress->mFloorTriangle->mOwner != this)
        return false;

    return SMS_IsMarioTouchGround4cm__Fv();
}

bool TPearlCracked::checkMarioPounding(TMario *player) {
    if (!checkMarioRiding(player)) {
        return false;
    }

    if (player->mState == TMario::STATE_G_POUND || player->mState == 0x24DF) {
        return true;
    }

    if (player->mPrevState == TMario::STATE_G_POUND || player->mPrevState == 0x24DF) {
        return true;
    }

    return false;
}

void TPearlCracked::swapToCracked() {
    if (mCollisionManager) {
        mCollisionManager->mCurrentMapCollision->_5C |= 1;  // Disable
    }
    playFractureAnim();
    mState = State::STATE_BROKEN;
}

void TPearlCracked::playFractureAnim() {
    if (!mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        return;
    }

    if (gpMSound->gateCheck(MSD_SE_OBJ_GLASS_BREAK)) {
        MSoundSE::startSoundActor(MSD_SE_OBJ_GLASS_BREAK, reinterpret_cast<Vec *>(&mTranslation), 0,
                                  nullptr, 0, 4);
    }

    mActorData->setBck("pearl_fracture");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.0f;
    mModelLoadFlags &= ~0x100;
}

void TPearlCracked::spawnShine() {
    auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
    u16 keyCode   = JDrama::TNameRef::calcKeyCode("PearlShine");
    if (TNameRef *p = nameRef->searchF(keyCode, "PearlShine")) {
        TShine *shine       = reinterpret_cast<TShine *>(p);
        shine->mTranslation = mTranslation;
        shine->mTranslation.y += 100.0f;
        shine->appearWithDemo("PearlCamera");
    }
}

ObjData pearlData{.mMdlName  = "pearl",
                  .mObjectID = 0x40000F81 /*0x80000FFF*/,
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
                  .mKeyCode          = cexp_calcKeyCode("Pearl")};