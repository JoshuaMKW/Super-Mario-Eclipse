#include <Dolphin/GX.h>
#include <Dolphin/MTX.h>
#include <Dolphin/types.h>

#include "player.hxx"
#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>
#include <JSystem/J3D/J3DModel.hxx>
#include <JSystem/J3D/J3DModelLoaderDataBase.hxx>
#include <JSystem/JDrama/JDRNameRef.hxx>
#include <SMS/M3DUtil/MActor.hxx>
#include <SMS/M3DUtil/MActorKeeper.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/ItemManager.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Map/BGCheck.hxx>
#include <SMS/Player/Mario.hxx>

#include "object/follow_key.hxx"
#include "object/key_chest.hxx"

TKeyChest::TKeyChest(const char *name)
    : TMapObjGeneral(name), mOpening(false), mHasFinishedSounds(false) {}

void TKeyChest::initMapCollisionData() {
    mCollisionManager = new TMapCollisionManager(1, "mapObj", this);
    mCollisionManager->init("key_chest", 1, nullptr);
}

void TKeyChest::initMapObj() {
    TMapObjGeneral::initMapObj();

    if (mCollisionManager) {
        mCollisionManager->changeCollision(0);
        mCollisionManager->mCurrentMapCollision->setAllActor(this);
    }
}

void TKeyChest::setGroundCollision() {
    auto *model = getModel();
    if (mCollisionManager) {
        mCollisionManager->mCurrentMapCollision->_5C &= 0xFFFFFFFE;  // Enable
        mCollisionManager->mCurrentMapCollision->moveMtx(*model->mJointArray);
    }
}

void TKeyChest::control() {
    TMapObjGeneral::control();

    if (mOpening) {
        // Play chest next open sound when first sound has ended
        bool isNextSoundReady = gpMSound->gateCheck(0x1967) && gpMSound->gateCheck(0x1966);
        if (!mHasFinishedSounds && isNextSoundReady) {
            MSoundSE::startSoundActor(0x1966, mTranslation, 0, nullptr, 0, 4);
            mHasFinishedSounds = true;
        }

        if (mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
            mOpening = false;
            spawnShine();
        }
    }
}

void TKeyChest::playIdleAnim() {}

void TKeyChest::playOpenAnim() {
    mActorData->setBck("key_chest_open");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 0.5f;
    mModelLoadFlags &= ~0x100;

    // Play chest open sound
    if (gpMSound->gateCheck(0x1967)) {
        MSoundSE::startSoundActor(0x1967, mTranslation, 0, nullptr, 0, 4);
    }
}

bool TKeyChest::receiveMessage(THitActor *sender, u32 message) {
    if (TMapObjGeneral::receiveMessage(sender, message)) {
        return true;
    }

    if (message == MESSAGE_KEY_OPEN_CHEST) {
        playOpenAnim();
        mOpening = true;
        return true;
    }

    return false;
}

void TKeyChest::spawnShine() {
    auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
    u16 keyCode   = JDrama::TNameRef::calcKeyCode("TreasureShine");
    if (TNameRef *p = nameRef->searchF(keyCode, "TreasureShine")) {
        TShine *shine       = reinterpret_cast<TShine *>(p);
        shine->mTranslation = mTranslation;
        shine->mTranslation.y += 100.0f;
        shine->appearWithDemo("TreasureCamera");
    }
}

void TKeyChest::touchPlayer(THitActor *actor) {
    if (actor->mObjectID == OBJECT_ID_MARIO) {
        TMario *player = reinterpret_cast<TMario *>(actor);
        auto *data     = SME::Player::getEclipseData(player);
        if (data->mIsHoldingKey) {
            auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
            u16 keyCode   = JDrama::TNameRef::calcKeyCode("TreasureKey");
            if (TNameRef *p = nameRef->searchF(keyCode, "TreasureKey")) {
                TFollowKey *foll = reinterpret_cast<TFollowKey *>(p);
                foll->receiveMessage(this, MESSAGE_KEY_OPEN_CHEST);
                data->mIsHoldingKey = false;
            }
        }
    }
}
static hit_data keyChest_hit_data{.mAttackRadius  = 300.0f,
                                  .mAttackHeight  = 300.0f,
                                  .mReceiveRadius = 300.0f,
                                  .mReceiveHeight = 300.0f};

static obj_hit_info keyChest_collision_data{
    ._00 = 1, .mType = 0xFC000000, ._08 = 0, .mHitData = &keyChest_hit_data};

ObjData keyChestData{.mMdlName  = "key_chest",
                     .mObjectID = 0x80000400 /*0x80000FFF*/,
                     .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                     .mObjKey           = gUnkManagerName,  // const_cast<char *>("waterballoon"),
                     .mAnimInfo         = nullptr,
                     .mObjCollisionData = &keyChest_collision_data,
                     .mMapCollisionInfo = nullptr,
                     .mSoundInfo        = nullptr,
                     .mPhysicalInfo     = nullptr,
                     .mSinkData         = nullptr,
                     ._28               = nullptr,
                     .mBckMoveData      = nullptr,
                     ._30               = 50.0f,
                     .mUnkFlags         = 0x10004000 /*0x02130100*/,
                     .mKeyCode          = cexp_calcKeyCode("TreasureChest")};