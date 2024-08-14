#include "object/button.hxx"

#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

TButtonSwitch::TButtonSwitch(const char *name)
    : TMapObjGeneral(name), mState(State::STATE_NORMAL), mFlagID(0x50009),
      mPressedSoundID(MSD_SE_OBJ_AP_BUTTON) {}

void TButtonSwitch::load(JSUMemoryInputStream &in) {
    TMapObjGeneral::load(in);

    int flagID;
    in.read(&flagID, 4);
    if (flagID != -1) {
        mFlagID = flagID;
    }

    int soundID;
    in.read(&soundID, 4);
    if (soundID != -1) {
        mPressedSoundID = soundID;
    }

    in.read(&mNeedsPound, 1);
}

void TButtonSwitch::control() {
    TMapObjGeneral::control();

    switch (mState) {
    case State::STATE_NORMAL: {
        bool pressed = mNeedsPound ? checkMarioPounding(gpMarioAddress)
                                   : checkMarioRiding(gpMarioAddress);
        if (pressed) {
            swapToPressed();
            mState = State::STATE_PRESSING;
        }
        break;
    }
    case State::STATE_PRESSING:
        if (mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
            TFlagManager::smInstance->setFlag(mFlagID, 1);
            mState = State::STATE_PRESSED;
        }
        break;
    case State::STATE_PRESSED:
        if (TFlagManager::smInstance->getFlag(mFlagID) == 0) {
            swapToUnpressed();
            mState = State::STATE_RISING;
        }
        break;
    case State::STATE_RISING:
        if (mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
            TFlagManager::smInstance->setFlag(mFlagID, 0);
            mState = State::STATE_NORMAL;
        }
    default:
        break;
    }
}

void TButtonSwitch::initMapCollisionData() {
    mCollisionManager = new TMapCollisionManager(1, "mapObj", this);
    mCollisionManager->init("button_switch", 1, nullptr);
}

void TButtonSwitch::initMapObj() {
    TMapObjGeneral::initMapObj();

    if (mCollisionManager) {
        mCollisionManager->changeCollision(0);
        mCollisionManager->mCurrentMapCollision->setAllActor(this);
    }
}

void TButtonSwitch::setGroundCollision() {
    auto *model = getModel();
    if (mCollisionManager) {
        if (mState == State::STATE_PRESSED || mState == State::STATE_PRESSING) {
            mCollisionManager->mCurrentMapCollision->_5C |= 1;  // Disable
        } else {
            mCollisionManager->mCurrentMapCollision->_5C &= ~1;  // Enable
            mCollisionManager->mCurrentMapCollision->moveMtx(*model->mJointArray);
        }
    }
}

bool TButtonSwitch::checkMarioRiding(TMario *player) {
    if (gpMarioAddress->mFloorTriangle->mOwner != this)
        return false;

    return SMS_IsMarioTouchGround4cm__Fv();
}

bool TButtonSwitch::checkMarioPounding(TMario *player) {
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

void TButtonSwitch::swapToPressed() {
    if (mCollisionManager) {
        mCollisionManager->mCurrentMapCollision->_5C |= 1;  // Disable
    }
    playPressedAnim();
}

void TButtonSwitch::swapToUnpressed() {
    if (mCollisionManager) {
        mCollisionManager->mCurrentMapCollision->_5C &= ~1;  // Enable
    }
    playUnpressedAnim();
}

void TButtonSwitch::playPressedAnim() {
    if (!mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        return;
    }

    if (gpMSound->gateCheck(mPressedSoundID)) {
        MSoundSE::startSoundActor(mPressedSoundID, reinterpret_cast<Vec *>(&mTranslation), 0,
                                  nullptr, 0, 4);
    }

    mActorData->setBck("button_switch_press");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.0f;
    mModelLoadFlags &= ~0x100;
}

void TButtonSwitch::playUnpressedAnim() {
    mActorData->setBck("button_switch_unpress");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.0f;
    mModelLoadFlags &= ~0x100;
}

ObjData buttonSwitchData{.mMdlName  = "button_switch",
                         .mObjectID = 0x40000F81 /*0x80000FFF*/,
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
                         .mKeyCode          = cexp_calcKeyCode("ButtonSwitch")};