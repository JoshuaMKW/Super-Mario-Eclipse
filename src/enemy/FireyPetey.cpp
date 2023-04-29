#include <Dolphin/math.h>

#include <JSystem/JGadget/List.hxx>
#include <JSystem/JDrama/JDRNameRefGen.hxx>
#include <JSystem/JDrama/JDRNameRefPtrListT.hxx>
#include <JSystem/JParticle/JPAResourceManager.hxx>

#include <SMS/Player/Mario.hxx>
#include <SMS/MoveBG/Coin.hxx>
#include <SMS/Manager/PollutionManager.hxx>
#include <SMS/Strategic/ObjHitCheck.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/libs/constmath.hxx>

#include <enemy/firey_petey.hxx>

#include "Manager/MarioParticleManager.hxx"
#include "MSound/MSBGM.hxx"

using namespace BetterSMS;


// Globals
static TNerveFPSleep sleep;
static TNerveFPBreakSleep breakSleep;
static TNerveFPTakeOff takeoff;
// static TNerveFPFly fly;
// static TNerveFPWait wait;
// static TNerveFPHover hover;
static TNerveFPFireBreath fireBreath;

#define MAX_TORNADOS 3

TFireyPetey::TFireyPetey(const char *test) : TBossPakkun(test)
{
  numTornados = MAX_TORNADOS;
}

TFPTornado::TFPTornado(TBossPakkun *parent, const char *name) : TBPTornado::TBPTornado(parent, name)
{
}

TFPFire::TFPFire(TBossPakkun *parent, const char *name) : THitActor::THitActor(name)
{
  initHitActor(0x8000027, 1, -0x7f000000, 275.0f, 275.0f, 100.0f, 100.0f);
}

void TFPFire::perform(u32 flags, JDrama::TGraphics *graphics)
{
  THitActor::perform(flags, graphics);
  if (mLifetime > 0)
  {
    for (int iVar9 = 0; iVar9 < mNumObjs; iVar9 = iVar9 + 1)
    {
      if (mCollidingObjs[iVar9]->mObjectID == OBJECT_ID_MARIO)
      {
        // Player::setFire(gpMarioAddress);
      }
    }
    mTranslation.add(mVelocity);
    mLifetime--;
  }
}

void TFPTornado::perform(u32 flags, JDrama::TGraphics *graphics)
{
  if (((*(int **)(this->mParent))[0x3c] & 1U) != 0)
  {
    return;
  }
  if (this->_98 == 0)
  {
    return;
  }
  TBPTornado::perform(flags, graphics);
  mTargetPos = *gpMarioPos;
  for (int i = 0; i < mNumObjs; i = i + 1)
  {
      if (mCollidingObjs[i]->mObjectID == OBJECT_ID_MARIO) {
        // Player::setFire(gpMarioAddress);
    }
  }
  gpPollution->stamp(1, mTranslation.x, mTranslation.y, mTranslation.z, 384.0f);
}

TNerveFPSleep::~TNerveFPSleep() {}

TNerveFPBreakSleep::~TNerveFPBreakSleep() {}

TNerveFPTakeOff::~TNerveFPTakeOff() {}

TNerveFPFly::~TNerveFPFly() {}

TNerveFPHover::~TNerveFPHover() {}

TNerveFPFireBreath::~TNerveFPFireBreath() {}

bool TNerveFPWait::execute(TSpineBase<TLiveActor> *spine) const
{
  TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
  bool result = TNerveBPWait::execute(spine);
  // if (spine->mNerveCurrent == (TNerveBase<TLiveActor> *)&instance$3678)
  // {
  //   if (target->numTornados > 0)
  //   {
  //     if (spine->mNerveStack.depth() < spine->mNerveStack.capacity())
  //     {
  //       // spine->_01[0] = &wait;
  //       // spine->_01[1] = (TNerveBase<TLiveActor> *)NerveGetByIndex__Fi(0x1d);
  //     }
  //     target->numTornados--;
  //   }
  //   else
  //   {
  //     if (spine->mNerveStack.depth() < spine->mNerveStack.capacity())
  //     {
  //         spine->setNerve(&takeoff);
  //     }
  //     target->numTornados = MAX_TORNADOS;
  //   }
  // }
  return result;
}

