#pragma once

#include "sms/GC2D/ConsoleStr.hxx"
#include "sms/JSystem/J2D/J2DPicture.hxx"
#include "sms/JSystem/J2D/J2DWindow.hxx"
#include "sms/JSystem/JUT/JUTColor.hxx"
#include "types.h"

class TGCConsole2 {

public:
  u32 _00[0x24 / 4];                      // 0x0000
  s32 mShineCount;                        // 0x0024
  s32 mCurWater;                          // 0x0028
  u32 _01[0x18 / 4];                      // 0x002C
  bool mIsDEBSAlert;                      // 0x0044
  bool mWaterCardRising;                  // 0x0045
  bool mIsWaterCard;                      // 0x0046
  u8 _02;                                 // 0x0047
  u16 _03;                                // 0x0048
  u8 _04;                                 // 0x004A
  bool mWaterCardFalling;                 // 0x004B
  u32 _05[0x48 / 4];                      // 0x004C
  TConsoleStr *mConsoleStr;               // 0x0094
  u16 _06;                                // 0x0098
  JUtility::TColor mWaterLeftPanelColor;  // 0x009A
  JUtility::TColor mWaterRightPanelColor; // 0x009E
  JUtility::TColor mJuiceCardOrangeColor; // 0x00A2
  JUtility::TColor mJuiceCardPurpleColor; // 0x00A6
  JUtility::TColor mJuiceCardPinkColor;   // 0x00AA
  u16 _07;                                // 0x00AE
  u32 _08[0x1F0 / 4];                     // 0x00B0
  J2DPicture *mWaterTopPanel;             // 0x02A0
  u32 _09[0x14 / 4];                      // 0x02A4
  u32 mWaterCardTopHeight;                // 0x02B8
  u32 _10[0x268 / 4];                     // 0x02BC
  TExPane *mTelopPanel;                   // 0x0524
  TExPane *mTelopWindow;                  // 0x0528
  u32 _528[0x50 / 4];

  //0x1C4 = TExPane *LifeMeter
  //0x2F8 = TExPane *WaterBackPanel

  //0x314 = J2DPicture *FruitText
};