#include <Dolphin/GX.h>
#include <Dolphin/MTX.h>
#include <Dolphin/types.h>

#include "player.hxx"
#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>
#include <JSystem/J3D/J3DModel.hxx>
#include <JSystem/J3D/J3DModelLoaderDataBase.hxx>
#include <JSystem/JDrama/JDRNameRef.hxx>
#include <SMS/Enemy/Conductor.hxx>
#include <SMS/M3DUtil/MActor.hxx>
#include <SMS/M3DUtil/MActorKeeper.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/ItemManager.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Map/BGCheck.hxx>
#include <SMS/MapObj/MapObjWave.hxx>
#include <SMS/Player/Mario.hxx>
#include <SMS/Strategic/Strategy.hxx>

#include "object/iron_crate.hxx"
#include "stage.hxx"

TIronCrate::TIronCrate(const char *name)
    : TMapObjGeneral(name), m_is_hit(false), m_is_dead(false) {}

void TIronCrate::makeMActors() {
    mActorKeeper = new TMActorKeeper(mLiveManager, 1);
    if (mModelLoadFlags & 0x8000)
        mActorKeeper->mModelFlags = 0x112F0000;
    else
        mActorKeeper->mModelFlags = 0x102F0000;

    mActorData       = mActorKeeper->createMActor("rocketbox.bmd", getSDLModelFlag());
    m_model_datas[0] = mActorData;
    m_model_datas[1] = mActorKeeper->createMActor("rocketbox_crash.bmd", getSDLModelFlag());

    if (mModelLoadFlags & 0x4000) {
        m_model_datas[0]->setLightID(0);
        m_model_datas[0]->_40 = 0;
        m_model_datas[1]->setLightID(0);
        m_model_datas[1]->_40 = 0;
    }

    calcRootMatrix();
    m_model_datas[0]->calc();
    m_model_datas[0]->viewCalc();
    m_model_datas[1]->calc();
    m_model_datas[1]->viewCalc();
}

void TIronCrate::initMapCollisionData() {
    mCollisionManager = new TMapCollisionManager(1, "mapObj", this);
    mCollisionManager->init("rocketbox", 1, nullptr);
}

void TIronCrate::initMapObj() {
    TMapObjGeneral::initMapObj();

    if (mCollisionManager) {
        mCollisionManager->changeCollision(0);
        mCollisionManager->mCurrentMapCollision->setAllActor(this);
    }
}

void TIronCrate::setGroundCollision() {
    auto *model = getModel();
    if (mCollisionManager) {
        if (m_is_dead) {
            mCollisionManager->mCurrentMapCollision->_5C |= 1;  // Disable
        } else {
            mCollisionManager->mCurrentMapCollision->_5C &= ~1;  // Enable
            mCollisionManager->mCurrentMapCollision->moveMtx(*model->mJointArray);
        }
    }
}

void TIronCrate::control() {
    TMapObjGeneral::control();

    if (isFragmentAnimComplete() && !m_is_dead) {
        makeObjDead();
        m_is_dead = true;
    }

    if (m_is_dead || m_is_hit) {
        return;
    }

    //for (size_t i = 0; i < mNumObjs; ++i) {
    //    if (mCollidingObjs[i]->mObjectID == 0x1000001E) {
    //        TSpineEnemy *bomb = reinterpret_cast<TSpineEnemy *>(mCollidingObjs[i]);
    //        if (!isDamageToCannon__8TBombHeiFv(bomb)) {
    //            continue;
    //        }

    //        playFragmentAnim();

    //        if (PSVECDistance(gpMarioAddress->mTranslation, mTranslation) < 600.0f) {
    //            // Hurt player
    //            gpMarioAddress->decHP(1);
    //            gpMarioAddress->mAttributes.mIsVisible = true;
    //        }

    //        m_is_hit = true;
    //        break;
    //    }
    //}

    for (void *obj : gpStrategy->mEnemyGroup->mViewObjList) {
        THitActor *actor = static_cast<THitActor *>(obj);
        if (actor->mObjectID == 0x1000001E) {
            if (!isDamageToCannon__8TBombHeiFv(actor)) {
                continue;
            }

            if (PSVECDistance(actor->mTranslation, mTranslation) > 300.0f) {
                continue;
            }

            if (PSVECDistance(gpMarioAddress->mTranslation, mTranslation) < 600.0f) {
                // Hurt player
                gpMarioAddress->decHP(1);
            }

            playFragmentAnim();
            m_is_hit = true;
            break;
        }

        if (actor->mObjectID == 0x1000001F) {
            if (PSVECDistance(actor->mTranslation, mTranslation) > 300.0f) {
                continue;
            }

            if (PSVECDistance(gpMarioAddress->mTranslation, mTranslation) < 600.0f) {
                // Hurt player
                gpMarioAddress->decHP(1);
            }

            playFragmentAnim();
            m_is_hit = true;
            break;
        }
    }
}

