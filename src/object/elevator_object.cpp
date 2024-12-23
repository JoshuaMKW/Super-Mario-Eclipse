﻿
#include "object/elevator_object.hxx"

#include <SMS/Manager/FlagManager.hxx>

TElevatorObject::TElevatorObject(const char *name) : TMapObjGeneral(name) {
    flagId    = 393248;
    hasPlayed = false;
}

void TElevatorObject::load(JSUMemoryInputStream &in) { TMapObjGeneral::load(in); }

void TElevatorObject::control() {
    TMapObjGeneral::control();

    if (TFlagManager::smInstance->getFlag(flagId)) {
        if (!hasPlayed) {
            playAnim();
            hasPlayed = true;
        }
    }
}


void TElevatorObject::initMapCollisionData() {
    mCollisionManager = new TMapCollisionManager(1, "mapObj", this);
    mCollisionManager->init("elevator_door", 1, nullptr);
}

void TElevatorObject::initMapObj() {
    TMapObjGeneral::initMapObj();

    if (mCollisionManager) {
        mCollisionManager->changeCollision(0);
        mCollisionManager->mCurrentMapCollision->setAllActor(this);
    }
}

void TElevatorObject::setGroundCollision() {
    auto *model = getModel();
    if (mCollisionManager) {
        mCollisionManager->mCurrentMapCollision->_5C &= 0xFFFFFFFE;  // Enable
        mCollisionManager->mCurrentMapCollision->moveMtx(*model->mJointArray);
    }
}

void TElevatorObject::playAnim() {

    mActorData->setBck("elevator_door");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.0f;
    mModelLoadFlags &= ~0x100;
    
}

ObjData elevatorObjectData{.mMdlName  = "elevator_door",
                           .mObjectID = 0x80000401 /*0x80000FFF*/,
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
                           .mKeyCode          = cexp_calcKeyCode("ElevatorDoor")};