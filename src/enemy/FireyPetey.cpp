#include <Dolphin/math.h>

#include <JSystem/JGadget/List.hxx>
#include <JSystem/JDrama/JDRNameRefGen.hxx>
#include <JSystem/JDrama/JDRNameRefPtrListT.hxx>
#include <JSystem/JParticle/JPAResourceManager.hxx>

#include <SMS/Player/Mario.hxx>
#include <SMS/MoveBG/Coin.hxx>
#include <SMS/Manager/PollutionManager.hxx>
#include <SMS/Strategic/ObjHitCheck.hxx>
#include <SMS/Camera/CameraShake.hxx>
#include <SMS/MarioUtil/MathUtil.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/libs/constmath.hxx>

#include <enemy/firey_petey.hxx>

#include "rand.h"
#include "Enemy/Conductor.hxx"
#include "Manager/MarioParticleManager.hxx"
#include "Manager/ModelWaterManager.hxx"
#include "MSound/MSBGM.hxx"
#include "MSound/MSoundSESystem.hxx"

using namespace BetterSMS;

// Globals
static TNerveFPSleep sleep;
static TNerveFPBreakSleep breakSleep;
static TNerveFPTakeOff takeoff;
static TNerveFPFly fly;
static TNerveFPWait wait;
static TNerveFPHover hover;
static TNerveFPFireBreath fireBreath;
static TNerveFPFall fall;
static TNerveFPSwallow swallow;
static TNerveFPTumbleIn tumbleIn;
static TNerveFPTumble tumble;
static TNerveFPTumbleOut tumbleOut;


#define MAX_TORNADOS 3

bool TNerveFPWait::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);

    TVec3f distanceVector = (target->mTranslation - *gpMarioPos);

    f32 swingRange = 400.0f * 400.0f;
    if (distanceVector.dot(distanceVector) < swingRange) {
        spine->pushNerve(&wait);
        spine->pushNerve(reinterpret_cast<TNerveBase<TLiveActor> *>(theNerve__13TNerveBPSwingFv()));
        return true;
    }
    if (spine->mNerveTimer == 0) {
        target->changeBck(0x19);
        target->_02 = 0x0;
    }
    s32 timeTillAttack = 300;
    if (timeTillAttack <= spine->mNerveTimer) {
        if (!target->mActorData->isCurAnmAlreadyEnd(0x0)) {

            if (target->mNumTornados > 0) {
                spine->pushNerve(&wait);
                spine->pushNerve(
                    reinterpret_cast<TNerveBase<TLiveActor> *>(theNerve__15TNerveBPTornadoFv()));
                target->mNumTornados--;
            } else {
                spine->pushNerve(&takeoff);
                spine->pushNerve(&fireBreath);
                target->mNumTornados = MAX_TORNADOS;
            }

            float randomX = ((static_cast<float>(rand()) / static_cast<float>(0x7fff)) - 0.5f) *
                            10000.0f;
            float randomZ = ((static_cast<float>(rand()) / static_cast<float>(0x7fff)) - 0.5f) *
                            10000.0f;

            if (target->_118 < target->_114) {
                u32 *unk = ((target->_11C) + (target->_118) * 0x10);
                unk[0] = target->_F4;
                unk[1] = target->_F8;
                unk[2] = target->_FC;
                unk[3] = target->_100;
                target->_118++;
            }
            target->_F4 = 0;
            target->_F8 = (target->mTranslation.x + randomX);
            target->_FC = target->mTranslation.y;
            target->_100 = (target->mTranslation.z + randomZ);
            spine->pushNerve(
                reinterpret_cast<TNerveBase<TLiveActor> *>(theNerve__13TNerveBPPivotFv()));

            return true;
        }

    }
    return false;
}

