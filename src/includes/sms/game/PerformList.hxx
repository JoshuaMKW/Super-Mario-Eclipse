#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JGadget/List.hxx"
#include "JSU/JSUMemoryStream.hxx"

class TPerformList : public JDrama::TViewObj,
                     public JGadget::TSingleNodeLinkList {
public:
  virtual ~TPerformList();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *);

  void push_back(JDrama::TViewObj *, u32);
  void push_back(char *, u32);
};