bool TNerveFPFly::execute(TSpineBase<TLiveActor> *spine) const
{
  // TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
  // if (spine->mNerveTimer % 25 == 10)
  // {
  //
  //   TKukkuBall *mKukkuBall = nullptr;
  //   for (int i = 0; i < NUM_GOOP_DROPS; i++)
  //   {
  //     TKukkuBall *tmKukkuBall = target->mKukkuBall[i];
  //     bool bVar1 = false;
  //     if (((tmKukkuBall->unk1 & 1) != 0) && (tmKukkuBall->unk2 == 0))
  //     {
  //       mKukkuBall = tmKukkuBall;
  //       TVec3f step(0.0f, 1.5f, 0.0f);
  //
  //       mKukkuBall->mTranslation.set(target->mTranslation);
  //       // mKukkuBall->mTranslation.add(Rand::randVector(2000.0f));
  //       // step.add(Rand::randUnitVector());
  //       mKukkuBall->mVelocity.set(step);
  //       mKukkuBall->mObjectType = mKukkuBall->mObjectType & 0xfffffffe;
  //       mKukkuBall->unk1 = mKukkuBall->unk1 & 0xfffffffe;
  //       break;
  //     }
  //   }
  // }
  //
  // if (spine->mNerveTimer % 50 == 25)
  // {
  //   TBPPolDrop *poldrop = target->mPollutionDrop;
  //   if (poldrop->_01[1] == 0)
  //   {
  //     TVec3f step(0.0f, 1.5f, 0.0f);
  //
  //     TVec3f rotation(0.0f, 0.0f, 0.0f);
  //     TVec3f size(0.7f, 0.7f, 0.7f);
  //
  //     poldrop->mVelocity.set(step);
  //     poldrop->mTranslation.set(target->mTranslation);
  //     poldrop->mScale.set(size);
  //     poldrop->mRotation.set(rotation);
  //     poldrop->_01[1] = 1;
  //     poldrop->_01[2] = 0;
  //     poldrop->mActorData->setBck("pollut_ball");
  //     poldrop->mActorData->setBtk("pollut_ball_01");
  //     poldrop->mActorData->setBtk("pollut_ball_02");
  //     poldrop->_02 = target->mTranslation.y;
  //   }
  // }
  // return TNerveBPFly::execute(spine);
    return 0;
}

bool TNerveFPTakeOff::execute(TSpineBase<TLiveActor> *spine) const
{
  // TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
  // TVec3f mTranslation;
  //
  // return TNerveBPTakeOff::execute(spine);
    return 0;
}

bool TNerveFPBreakSleep::execute(TSpineBase<TLiveActor> *spine) const
{
  // TBossPakkun *target = reinterpret_cast<TBossPakkun *>(spine->mTarget);
  // if (spine->mNerveTimer == 0)
  // {
  //   target->changeBck(0x0E);
  //   MSBgm::stopTrackBGMs('\a', 10);
  // }
  // if (!target->mActorData->curAnmEndsNext(0, nullptr))
  // {
  //   return false;
  // }
  // if (spine->mNerveStack.depth() < spine->mNerveStack.capacity())
  // {
  //     spine->pushNerve(&takeoff);
  // }
  return true;
}

bool TNerveFPSleep::execute(TSpineBase<TLiveActor> *param1) const
{
  // if (param1->mNerveTimer == 0)
  // {
  //   reinterpret_cast<TBossPakkun *>(param1->mTarget)->changeBck(0x17);
  // }
  return 0;
}

