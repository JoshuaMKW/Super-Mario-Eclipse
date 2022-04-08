#pragma once

#include "JDrama/JDRViewObj.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "J3D/J3DModel.hxx"
#include "sms/M3DUtil/MActor.hxx"

#include "JointObj.hxx"

class TJointModelManager;

class TJointModel : public TJointObj {
public:
  TJointModel();

  virtual void perform(u32, JDrama::TGraphics *);

  virtual void initJointModel();
  virtual u32 getJ3DModelFlag() const;

  TJointModelManager *mManager;
  J3DModelData *mModelData;
  J3DModel *mModel;
  MActor *mActor;
};