bool TNerveFPFly::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
    dropLava(spine, target);
    if (spine->mNerveTimer == 0) {
        target->changeBck(0xB);
        target->goToRandomNextGraphNode();
        if (target->_13 == 0) {
            MSBgm::startBGM(BGM_BOSS);
            target->_13 = 1;
        }
    }

    auto travelVector = TVec3f(target->_108 - target->mTranslation.x,
                               0.0f,
                               target->_110 - target->mTranslation.z);

    f32 magnitude = travelVector.magnitude();

    if (magnitude < 100) {
        if ((target->mGraphTracer->mGraph->mNodes[target->mGraphTracer->mCurrentNode]._08 & 0x800)
            != 0) {
            spine->pushNerve(&hover);
            return true;
        }
        target->goToRandomNextGraphNode();
    }

    target->turnToCurPathNode(target->_144);

    TSpineEnemyParams *params = target->getSaveParam();

    travelVector = TVec3f(target->mTranslation.x - target->_108,
                          target->mTranslation.y - target->_10C,
                          target->mTranslation.z - target->_110);

    TVec3f velocity = travelVector;
    velocity.normalize();

    velocity.scale(-20.0f);

    target->mPositionalVelocity = velocity;

    return false;
}

bool TNerveFPTakeOff::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);

    if (spine->mNerveTimer == 0) {
        // TODO: Use asFlags
        target->mStateFlags.asU32 = target->mStateFlags.asU32 | 0x10;
        target->mStateFlags.asU32 = target->mStateFlags.asU32 | 0x80;
        target->changeBck(0xD);
    }
    if (target->mActorData->checkCurBckFromIndex(0xD) && target->mActorData->
                                                                 curAnmEndsNext(0, nullptr)) {
        target->changeBck(0xB);
    }
    if (target->mActorData->checkCurBckFromIndex(0xB)) {
        target->mTranslation.y += 5.0f;
        if (target->_10C < target->mTranslation.y) {
            target->mTranslation.y = target->_10C;
            if (target->mGraphTracer) {
                spine->pushNerve(&fly);
            } else {
                spine->pushNerve(&sleep);
            }
            return true;
        }
    }
    return false;
}

bool TNerveFPBreakSleep::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TBossPakkun *>(spine->mTarget);
    if (spine->mNerveTimer == 0) {
        target->changeBck(0x0E);
        MSBgm::stopTrackBGMs('\a', 10);
    }
    if (!target->mActorData->curAnmEndsNext(0, nullptr)) {
        return false;
    }
    if (spine->mNerveStack.depth() < spine->mNerveStack.capacity()) {
        spine->pushNerve(&fireBreath);
    }
    return true;
}

bool TNerveFPFall::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
    if (spine->mNerveTimer == 0) {
        target->mStateFlags.asU32 = target->mStateFlags.asU32 & 0xffffffef;
        target->mStateFlags.asU32 = target->mStateFlags.asU32 | 0x80;
        target->changeBck(0xA);
    }

    if (target->mActorData->checkCurBckFromIndex(0xA)) {
        if (target->mActorData->curAnmEndsNext(0, nullptr)) {
            target->changeBck(0x9);
        }
    } else if (target->mActorData->checkCurBckFromIndex(0x9)) {
        if ((target->mStateFlags.asU32 & 0x80U) == 0) {
            target->changeBck(0x8);
            gpCameraShake->startShake(UnknownShakeF, 1.0f);
            target->rumblePad(2, target->mTranslation);
        }
    } else if (target->mActorData->checkCurBckFromIndex(0x8)) {
        if (target->mActorData->curAnmEndsNext(0, nullptr)) {
            target->changeBck(0xe);
            gpCameraShake->startShake(UnknownShake10, 1.0f);
            target->rumblePad(0, target->mTranslation);
        }
    } else if (target->mActorData->checkCurBckFromIndex(0xe) && target->mActorData->
               curAnmEndsNext(0, nullptr)) {
        if (target->mNumTornados > 0) {
            spine->pushNerve(&wait);
            spine->pushNerve(
                reinterpret_cast<TNerveBase<TLiveActor> *>(theNerve__15TNerveBPTornadoFv()));
            target->mNumTornados--;
        } else {
            spine->pushNerve(&takeoff);
            spine->pushNerve(&fireBreath);
            target->mNumTornados = MAX_TORNADOS;
        }
        return true;
    }

    return false;
}

