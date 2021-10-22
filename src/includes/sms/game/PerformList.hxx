#pragma once

#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JGadget.hxx"

class TPerformList : public JDrama::TViewObj,
                     public JGadget::TSingleNodeLinkList {
public:
  virtual ~TPerformList();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *);

  void push_back(JDrama::TViewObj *, u32);
  void push_back(char *, u32);
};