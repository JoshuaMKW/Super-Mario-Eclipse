#include "enemy/FireyPetey.hxx"
#include "sms/actor/item/Coin.hxx"
#include "sms/rand.h"
#include "math.h"
#include "sme/libs/sMath.hxx"
#include "SME.hxx"
#include "JGadget/List.hxx"
#include "JDrama/JDRNameRefGen.hxx"
#include "JDrama/JDRNameRefPtrListT.hxx"
#include "ppc_intrinsics.h"
#include "sms/game/IdxGroup.hxx"
#include "sms/sound/MSoundSESystem.hxx"
#include "sme/libs/sRand.hxx"

using namespace SME;
using namespace SME::Util;

// BPFly
extern int instance$3889;
extern bool init$3891;

// BPWait
extern int instance$3625;
extern bool init$3627;

// BPHover
extern int instance$3940;
extern bool init$3942;

// BPVomit
extern int instance$3678;
extern bool init$3680;

// Globals
static TNerveFPSleep sleep;
static TNerveFPBreakSleep breakSleep;
static TNerveFPTakeOff takeoff;
static TNerveFPFly fly;
static TNerveFPWait wait;
static TNerveFPHover hover;
static TNerveFPFireBreath fireBreath;

extern const char peteyIdxGroupName[];

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
      if (mCollidingObjs[iVar9]->mObjectID == -0x7fffffff)
      {
        Mario::setFireToPlayer(gpMarioAddress);
      }
    }
    mPosition.add(mVelocity);
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
    if (mCollidingObjs[i]->mObjectID == -0x7fffffff)
    {
      Mario::setFireToPlayer(gpMarioAddress);
    }
  }
  stamp__17TPollutionManagerFUsffff(gpPollution, 1, mPosition.x, mPosition.y,
                                    mPosition.z, 384.0f);
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
  if (spine->_01[1] == (TNerveBase<TLiveActor> *)&instance$3678)
  {
    if (target->numTornados > 0)
    {
      if (spine->mVTableIndex < spine->_00)
      {
        spine->_01[0] = &wait;
        spine->_01[1] = (TNerveBase<TLiveActor> *)NerveGetByIndex__Fi(0x1d);
      }
      target->numTornados--;
    }
    else
    {
      if (spine->mVTableIndex < spine->_00)
      {
        spine->_01[0] = &takeoff;
      }
      target->numTornados = MAX_TORNADOS;
    }
  }
  return result;
}

bool TNerveFPFly::execute(TSpineBase<TLiveActor> *spine) const
{
  TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
  if (spine->mStateTimer % 25 == 10)
  {

    TKukkuBall *mKukkuBall = nullptr;
    for (int i = 0; i < NUM_GOOP_DROPS; i++)
    {
      TKukkuBall *tmKukkuBall = target->mKukkuBall[i];
      bool bVar1 = false;
      if (((tmKukkuBall->unk1 & 1) != 0) && (tmKukkuBall->unk2 == 0))
      {
        mKukkuBall = tmKukkuBall;
        TVec3f step(0.0f, 1.5f, 0.0f);

        mKukkuBall->mPosition.set(target->mPosition);
        mKukkuBall->mPosition.add(Rand::randVector(2000.0f));
        step.add(Rand::randUnitVector());
        mKukkuBall->mVelocity.set(step);
        mKukkuBall->mObjectType = mKukkuBall->mObjectType & 0xfffffffe;
        mKukkuBall->unk1 = mKukkuBall->unk1 & 0xfffffffe;
        break;
      }
    }
  }

  if (spine->mStateTimer % 50 == 25)
  {
    TBPPolDrop *poldrop = target->mPollutionDrop;
    if (poldrop->_01[1] == 0)
    {
      TVec3f step(0.0f, 1.5f, 0.0f);

      TVec3f rotation(0.0f, 0.0f, 0.0f);
      TVec3f size(0.7f, 0.7f, 0.7f);

      poldrop->mVelocity.set(step);
      poldrop->mPosition.set(target->mPosition);
      poldrop->mSize.set(size);
      poldrop->mRotation.set(rotation);
      poldrop->_01[1] = 1;
      poldrop->_01[2] = 0;
      poldrop->mActorData->setBck("pollut_ball");
      poldrop->mActorData->setBtk("pollut_ball_01");
      poldrop->mActorData->setBtk("pollut_ball_02");
      poldrop->_02 = target->mPosition.y;
    }
  }
  return TNerveBPFly::execute(spine);
}

bool TNerveFPTakeOff::execute(TSpineBase<TLiveActor> *spine) const
{
  TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
  TVec3f mPosition;

  return TNerveBPTakeOff::execute(spine);
}