bool TNerveFPSleep::execute(TSpineBase<TLiveActor> *param1) const {
    if (param1->mNerveTimer == 0) {
        reinterpret_cast<TBossPakkun *>(param1->mTarget)->changeBck(0x17);
    }
    return false;
}

bool TNerveFPHover::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
    if (spine->mNerveTimer == 0) {
        target->changeBck(0x10);
        target->_02 = 3;
    }
    dropLava(spine, target);
    // SaveParam Again!

    // Use Params
    if (spine->mNerveTimer < 600) {
        return false;
    }
    target->_02 = 0;
    spine->pushNerve(&fly);
    return true;
}

bool TNerveFPFireBreath::execute(TSpineBase<TLiveActor> *spine) const {

    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
    MActor *peteyMActor = target->mActorData;
    if (spine->mNerveTimer == 0x0) {
        target->changeBck(0x15);

        // Set positions of collision
        for (int i = 0; i < 10; i++) {
            target->mFire[i]->mTranslation.set(target->mTranslation);
            target->mFire[i]->mVelocity.x = 0.0f;
            target->mFire[i]->mVelocity.y = 0.0f;
            target->mFire[i]->mVelocity.z = 0.0f;
            target->mFire[i]->mLifetime = 0;
        }
        peteyMActor->setFrameRate(SMSGetAnmFrameRate__Fv() * 2.0f, 0);
    }
    bool isAnimationRunning = peteyMActor->checkCurAnmFromIndex(0x15, 0);
    if (isAnimationRunning) {
        J3DFrameCtrl *frameCtrl = peteyMActor->getFrameCtrl(0);
        if ((frameCtrl->mCurFrame <= 25.0f) || (frameCtrl->mCurFrame >= 165.0)) {
            target->_02 = 0x0;
        } else {
            target->_02 = 0x2;
        }
    }
    bool isBckRunning = peteyMActor->checkCurBckFromIndex(0x14);
    if (isBckRunning) {
        peteyMActor->setFrameRate(SMSGetAnmFrameRate__Fv(), 0);
        isAnimationRunning = peteyMActor->checkCurAnmFromIndex(0x14, 0);
        if (isAnimationRunning) {
            J3DFrameCtrl *frameCtrl = peteyMActor->getFrameCtrl(0);

            // Joint 0xC
            if (frameCtrl->mCurFrame >= 15.0f && frameCtrl->mCurFrame <= 46.0f) {
                if (spine->mNerveTimer % 10 == 0) {
                    JPABaseEmitter *emitterFire = gpMarioParticleManager->emitAndBindToMtxPtr(
                        0x1a7, target->mActorData->mModel->mJointArray[0x10], 1, target);

                    for (int i = 0x1a6; i > 0x1a2; i--) {
                        gpMarioParticleManager->emitAndBindToMtxPtr(
                            i, target->mActorData->mModel->mJointArray[0x10], 1, target);
                    }
                    TFPFire *mFire;
                    bool found = false;
                    for (int i = 0; i < 10; i++) {
                        if (target->mFire[i]->mLifetime == 0) {
                            mFire = target->mFire[i];
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        mFire->mVelocity.x = 50.0f * sinf(angleToRadians(target->mRotation.y));
                        mFire->mVelocity.y = 0;
                        mFire->mVelocity.z = 50.0f * cosf(angleToRadians(target->mRotation.y));
                        mFire->mTranslation.set(*reinterpret_cast<TVec3f *>(&emitterFire->_00[84]));
                        mFire->mTranslation.y -= 237.5f;
                        mFire->mLifetime = 30 * 4;
                    }
                }
            }
        }
        bool isAnimationFinished = peteyMActor->curAnmEndsNext(0, nullptr);
        if (isAnimationFinished) {
            spine->pushNerve(&fireBreath);
            return true;
        }
    }
    bool isAnimationFinished = peteyMActor->curAnmEndsNext(0, nullptr);
    if (isAnimationFinished) {
        target->_02 = 0x0;
        target->changeBck(0x14);
        target->rumblePad(1, target->mTranslation);
    }
    return false;
}

bool TNerveFPSwallow::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
    auto params = reinterpret_cast<TBossPakkunParams *>(target->getSaveParam());
    if (spine->mNerveTimer == 0) {
        target->changeBck(0x1a);
    }
    if (target->_03[2] < params->mSLWaterMarkLimit.get()) {
        auto positonMatrix = target->getModel()->mJointArray[0x12];
        gpMarioParticleManager->emitAndBindToMtxPtr(0x15d, positonMatrix, 1, target);
        gpMarioParticleManager->emitAndBindToMtxPtr(0x15e, positonMatrix, 1, ((u8 *)target) + 0x1);

        if (target->_03[0] == 0) {
            target->_03[1] = 0;
            spine->pushNerve(&wait);
            return true;
        }
        target->changeBck(0x1a);
        target->_03[0] = 0;
        return false;

    }
    spine->pushNerve(&tumbleIn);
    target->_02 = 0;
    target->_03[0] = 0;
    return true;
}

