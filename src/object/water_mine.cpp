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

#include "object/water_mine.hxx"
#include "stage.hxx"

TWaterMine::TWaterMine(const char *name)
    : TMapObjGeneral(name), m_is_hit(false), m_is_dead(false) {}

void TWaterMine::control() {
    TMapObjGeneral::control();

    mGravity = 0.0f;
    mTranslation.y =
        gpMapObjWave->getHeight(mTranslation.x, mTranslation.y + 50.0f, mTranslation.z);

    if (isFragmentAnimComplete() && !m_is_dead) {
        makeObjDead();
        m_is_dead = true;
    }

    if (m_is_dead) {
        return;
    }

    for (size_t i = 0; i < mNumObjs; ++i) {
        if (mCollidingObjs[i]->mObjectID == 0x1000002B || mCollidingObjs[i]->mObjectID == 0x4000022E) {
            playFragmentAnim();

            if (PSVECDistance(gpMarioAddress->mTranslation, mTranslation) < 600.0f) {
                // Hurt player
                gpMarioAddress->decHP(gpMarioAddress->mHealth);
                gpMarioAddress->mAttributes.mIsVisible = true;
                gpMarioAddress->mTranslation.y         = mTranslation.y - 200.0f;
            }

            m_is_hit = true;
            break;
        }

        if (mCollidingObjs[i]->mObjectID == OBJECT_ID_MARIO) {
            playFragmentAnim();
            m_is_hit = true;

            // Hurt player
            TMario *player = reinterpret_cast<TMario *>(mCollidingObjs[i]);
            player->decHP(player->mHealth);
            player->mAttributes.mIsVisible = true;
            player->mTranslation.y = mTranslation.y - 200.0f;
            break;
        }
    }
}

bool TWaterMine::isFragmentAnimComplete() const {
    return mActorData->isCurAnmAlreadyEnd(MActor::BCK) && m_is_hit;
}

static const char *s_column_water_name =
    "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x94\x9A\x94\xAD\x90\x85\x92\x8C\x83\x7D\x83\x6C\x81"
    "\x5B\x83\x57\x83\x83\x81\x5B";

static const char *s_explosion_name =
    "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x94\x9A\x94\xAD\x83\x7D\x83\x6C\x81\x5B\x83\x57\x83\x83\x81\x5B";

void TWaterMine::playFragmentAnim() {
    mActorData->setBck("seamine_fragment");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    ctrl->mFrameRate = 1.0f;
    mModelLoadFlags &= ~0x100;

    // Play fragment
    if (gpMSound->gateCheck(MSD_SE_BS_MKP_EXPLOSION_S)) {
        MSoundSE::startSoundActor(MSD_SE_BS_MKP_EXPLOSION_S, mTranslation, 0, nullptr, 0, 4);
    }

    void *columwater = gpConductor->makeOneEnemyAppear(mTranslation, s_column_water_name, 1);
    if (columwater) {
        generate__17TEffectColumWaterFRQ29JGeometry8TVec3_f(columwater, mTranslation, mScale);
    }

    void *explosion = gpConductor->makeOneEnemyAppear(mTranslation, s_explosion_name, 1);
    if (explosion) {
        generate__16TEffectExplosionFRQ29JGeometry8TVec3_f(explosion, mTranslation, mScale);
    }
}

bool TWaterMine::receiveMessage(THitActor *sender, u32 message) {
    if (sender->mObjectID == 0x1000002B || sender->mObjectID == 0x4000022E) {
        playFragmentAnim();

        if (PSVECDistance(gpMarioAddress->mTranslation, mTranslation) < 600.0f) {
            // Hurt player
            gpMarioAddress->decHP(gpMarioAddress->mHealth);
            gpMarioAddress->mAttributes.mIsVisible = true;
            gpMarioAddress->mTranslation.y         = mTranslation.y - 200.0f;
        }

        m_is_hit = true;

        return true;
    }

    if (TMapObjGeneral::receiveMessage(sender, message)) {
        return true;
    }

    return false;
}

// void TWaterMine::touchPlayer(THitActor *actor) {
//     if (actor->mObjectID == OBJECT_ID_MARIO) {
//         // Hurt player
//         TMario *player = reinterpret_cast<TMario *>(actor);
//         player->receiveMessage(this, 14);
//     }
// }

void TWaterMine::calc() {
    if (m_is_dead) {
        return;
    }

    TVec3f scale(1.0f, 1.0f, 1.0f);
    scale *= mScale;

    emitAndScale(0x1F6, 3, &mTranslation, scale);
    emitAndScale(0x1C6, 1, &mTranslation, scale);
}

static hit_data waterMine_hit_data{.mAttackRadius  = 100.0f,
                                   .mAttackHeight  = 100.0f,
                                   .mReceiveRadius = 100.0f,
                                   .mReceiveHeight = 100.0f};

static obj_hit_info waterMine_collision_data{
    ._00 = 1, .mType = 0xDC000000, ._08 = 0, .mHitData = &waterMine_hit_data};

ObjData waterMineData{.mMdlName  = "seamine",
                      .mObjectID = 0x80000410 /*0x80000FFF*/,
                      .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                      .mObjKey           = gUnkManagerName,  // const_cast<char *>("waterballoon"),
                      .mAnimInfo         = nullptr,
                      .mObjCollisionData = &waterMine_collision_data,
                      .mMapCollisionInfo = nullptr,
                      .mSoundInfo        = nullptr,
                      .mPhysicalInfo     = nullptr,
                      .mSinkData         = nullptr,
                      ._28               = nullptr,
                      .mBckMoveData      = nullptr,
                      ._30               = 50.0f,
                      .mUnkFlags =
                          0x0 /*0x02130100*/,  // | 0x100 = disable movement (freeze)
                      .mKeyCode = cexp_calcKeyCode("WaterMine")};