bool TNerveFPBreakSleep::execute(TSpineBase<TLiveActor> *spine) const
{
  TBossPakkun *target = reinterpret_cast<TBossPakkun *>(spine->mTarget);
  if (spine->mStateTimer == 0)
  {
    target->changeBck(0x0E);
    MSBgm::stopTrackBGMs('\a', 10);
  }
  if (!target->mActorData->curAnmEndsNext(0, 0))
  {
    return false;
  }
  if (spine->mVTableIndex < spine->_00)
  {
    spine->_01[spine->mVTableIndex++] = &takeoff;
  }
  return true;
}

bool TNerveFPSleep::execute(TSpineBase<TLiveActor> *param1) const
{
  if (param1->mStateTimer == 0)
  {
    reinterpret_cast<TBossPakkun *>(param1->mTarget)->changeBck(0x17);
  }
  return 0;
}

bool TNerveFPHover::execute(TSpineBase<TLiveActor> *spine) const
{
  if (spine->mStateTimer % 15 == 10)
  {
    TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);

    TKukkuBall *mKukkuBall = nullptr;
    for (int i = 0; i < NUM_GOOP_DROPS; i++)
    {
      TKukkuBall *tmKukkuBall = target->mKukkuBall[i];
      bool bVar1 = false;
      if (((tmKukkuBall->unk1 & 1) != 0) && (tmKukkuBall->unk2 == 0))
      {
        mKukkuBall = tmKukkuBall;
        TVec3f step(0.0f, 1.5f, 0.0f);

        mKukkuBall->mPosition.set(target->mPosition);
        mKukkuBall->mPosition.add(Util::Rand::randVector(2000.0f));
        step.add(Util::Rand::randUnitVector());
        mKukkuBall->mVelocity.set(step);
        mKukkuBall->mObjectType = mKukkuBall->mObjectType & 0xfffffffe;
        mKukkuBall->unk1 = mKukkuBall->unk1 & 0xfffffffe;
        break;
      }
    }
  }
  return TNerveBPHover::execute(spine);
}

bool TNerveFPFireBreath::execute(TSpineBase<TLiveActor> *spine) const
{

  TFireyPetey *target = reinterpret_cast<TFireyPetey *>(spine->mTarget);
  MActor *peteyMActor = target->mActorData;
  if (spine->mStateTimer == 0x0)
  {
    target->changeBck(0x15);

    // Set positions of collision
    for (int i = 0; i < 10; i++)
    {
      target->mFire[i]->mPosition.set(target->mPosition);
      target->mFire[i]->mVelocity.x = 0.0f;
      target->mFire[i]->mVelocity.y = 0.0f;
      target->mFire[i]->mVelocity.z = 0.0f;
      target->mFire[i]->mLifetime = 0;
    }
    peteyMActor->setFrameRate(SMSGetAnmFrameRate__Fv() * 2.0f, 0);
  }
  bool isAnimationRunning = peteyMActor->checkCurAnmFromIndex(0x15, 0);
  if (isAnimationRunning)
  {
    J3DFrameCtrl *frameCtrl = peteyMActor->getFrameCtrl(0);

    if ((frameCtrl->mCurFrame <= 25.0f) || (165.0 <= frameCtrl->mCurFrame))
    {
      target->_02 = 0x0;
    }
    else
    {
      target->_02 = 0x2;
    }
  }
  bool isBckRunning = peteyMActor->checkCurBckFromIndex(0x14);
  if (isBckRunning)
  {
    peteyMActor->setFrameRate(SMSGetAnmFrameRate__Fv(), 0);
    isAnimationRunning = peteyMActor->checkCurAnmFromIndex(0x14, 0);
    if (isAnimationRunning)
    {
      J3DFrameCtrl *frameCtrl = peteyMActor->getFrameCtrl(0);

      // Joint 0xC

      //*(u32*)0x800003e0
      if (frameCtrl->mCurFrame >= 15.0f && frameCtrl->mCurFrame <= 46.0f)
      {
        if (spine->mStateTimer % 10 == 0)
        {
          JPABaseEmitter *emitterFire = gpMarioParticleManager->emitAndBindToMtxPtr(0x1a7, target->mActorData->mModel->mJointArray[0xC], 1, target);

          for (int i = 0x1a6; i > 0x1a2; i--)
          {
            gpMarioParticleManager->emitAndBindToMtxPtr(i, target->mActorData->mModel->mJointArray[0xC], 1, target);
          }
          TFPFire *mFire;
          bool found = false;
          for (int i = 0; i < 10; i++)
          {
            if (target->mFire[i]->mLifetime == 0)
            {
              mFire = target->mFire[i];
              found = true;
              break;
            }
          }
          if (found)
          {
            mFire->mVelocity.x = 50.0f * sinf(Math::angleToRadians(target->mRotation.y));
            mFire->mVelocity.y = 0;
            mFire->mVelocity.z = 50.0f * cosf(Math::angleToRadians(target->mRotation.y));
            mFire->mPosition.set(*reinterpret_cast<TVec3f *>(&emitterFire->_00[84]));
            mFire->mPosition.y -= 237.5f;
            mFire->mLifetime = 30 * 4;
          }
        }
      }
    }
  }
  bool isAnimationFinished = peteyMActor->curAnmEndsNext(0, 0x0);
  if (isAnimationFinished)
  {
    bool isBckRunning = peteyMActor->checkCurBckFromIndex(0x15);
    if (isBckRunning)
    {
      spine->_01[spine->mVTableIndex++] = &takeoff;
      return 1;
    }
    target->_02 = 0x0;
    target->changeBck(0x14);
    target->rumblePad(1, target->mPosition);
  }
  return 0;
}