bool TNerveFPHover::execute(TSpineBase<TLiveActor> *spine) const
{
  // if (spine->mNerveTimer % 15 == 10)
  // {
  //   TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
  //
  //   TKukkuBall *mKukkuBall = nullptr;
  //   for (int i = 0; i < NUM_GOOP_DROPS; i++)
  //   {
  //     TKukkuBall *tmKukkuBall = target->mKukkuBall[i];
  //     bool bVar1 = false;
  //     if (((tmKukkuBall->unk1 & 1) != 0) && (tmKukkuBall->unk2 == 0))
  //     {
  //       mKukkuBall = tmKukkuBall;
  //       TVec3f step(0.0f, 1.5f, 0.0f);
  //
  //       mKukkuBall->mTranslation.set(target->mTranslation);
  //       // mKukkuBall->mTranslation.add(Util::Rand::randVector(2000.0f));
  //       // step.add(Util::Rand::randUnitVector());
  //       mKukkuBall->mVelocity.set(step);
  //       mKukkuBall->mObjectType = mKukkuBall->mObjectType & 0xfffffffe;
  //       mKukkuBall->unk1 = mKukkuBall->unk1 & 0xfffffffe;
  //       break;
  //     }
  //   }
  // }
  // return TNerveBPHover::execute(spine);
    return 0;
}

bool TNerveFPFireBreath::execute(TSpineBase<TLiveActor> *spine) const
{

  // TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
  // MActor *peteyMActor = target->mActorData;
  // if (spine->mNerveTimer == 0x0)
  // {
  //   target->changeBck(0x15);
  //
  //   // Set positions of collision
  //   for (int i = 0; i < 10; i++)
  //   {
  //     target->mFire[i]->mTranslation.set(target->mTranslation);
  //     target->mFire[i]->mVelocity.x = 0.0f;
  //     target->mFire[i]->mVelocity.y = 0.0f;
  //     target->mFire[i]->mVelocity.z = 0.0f;
  //     target->mFire[i]->mLifetime = 0;
  //   }
  //   peteyMActor->setFrameRate(SMSGetAnmFrameRate__Fv() * 2.0f, 0);
  // }
  // bool isAnimationRunning = peteyMActor->checkCurAnmFromIndex(0x15, 0);
  // if (isAnimationRunning)
  // {
  //   J3DFrameCtrl *frameCtrl = peteyMActor->getFrameCtrl(0);
  //
  //   if ((frameCtrl->mCurFrame <= 25.0f) || (165.0 <= frameCtrl->mCurFrame))
  //   {
  //     target->_02 = 0x0;
  //   }
  //   else
  //   {
  //     target->_02 = 0x2;
  //   }
  // }
  // bool isBckRunning = peteyMActor->checkCurBckFromIndex(0x14);
  // if (isBckRunning)
  // {
  //   peteyMActor->setFrameRate(SMSGetAnmFrameRate__Fv(), 0);
  //   isAnimationRunning = peteyMActor->checkCurAnmFromIndex(0x14, 0);
  //   if (isAnimationRunning)
  //   {
  //     J3DFrameCtrl *frameCtrl = peteyMActor->getFrameCtrl(0);
  //
  //     // Joint 0xC
  //     if (frameCtrl->mCurFrame >= 15.0f && frameCtrl->mCurFrame <= 46.0f)
  //     {
  //       if (spine->mNerveTimer % 10 == 0)
  //       {
  //         JPABaseEmitter *emitterFire = gpMarioParticleManager->emitAndBindToMtxPtr(0x1a7, target->mActorData->mModel->mJointArray[0xC], 1, target);
  //
  //         for (int i = 0x1a6; i > 0x1a2; i--)
  //         {
  //           gpMarioParticleManager->emitAndBindToMtxPtr(i, target->mActorData->mModel->mJointArray[0xC], 1, target);
  //         }
  //         TFPFire *mFire;
  //         bool found = false;
  //         for (int i = 0; i < 10; i++)
  //         {
  //           if (target->mFire[i]->mLifetime == 0)
  //           {
  //             mFire = target->mFire[i];
  //             found = true;
  //             break;
  //           }
  //         }
  //         if (found)
  //         {
  //           mFire->mVelocity.x = 50.0f * sinf(angleToRadians(target->mRotation.y));
  //           mFire->mVelocity.y = 0;
  //           mFire->mVelocity.z = 50.0f * cosf(angleToRadians(target->mRotation.y));
  //           mFire->mTranslation.set(*reinterpret_cast<TVec3f *>(&emitterFire->_00[84]));
  //           mFire->mTranslation.y -= 237.5f;
  //           mFire->mLifetime = 30 * 4;
  //         }
  //       }
  //     }
  //   }
  // }
  // bool isAnimationFinished = peteyMActor->curAnmEndsNext(0, 0x0);
  // if (isAnimationFinished)
  // {
  //   bool isBckRunning = peteyMActor->checkCurBckFromIndex(0x15);
  //   if (isBckRunning)
  //   {
  //       spine->pushNerve(&takeoff);
  //       return 1;
  //   }
  //   target->_02 = 0x0;
  //   target->changeBck(0x14);
  //   target->rumblePad(1, target->mTranslation);
  // }
  return 0;
}