bool TNerveFPTumbleIn::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
    if (spine->mNerveTimer == 0) {
        target->changeBck(3);
    }
    if (spine->mNerveTimer == 0x150) {
        auto positonMatrix = target->getModel()->mJointArray[0xc];
        gpMarioParticleManager->emitAndBindToMtxPtr(0xaa, positonMatrix, 0, target);
    }
    if (spine->mNerveTimer == 0x15c) {
        gpCameraShake->startShake(UnknownShakeE, 1.0);
        target->rumblePad(2, target->mTranslation);
    }
    if (target->mActorData->curAnmEndsNext(0, nullptr)) {
        spine->pushNerve(&tumble);
        return true;
    }
    return false;
}

bool TNerveFPTumbleOut::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
    auto params = reinterpret_cast<TBossPakkunParams *>(target->getSaveParam());
    if (spine->mNerveTimer == 0) {
        target->changeBck(0xE);
        gpCameraShake->startShake(UnknownShake10, 1.0);
        target->rumblePad(0, target->mTranslation);
    }
    if (target->mActorData->curAnmEndsNext(0, nullptr)) {
        if (target->mActorData->checkCurBckFromIndex(0xE)) {
            target->changeBck(0x16);
        } else {
            spine->pushNerve(&wait);
            return true;
        }
    }
    if (target->mActorData->checkCurBckFromIndex(0x16)) {
        J3DFrameCtrl *frameCtrl = target->mActorData->getFrameCtrl(0);
        if (frameCtrl->mCurFrame > 140.0f && frameCtrl->mCurFrame < 160.0f && target->_04 == 0) {
            target->_04 = 1;
            target->_03[1] = 0;
            target->_03[0] = 0;
            target->_09 = 0x32;

            if (target->mWaterEmitInfo) {
                auto jointTranslation = TVec3f(0.0f, 0.0f, 0.0f);
                target->getJointTransByIndex(0x12, &jointTranslation);
                jointTranslation.y += 250.0f;
                target->mWaterEmitInfo->mPos.set(jointTranslation);
                gpModelWaterManager->emitRequest(*target->mWaterEmitInfo);
            }

        }
        if (frameCtrl->mCurFrame > 35.0f) {
            target->_10 = 1;
        }
    }
    return false;
}

bool TNerveFPTumble::execute(TSpineBase<TLiveActor> *spine) const {
    auto target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
    auto params = reinterpret_cast<TBossPakkunParams *>(target->getSaveParam());
    if (spine->mNerveTimer == 0) {
        target->changeBck(6);
        target->_02 = 1;
    }

    if (spine->mNerveTimer < params->mSLTumbleTime.get()) {
        return false;
    }
    spine->pushNerve(&tumbleOut);
    return true;
}


TFireyPetey::TFireyPetey(const char *test)
    : TBossPakkun(test) {
    mNumTornados = MAX_TORNADOS;
}

