#pragma once

#include "sms/spc/SpcBinary.hxx"
#include "sms/spc/SpcSlice.hxx"
#include "sms/spc/SpcStack.hxx"
#include "types.h"


class TSpcInterp {
public:
  //TSpcBinary *mSpcBinary; // _0
  u32 _4;
  u32 _8;
  u32 _C;
  u32 _10;
  u32 _14;
  u32 mSlicesMax; // _18
  u32 mSlicesCount;
  TSpcSlice *mSlices; // _20 (array of TSpcSlice*)
  u32 _24;
  u32 _28;
  u32 _2C;
  u32 _30;
  u32 _34;
  TSpcStack<u32> mStack;
  u32 _48;
  u32 _4C;
  u32 _50;
  u32 _54;
  char *mName;
  
  TSpcInterp(TSpcBinary *, void *, int, int, int, int);
  virtual ~TSpcInterp();

  virtual void update();
  virtual void dispatchBuiltin(u32, u32);

  void verifyArgNum(u32 argc, u32 *numArgs);
  void dump();
  void dispatchBuiltinDefault(u32, u32);

  /* SPC EXECUTE FUNCTIONS */
  void execadd();
  void execadr();
  void execand();
  void execass();
  void execband();
  void execbor();
  void execcall();
  void execdec();
  void execdiv();
  void execend();
  void execeq();
  void execflt();
  void execfunc();
  void execge();
  void execgt();
  void execinc();
  void execint();
  void execint0();
  void execint1();
  void execjmp();
  void execjne();
  void execle();
  void execlt();
  void execmkds();
  void execmkfr();
  void execmod();
  void execmul();
  void execne();
  void execneg();
  void execnop();
  void execnot();
  void execor();
  void execpop();
  void execret();
  void execret0();
  void execshl();
  void execshr();
  void execstr();
  void execsub();
  void execvar();

  /* FETCHERS */
  s32 fetchS32();
  u32 fetchU32();

  /* BUILTINS */
  void spcDump(u32);
  void spcExit(u32);
  void spcFloat(u32);
  void spcInt(u32);
  void spcLock(u32);
  void spcPrint(u32);
  void spcTypeof(u32);
  void spcUnlock(u32);
  void spcYield(u32);
};

void SpcTrace(const char *, ...);