void TFireyPetey::init(TLiveManager *param1)
{
  // TBossPakkun::init(param1);
  // TBossPakkun *target = reinterpret_cast<TBossPakkun *>(this);
  // target->mTornado = new TFPTornado(this, "<TFPTornado>\n");
  //
  // JDrama::TNameRefGen *instance = JDrama::TNameRefGen::getInstance();
  // JDrama::TNameRef *root = instance->getRootNameRef();
  //
  // u16 keyCode = calcKeyCode(reinterpret_cast<const char*>(
  //     gUnkManagerName));
  //
  // TIdxGroupObj *group = static_cast<TIdxGroupObj*>(root->searchF(keyCode, reinterpret_cast<const char*>(
  //     gUnkManagerName)));
  //
  // group->mViewObjList.insert(group->mViewObjList.end(), mTornado);
  //
  // mSpineBase->setNerve(&sleep);
  //
  //
  // mMActorKeeperSecondary = new TMActorKeeper(mLiveManager, NUM_GOOP_DROPS);
  //
  // for (int i = 0; i < NUM_GOOP_DROPS; i++)
  // {
  //   mKukkuBall[i] = new TKukkuBall("lavaDrop\n");
  //   mKukkuBall[i]->mActorData = mMActorKeeperSecondary->createMActor("torifun.bmd", 3);
  //   mKukkuBall[i]->init();
  // }
  //
  // for (int i = 0; i < 10; i++)
  // {
  //   mFire[i] = new TFPFire(this, "fire\n");
  //   group->mViewObjList.insert(group->mViewObjList.end(), mFire[i]);
  // }
}

void TFireyPetey::perform(u32 flags, JDrama::TGraphics *graphics)
{
  // TBossPakkun::perform(flags, graphics);
  // for (int i = 0; i < NUM_GOOP_DROPS; i++)
  // {
  //   if (mKukkuBall[i] != nullptr)
  //   {
  //     mKukkuBall[i]->perform(flags, graphics);
  //   }
  // }
  // for (int i = 0; i < 10; i++)
  // {
  //   if (mFire[i] != nullptr)
  //   {
  //     mFire[i]->perform(flags, graphics);
  //   }
  // }
}

bool TFireyPetey::receiveMessage(THitActor *reciever, u32 param2)
{
  // if (reciever->mObjectID == 0x1000001 && mSpineBase->getLatestNerve() == &sleep)
  // {
  //     mSpineBase->setNerve(&breakSleep);
  //     return false;
  // }else if(_02 == 3 && (reciever->mObjectID == 0x1000001) || (reciever->mObjectID == 0x100000d)) {
  //     if(mTranslation.y - 300.0f <= reciever->mTranslation.y) {
  //         if (reciever->mTranslation.y <= mTranslation.y + 1500.0f) {
  //             _02 = 0;
  //             mSpineBase->setNerve(&fireBreath);
  //             return true;
  //         }
  //     }
  // }
  // return TBossPakkun::receiveMessage(reciever, param2);
    return 0;
}


TFireyPetey::~TFireyPetey()
{
}

//// Manager code
TFireyPeteyManager::TFireyPeteyManager(char const *name, int isDemo) : TBossPakkunManager(name, isDemo)
{
}

void TFireyPeteyManager::load(JSUMemoryInputStream &inputStream)
{
  TBossPakkunManager::load(inputStream);
  gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_a.jpa", 0x1a3);
  gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_b.jpa", 0x1a4);
  gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_c.jpa", 0x1a5);
  gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_d.jpa", 0x1a6);
  gpResourceManager->load("/scene/bosspakkun/jpa/ms_kp_fire_e.jpa", 0x1a7);

}