void TFireyPetey::init(TLiveManager *liveManager) {
    TBossPakkun::init(liveManager);

    mTornado = new TFPTornado(this, "<TFPTornado>\n");
    mNavel = new TFPNavel(this, "<TFPNavel>\n");
    mHeadHit = new TFPHeadHit(this, "<TFPHeadHit>\n");

    JDrama::TNameRefGen *instance = JDrama::TNameRefGen::getInstance();
    TNameRef *root = instance->getRootNameRef();

    u16 keyCode = calcKeyCode(gUnkManagerName);

    auto group = static_cast<TIdxGroupObj *>(root->searchF(keyCode, gUnkManagerName));

    group->mViewObjList.insert(group->mViewObjList.end(), mTornado);

    group->mViewObjList.insert(group->mViewObjList.end(), mNavel);

    group->mViewObjList.insert(group->mViewObjList.end(), mHeadHit);

    mSpineBase->setNerve(&sleep);

    mMActorKeeperSecondary = new TMActorKeeper(mLiveManager, NUM_GOOP_DROPS);

    for (int i = 0; i < NUM_GOOP_DROPS; i++) {
        mKukkuBall[i] = new TKukkuBall("lavaDrop\n");
        mKukkuBall[i]->mActorData = mMActorKeeperSecondary->createMActor("torifun.bmd", 3);
        mKukkuBall[i]->init();
    }

    for (int i = 0; i < 10; i++) {
        mFire[i] = new TFPFire(this, "fire\n");
        group->mViewObjList.insert(group->mViewObjList.end(), mFire[i]);
    }
}

void TFireyPetey::perform(u32 flags, JDrama::TGraphics *graphics) {
    TBossPakkun::perform(flags, graphics);
    for (int i = 0; i < NUM_GOOP_DROPS; i++) {
        if (mKukkuBall[i] != nullptr) {
            mKukkuBall[i]->perform(flags, graphics);
        }
    }
    for (int i = 0; i < 10; i++) {
        if (mFire[i] != nullptr) {
            mFire[i]->perform(flags, graphics);
        }
    }
}

bool TFireyPetey::receiveMessage(THitActor *reciever, u32 param2) {
    if (reciever->mObjectID == 0x1000001 && mSpineBase->getLatestNerve() == &sleep) {
        mSpineBase->setNerve(&breakSleep);
        return false;
    }
    if (_02 == 3 && (reciever->mObjectID == 0x1000001) || (reciever->mObjectID == 0x100000d)) {
        if (mTranslation.y - 300.0f <= reciever->mTranslation.y) {
            if (reciever->mTranslation.y <= mTranslation.y + 1500.0f) {
                _02 = 0;
                mSpineBase->setNerve(&fall);
                return true;
            }
        }
    }
    return false;
}

void TFireyPetey::gotHipDropDamage() {
    if (mHealth != 0) {
        mHealth--;
    }
    _02 = 0;
    if (mHealth == 0) {
        if (mSpineBase->getLatestNerve() != reinterpret_cast<TNerveBase<TLiveActor> *>(
                theNerve__14TNerveBPPreDieFv())) {
            mSpineBase->setNerve(
                reinterpret_cast<TNerveBase<TLiveActor> *>(theNerve__14TNerveBPPreDieFv()));
            if (gpMSound->gateCheck(0x284e)) {
                MSoundSE::startSoundActor(0x284e, mTranslation, 0, nullptr, 0x0, 4);
            }
        }
    } else {
        if (mSpineBase->getLatestNerve() != reinterpret_cast<TNerveBase<TLiveActor> *>(
                theNerve__17TNerveBPTumbleOutFv())) {
            if (gpMSound->gateCheck(0x284d)) {
                MSoundSE::startSoundActor(0x284d, mTranslation, 0, nullptr, 0x0, 4);
            }
            mSpineBase->pushNerve(&takeoff);
            mSpineBase->pushNerve(
                reinterpret_cast<TNerveBase<TLiveActor> *>(theNerve__13TNerveBPGetUpFv()));
            mSpineBase->setNerve(
                reinterpret_cast<TNerveBase<TLiveActor> *>(theNerve__18TNerveBPStompReactFv()));
        }
    }

}

