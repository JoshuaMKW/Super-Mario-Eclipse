#pragma once

#include "SelectDir.hxx"
#include "SelectGrad.hxx"
#include "sms/G2D/BoundPane.hxx"
#include "sms/G2D/ExPane.hxx"
#include "J2D/J2DPicture.hxx"
#include "J2D/J2DSetScreen.hxx"
#include "J2D/J2DTextBox.hxx"
#include "J2D/J2DTexture.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "sms/manager/SelectShineManager.hxx"

class TSelectMenu : public JDrama::TViewObj {
public:
  TSelectMenu(const char *);
  virtual ~TSelectMenu();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void loadAfter() override;
  virtual void perform(u32, JDrama::TGraphics *) override;

  u32 mState;
  u32 _14[0xC / 4];
  J2DSetScreen *mScreen;          // 0x0020
  TExPane *mMask1;                // 0x0024
  TExPane *mMask2;                // 0x0028
  J2DPane *mMap;                  // 0x002C
  J2DPane *mPaneDynName;          // 0x0030
  TBoundPane *mBoundPaneDynName1; // 0x0034
  TBoundPane *mBoundPaneDynName2; // 0x0038

  TExPane *mS_0;            // 0x0040
  J2DTextBox *mSttxTextBox; // 0x0044
  J2DPicture *mPicture0;    // 0x0048
  J2DPicture *mPicture1;    // 0x004C

  TExPane *m0_0;            // 0x0068
  J2DTextBox *m0ttxTextBox; // 0x006C

  J2DTexture *mShineCountNumberTex[8]; // 0x0080
  J2DPane *mI_0;                       // 0x00A0
  J2DPane *mSc_0;                      // 0x00A4

  J2DTexture *mCoinCountNumberTex[10]; // 0x00A8

  J2DPane *mA_l; // 0x0104
  J2DPane *mA_r; // 0x0108

  int mPosX1_0;                  // 0x0110
  int mPosX2_0;                  // 0x0114
  int mPosX1_1;                  // 0x0118
  int mPosX2_1;                  // 0x011C
  int mPosX1_2;                  // 0x0120
  int mPosX2_2;                  // 0x0124
  int mPosX1_3;                  // 0x0128
  int mPosX2_3;                  // 0x012C
  TSelectShineManager *mManager; // 0x0130
  TSelectDir *mDir;              // 0x0134

  u8 mWorldIndex;    // ? | 0x013A
  s8 mSelectedIndex; // 0x013B
  s8 mNextIndex;     // ? | 0x013C

  f32 mAnmDeltaTime;     // 0x014C
  u8 mSelectionState[8]; // 0x0150

  u32 _C0;
  u8 _C4;
  bool mIsSetUp;                 // 0x00C5
  u32 *mTextures[10];            // 0x00C8 - JUTTexture *
  J2DPane *mPane_ss_i;           // 0x00F4
  J2DPane *mPane_ss[2];          // 0x00F8
  J2DPane *mPane_sq_i;           // 0x0100
  J2DPane *mPane_sq[2];          // 0x0104
  J2DPane *mPane_sc[3];          // 0x010C
  J2DPane *mPane_sc_s;           // 0x0118
  J2DPane *mPane_sb[2];          // 0x011C
  J2DTextBox *mTextBox1;         // 0x0124
  TExPane *mExPanes[2];          // 0x0128
  J2DPicture *mPictureSun2;      // 0x0134
  J2DPicture *mPictureShip2;     // 0x0138
  J2DPicture *mPicturePalmTree2; // 0x013C
  J2DPicture *mPicturePalmTree1; // 0x0140
  J2DPicture *mPictureFish2;     // 0x0144
  J2DPane *mPane_11;             // 0x0148
  J2DPane *mPane_14;             // 0x014C
  J2DPane *mPane_15;             // 0x0150
  J2DPane *mPane_12;             // 0x0154
  J2DPane *mPane_19;             // 0x0158
  u32 _15C[3];
  J2DPane *mPanes[13]; // 0x0168
  J2DPane *mPane0;     // 0x019C
  u32 _1A0[8];
  TExPane *mExPanes0[13];     // 0x01C0
  TExPane *mLeftWindowExPane; // 0x01F4
  u32 _1F8[8];
  JUTRect mRects[13]; // 0x0218
  JUTRect mBorder0;   // ? 0x02E8
  u32 _2F8[0x80 / 4];
  TExPane *mExPanes1[13]; // 0x0378
  TExPane *mExPane1;      // 0x03AC
  u32 _3B0[0x64 / 4];
  TExPane *mPane_unk0;       // 0x0424
  TExPane *mPane_unk1;       // 0x0428
  s16 mSelectedRectIndex;    // 0x042C
  J2DPane *mPaneMarioIcon;   // 0x0430
  JUTRect mBorder1;          // ? 0x0434
  TBoundPane *mBoundPane_20; // 0x0444
  u32 _448;
  J2DPane *mPane_pn[10]; // 0x044C
  void *mStageNameBMG;   // 0x0474
  TExPane *mMarker;      // 0x0478
  u32 _47C[0x27C / 4];   // 0x047C
};