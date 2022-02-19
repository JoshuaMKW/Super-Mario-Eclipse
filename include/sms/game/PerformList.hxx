#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JGadget/List.hxx"
#include "JSU/JSUMemoryStream.hxx"

#define PERFORM_ON_MOVEMENT 0x00000001
#define PERFORM_ON_GX 0x00000008
// #define PERFORM_ON_SILHOUETTE
// #define PERFORM_ON_CALCANIM
// #define PERFORM_ON_UIELEMENTS
// #define PERFORM_ON_SHINEMOVE
// #define PERFORM_ON_SHINEANIM

class TPerformList : public JDrama::TViewObj,
                     public JGadget::TSingleNodeLinkList {
public:
  virtual ~TPerformList();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;

  void push_back(JDrama::TViewObj *, u32);
  void push_back(char *, u32);
};