TFPTornado::TFPTornado(TBossPakkun *parent, const char *name)
    : TBPTornado(parent, name) {
}

void TFPTornado::perform(u32 flags, JDrama::TGraphics *graphics) {
    if (((*(int **)(this->mParent))[0x3c] & 1U) != 0) {
        return;
    }
    if (this->_98 == 0) {
        return;
    }
    TBPTornado::perform(flags, graphics);
    mTargetPos = *gpMarioPos;
    for (int i = 0; i < mNumObjs; i = i + 1) {
        if (mCollidingObjs[i]->mObjectID == OBJECT_ID_MARIO) {

            // Player::setFire(gpMarioAddress);
        }
    }
    gpPollution->stamp(1, mTranslation.x, mTranslation.y, mTranslation.z, 384.0f);
}

TFPFire::TFPFire(const char *name)
    : THitActor(name), mVelocity(0.0f, 0.0f, 0.0f), mLifetime(0) {
    initHitActor(0x8000027, 1, -0x7f000000, 275.0f, 275.0f, 100.0f, 100.0f);
}

void TFPFire::perform(u32 flags, JDrama::TGraphics *graphics) {
    THitActor::perform(flags, graphics);
    if (mLifetime > 0) {
        for (int iVar9 = 0; iVar9 < mNumObjs; iVar9 = iVar9 + 1) {
            if (mCollidingObjs[iVar9]->mObjectID == OBJECT_ID_MARIO) {
                OSReport("Burning\n");
                // Player::setFire(gpMarioAddress);
            }
        }
        mTranslation.add(mVelocity);
        mLifetime--;
    }
}

bool TFPNavel::receiveMessage(THitActor *sender, u32 msg) {

    if (sender->mObjectID == 0x1000001) {
        return false;
    }
    if (mParent->_02 == 1) {
        if (sender->mObjectID == -0x7fffffff && msg == 1) {
            reinterpret_cast<TFireyPetey *>(mParent)->gotHipDropDamage();
            return true;
        }
    }

    return false;
}

bool TFPHeadHit::receiveMessage(THitActor *sender, u32 msg) {

    auto params = reinterpret_cast<TBossPakkunParams *>(mParent->getSaveParam());

    if (mParent->mSpineBase->getLatestNerve() == &sleep) {
        return mParent->receiveMessage(sender, msg);
    }
    if (mParent->_02 == 3 && (sender->mObjectID == 0x1000001) || (sender->mObjectID == 0x100000d)) {
        mParent->_02 = 0;
        mParent->mSpineBase->setNerve(&fall);
        if (gpMSound->gateCheck(0x2817)) {
            MSoundSE::startSoundActor(0x2817, mTranslation, 0, nullptr, 0x0, 4);
        }
        return true;
    }
    if (mParent->_02 == 2) {
        if ((sender->mObjectID == 0x1000001) && msg == 0xf) {

            // Angle check
            // float xPosDifference = (mTranslation.x - gpMarioPos->x);
            // float zPosDifference = (mTranslation.z - gpMarioPos->z);
            //
            // float angle;
            //
            // if(zPosDifference == 0.0f) {
            //     if(xPosDifference < zPosDifference) {
            //         angle = -90.0f;
            //     }else {
            //         angle = 90.0f;
            //     }
            // }else if(zPosDifference < 0){
            //     angle = 180.0f - RADIANS_TO_DEGREES(matan__Fff(-zPosDifference, xPosDifference));
            // }else {
            //     angle = RADIANS_TO_DEGREES(matan__Fff(zPosDifference, xPosDifference));
            // }
            // OSReport("%f\n", angle);
            // // Shift angle into range 0 - 360
            // angle = fmodf__3stdFff(this, angle, 360.0f);
            // if (angle < 0)
            //     angle += 360.0f;
            //
            // float wrappedAngle = MsWrap(mParent->mRotation.y, angle - 180.0f, angle + 180.0f);
            // angle = angle - wrappedAngle;
            //
            // OSReport("wrapped %f\n", wrappedAngle);
            // OSReport("final %f\n", angle);
            // if (fabs(angle) < 0.5f * params->mSLDamageAngle.get())
            {
                mParent->_03[0]++;

                if (mParent->_03[2] < params->mSLWaterMarkLimit.get()) {
                    mParent->_03[2]++;
                }
                mParent->_03[1] = params->mSLWaterHitTimer.get();
                // Replace with custom nerve
                if (mParent->mSpineBase->getLatestNerve() != &swallow) {
                    mParent->mSpineBase->setNerve(&swallow);
                    return true;
                }
            }
        }
    }
    return true;

}

