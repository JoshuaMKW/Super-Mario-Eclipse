#pragma once

#include "J3D/J3DJoint.hxx"
#include "J3D/J3DModel.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "sms/actor/MActor.hxx"
#include "types.h"


class TJointObj {

public:
  u32 *vTable;
  u32 _4;
  u32 _8;
  J3DJoint *joint;         // _C
  u32 numChildJoints;      // _10
  TJointObj **childJoints; // _14 (array of child joints, non-fixed size)
  u32 _18;
  u32 _1C;
};

class TJointModelManager : public JDrama::TViewObj {

public:
  u32 _10;
  u32 _14;
  u32 _18;
  char *modelName;       // _1C
  MActorAnmData anmData; // _20
};

class TJointModel : public TJointObj {

public:
  TJointModelManager *mModelManager; // _20
  J3DModelData *mModelData;          // _24
  J3DModel *mModel;                  // _28
  MActor *mActor;                    // _2C
};