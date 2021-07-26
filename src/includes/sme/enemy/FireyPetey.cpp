#include "FireyPetey.hxx"

#include "SME.hxx"


//Globals
TNerveFPSleep *drop;


TFireyPetey::TFireyPetey(const char* test): TBossPakkun(test){
    // SME_DEBUG_LOG("Init\n");
}

TNerveFPSleep::~TNerveFPSleep(){

}


bool TNerveFPBreakSleep::execute(TSpineBase<TLiveActor> *param1){

  
  // int iVar1;
  // undefined4 uVar2;
  // TBossPakkun *this;
  
  // if (param1[8] == (TBossPakkun *)0x0) {
  //   ::TBossPakkun::changeBck(this,0xe);
  //   MSBgm::stopTrackBGMs('\a',10);
  // }
  // iVar1 = MActor::curAnmEndsNext(*(MActor **)(this + 0x74),0,(char *)0x0);
  // if (iVar1 == 0) {
  //   uVar2 = 0;
  // }
  // else {
  //   if (init$3873 == '\0') {
  //     instance$3871 = &TNerveBPTakeOff::__vt;
  //     Runtime.PPCEABI.H::__register_global_object
  //               (&instance$3871,::TNerveBPTakeOff::~TNerveBPTakeOff,(undefined4 *)&@3872);
  //     init$3873 = '\x01';
  //   }
  //   if ((int)param_2[2] < (int)param_2[1]) {
  //     *(undefined ***)(param_2[3] + (int)param_2[2] * 4) = &instance$3871;
  //     param_2[2] = param_2[2] + 1;
  //   }
  //   uVar2 = 1;
  // }
  return false;
}

bool TNerveFPSleep::execute(TSpineBase<TLiveActor> *param1){
    static_cast<TBossPakkun*>(param1->mTarget)->changeBck(0x17);
    return 0;
}

TNerveFPSleep::TNerveFPSleep(){

}

void TFireyPetey::init(TLiveManager *param1) {
    TBossPakkun::init(param1);
    // Nani?
    drop = new TNerveFPSleep();
    SME_DEBUG_LOG("flydrop\n");
    mSpineBase->mVTableIndex = 0;
    mSpineBase->mStateTimer = 0;
    mSpineBase->mCurVTable  = (void*)drop;
    mSpineBase->mPrevVTable = (void*)drop;
    mSpineBase->mPrevVTable = 0;
}

bool TFireyPetey::receiveMessage(THitActor *reciever,u32 param2) {
    if(mObjectID == 0x0&&mSpineBase->mCurVTable==drop){
        SME_DEBUG_LOG("POP\n");
        void* breaker = NerveGetByIndex__Fi(0x31);
        mSpineBase->mVTableIndex = 0;
        mSpineBase->mStateTimer = 0;
        mSpineBase->mCurVTable  = (void*)breaker;
        mSpineBase->mPrevVTable = (void*)breaker;
        mSpineBase->mPrevVTable = 0;
    }
    SME_DEBUG_LOG("0x%08x\n",*((int *)this + 0x4c));
    return TBossPakkun::receiveMessage(reciever,param2);
}

TFireyPetey::~TFireyPetey()
{
    
}