void dropLava(TSpineBase<TLiveActor> *spine, TFireyPetey *target) {
    if (spine->mNerveTimer % 25 == 10) {

        TKukkuBall *mKukkuBall = nullptr;
        for (int i = 0; i < NUM_GOOP_DROPS; i++) {
            TKukkuBall *tmKukkuBall = target->mKukkuBall[i];
            bool bVar1 = false;
            if (((tmKukkuBall->unk1 & 1) != 0) && (tmKukkuBall->unk2 == 0)) {
                mKukkuBall = tmKukkuBall;
                JGeometry::TVec3<f32> step(0.000f, 1.500f, 0.000f);

                float x =
                    (static_cast<float>(rand()) / static_cast<float>(0x7fff)) - 0.5f;
                float y =
                    (static_cast<float>(rand()) / static_cast<float>(0x7fff)) - 0.5f;
                float z =
                    (static_cast<float>(rand()) / static_cast<float>(0x7fff)) - 0.5f;

                float mag = sqrtf(x * x + y * y + z * z);
                x /= mag;
                y /= mag;
                z /= mag;

                float r =
                    (static_cast<float>(rand()) / static_cast<float>(0x7fff)) * 2000.0f;
                const JGeometry::TVec3<f32> velRandom(x, y, z);
                const JGeometry::TVec3<f32> random(x * r, y * r, z * r);

                mKukkuBall->mTranslation.set(target->mTranslation);
                mKukkuBall->mTranslation.add(random);
                step.add(velRandom);
                mKukkuBall->mVelocity.set(step);
                mKukkuBall->mObjectType = mKukkuBall->mObjectType & 0xfffffffe;
                mKukkuBall->unk1 = mKukkuBall->unk1 & 0xfffffffe;
                break;
            }
        }
    }

    if (spine->mNerveTimer % 50 == 25) {
        TBPPolDrop *poldrop = target->mPollutionDrop;
        if (poldrop->_01[1] == 0) {
            TVec3f step(0.0f, 1.5f, 0.0f);

            TVec3f rotation(0.0f, 0.0f, 0.0f);
            TVec3f size(0.7f, 0.7f, 0.7f);

            poldrop->mVelocity.set(step);
            poldrop->mTranslation.set(target->mTranslation);
            poldrop->mScale.set(size);
            poldrop->mRotation.set(rotation);
            poldrop->_01[1] = 1;
            poldrop->_01[2] = 0;
            poldrop->mActorData->setBck("pollut_ball");
            poldrop->mActorData->setBtk("pollut_ball_01");
            poldrop->mActorData->setBtk("pollut_ball_02");
            poldrop->_02 = target->mTranslation.y;
        }
    }
}


//// Manager code
TFireyPeteyManager::TFireyPeteyManager(const char *name, int isDemo)
    : TBossPakkunManager(name, isDemo) {
}

void TFireyPeteyManager::load(JSUMemoryInputStream &inputStream) {
    TBossPakkunManager::load(inputStream);
    gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_a.jpa", 0x1a3);
    gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_b.jpa", 0x1a4);
    gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_c.jpa", 0x1a5);
    gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_d.jpa", 0x1a6);
    gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_e.jpa", 0x1a7);

}