void TFireyPetey::init(TLiveManager *param1)
{
  TBossPakkun::init(param1);
  TBossPakkun *target = reinterpret_cast<TBossPakkun *>(this);
  target->mTornado = new TFPTornado(this, "<TFPTornado>\n");

  JDrama::TNameRefGen *instance = JDrama::TNameRefGen::getInstance();
  JDrama::TNameRef *root = instance->getRootNameRef();

  u16 keyCode = calcKeyCode(peteyIdxGroupName);

  IdxGroup *group = static_cast<IdxGroup *>(root->searchF(keyCode, peteyIdxGroupName));

  group->hitActorList.mEnd->mItem = mTornado;

  mSpineBase->mVTableIndex = 0;
  mSpineBase->mStateTimer = 0;
  mSpineBase->mCurVTable = &sleep;
  mSpineBase->mPrevVTable = 0;

  // Will need to swap to redirection
  instance$3889 = *(int *)&fly;
  init$3891 = true;

  instance$3625 = *(int *)&wait;
  init$3627 = true;

  instance$3940 = *(int *)&hover;
  init$3942 = true;

  instance$3678 = *(int *)&fireBreath;
  init$3680 = true;

  mMActorKeeperSecondary = new TMActorKeeper(mLiveManager, NUM_GOOP_DROPS);

  for (int i = 0; i < NUM_GOOP_DROPS; i++)
  {
    mKukkuBall[i] = new TKukkuBall("lavaDrop\n");
    mKukkuBall[i]->mActorData = mMActorKeeperSecondary->createMActor("torifun.bmd", 3);
    mKukkuBall[i]->init();
  }

  for (int i = 0; i < 10; i++)
  {
    mFire[i] = new TFPFire(this, "fire\n");
    group->hitActorList.insert(group->hitActorList.end(), mFire[i]);
  }
}

void TFireyPetey::perform(u32 flags, JDrama::TGraphics *graphics)
{
  TBossPakkun::perform(flags, graphics);
  for (int i = 0; i < NUM_GOOP_DROPS; i++)
  {
    if (mKukkuBall[i] != nullptr)
    {
      mKukkuBall[i]->perform(flags, graphics);
    }
  }
  for (int i = 0; i < 10; i++)
  {
    if (mFire[i] != nullptr)
    {
      mFire[i]->perform(flags, graphics);
    }
  }
}

bool TFireyPetey::receiveMessage(THitActor *reciever, u32 param2)
{
  if (reciever->mObjectID == 0x1000001 && mSpineBase->mCurVTable == &sleep)
  {
    mSpineBase->mPrevVTable = mSpineBase->mCurVTable;
    mSpineBase->mStateTimer = 0;
    mSpineBase->mCurVTable = &breakSleep;
    return false;
  }
  return TBossPakkun::receiveMessage(reciever, param2);
}

// Because Screw hardcoded behaviours
SME_WRITE_32(SME_PORT_REGION(0x80095C54, 0x8008F2F4, 0x0, 0x0), 0x60000000);

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

  load__18JPAResourceManagerFPCcUs(gpResourceManager, "/scene/bosspakkun/jpa/ms_kp_fire_a.jpa", 0x1a3);
  load__18JPAResourceManagerFPCcUs(gpResourceManager, "/scene/bosspakkun/jpa/ms_kp_fire_b.jpa", 0x1a4);
  load__18JPAResourceManagerFPCcUs(gpResourceManager, "/scene/bosspakkun/jpa/ms_kp_fire_c.jpa", 0x1a5);
  load__18JPAResourceManagerFPCcUs(gpResourceManager, "/scene/bosspakkun/jpa/ms_kp_fire_d.jpa", 0x1a6);
  load__18JPAResourceManagerFPCcUs(gpResourceManager, "/scene/bosspakkun/jpa/ms_kp_fire_e.jpa", 0x1a7);
}