bool TIronCrate::isFragmentAnimComplete() const {
    return mActorData->isCurAnmAlreadyEnd(MActor::BCK) && m_is_hit;
}

static const char *s_explosion_name = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x94\x9A\x94\xAD\x83"
                                      "\x7D\x83\x6C\x81\x5B\x83\x57\x83\x83\x81\x5B";

void TIronCrate::playFragmentAnim() {
    mActorData = m_model_datas[1];

    mActorData->setBck("rocketbox_crash");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 0.8f;
    mModelLoadFlags &= ~0x100;

    // Play fragment
    if (gpMSound->gateCheck(MSD_SE_BS_MKP_EXPLOSION_S)) {
        MSoundSE::startSoundActor(MSD_SE_BS_MKP_EXPLOSION_S, mTranslation, 0, nullptr, 0, 4);
    }

    /*void *explosion = gpConductor->makeOneEnemyAppear(mTranslation, s_explosion_name, 1);
    if (explosion) {
        generate__16TEffectExplosionFRQ29JGeometry8TVec3_f(explosion, mTranslation, mScale);
    }*/
}

bool TIronCrate::receiveMessage(THitActor *sender, u32 message) {
    if (sender->mObjectID == 0x1000001E || sender->mObjectID == 0x1000001F) {
        playFragmentAnim();

        if (PSVECDistance(gpMarioAddress->mTranslation, mTranslation) < 600.0f) {
            // Hurt player
            gpMarioAddress->decHP(1);
        }

        m_is_hit = true;

        return true;
    }

    if (TMapObjGeneral::receiveMessage(sender, message)) {
        return true;
    }

    return false;
}

// void TIronCrate::touchPlayer(THitActor *actor) {
//     if (actor->mObjectID == OBJECT_ID_MARIO) {
//         // Hurt player
//         TMario *player = reinterpret_cast<TMario *>(actor);
//         player->receiveMessage(this, 14);
//     }
// }

void TIronCrate::calc() {
    TMapObjGeneral::calc();

    if (m_is_dead) {
        return;
    }

    // TVec3f scale(1.0f, 1.0f, 1.0f);
    // scale *= mScale;

    // emitAndScale(0x1F6, 3, &mTranslation, scale);
    // emitAndScale(0x1C6, 1, &mTranslation, scale);
}

static hit_data ironCrate_hit_data{.mAttackRadius  = 220.0f,
                                   .mAttackHeight  = 220.0f,
                                   .mReceiveRadius = 170.0f,
                                   .mReceiveHeight = 170.0f};

static obj_hit_info ironCrate_collision_data{
    ._00 = 1, .mType = 0xDC000000, ._08 = 0, .mHitData = &ironCrate_hit_data};

ObjData ironCrateData{.mMdlName  = "rocketbox",
                      .mObjectID = 0x80000412 /*0x80000FFF*/,
                      .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                      .mObjKey           = gUnkManagerName,  // const_cast<char *>("waterballoon"),
                      .mAnimInfo         = nullptr,
                      .mObjCollisionData = &ironCrate_collision_data,
                      .mMapCollisionInfo = nullptr,
                      .mSoundInfo        = nullptr,
                      .mPhysicalInfo     = nullptr,
                      .mSinkData         = nullptr,
                      ._28               = nullptr,
                      .mBckMoveData      = nullptr,
                      ._30               = 50.0f,
                      .mUnkFlags = 0x0 /*0x02130100*/,  // | 0x100 = disable movement (freeze)
                      .mKeyCode  = cexp_calcKeyCode("IronCrate")};