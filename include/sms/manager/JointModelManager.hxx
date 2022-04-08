#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "sms/map/JointModel.hxx"
#include "sms/M3DUtil/MActor.hxx"

class TJointModelManager : public JDrama::TViewObj {
public:
  TJointModelManager(const char *);
  virtual ~TJointModelManager();

  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual TJointModel *newJointModel() const;

  void initJointModel();

  size_t mJointModelNum;
  TJointModel **mJointModels;
  u32 _18;
  const char *mJointModelsPath;
  MActorAnmData *mAnmData;
};