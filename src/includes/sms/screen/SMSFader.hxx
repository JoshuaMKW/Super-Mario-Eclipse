#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRRect.hxx"
#include "JUT/JUTColor.hxx"

class TSMSFader : public JDrama::TNameRef {
public:
  enum EFadeStatus { FADE_ON, FADE_OFF, FADE_IN, FADE_OUT };

  class WipeRequest {
  public:
    enum ERequest {
      FADE_NONE,
      FADE_CIRCLE_IN,
      FADE_CIRCLE_OUT,
      FADE_DIAMOND_IN,
      FADE_DIAMOND_OUT,
      FADE_GRID_CIRCLE_IN,
      FADE_GRID_CIRCLE_OUT,
      FADE_SPIRAL_IN,
      FADE_SPIRAL_OUT,
      FADE_UP_DOWN_IN,
      FADE_UP_DOWN_OUT,
      FADE_DOOR_OUT,
      FADE_INTRO_M,
      FADE_GAMEOVER,
      FADE_BASIC_IN,
      FADE_BASIC_OUT,
      FADE_SQUARE_IN,
      FADE_SQUARE_OUT
    };

    u32 mWipeRequest;
    f32 mWipeSpeed;
    f32 mDelayTime;
  };

  TSMSFader(JUtility::TColor color, f32, const char *name);
  virtual ~TSMSFader();

  virtual void load(JSUMemoryInputStream &);
  virtual void perform(u32, JDrama::TGraphics *);
  virtual void update();
  virtual void draw(const JDrama::TRect &);

  virtual void drawFadeinout(const JDrama::TRect &);

  void requestWipe(WipeRequest *request);
  void setColor(JUtility::TColor color);
  void setDisplaySize(int width, int height);
  void setFadeStatus(EFadeStatus status);
  void startFadeinT(f32 fadeTime);
  void startFadeoutT(f32 fadeTime);
  void startWipe(u32 requestKind, f32 fadeTime, f32 delayTime);

  u16 _0C;
  u16 _0E; // padding?
  u16 _10;
  u16 _12;
  f32 mWipeTimeCopy;
  JUtility::TColor mColor;
  bool _1C;
  EFadeStatus mFadeStatus;
  WipeRequest mQueuedWipeRequest;
};