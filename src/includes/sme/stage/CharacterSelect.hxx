#pragma once

#include "Player.hxx"
#include "sms/JSystem/J2D/J2DPicture.hxx"
#include "funcs.hxx"
#include "defines.h"

using namespace SME::Enum;

class TCharacterIcon final : public J2DPicture {
public:
  TCharacterIcon(J2DPane *, Player character, bool);
  virtual ~TCharacterIcon();

  Player getPlayerID() const { return mPlayerID; }

  static constexpr u32 getWidth() { return 32; }
  static constexpr u32 getHeight() { return 32; }

  void drawGrid(int x, int y, int row, int column);

private:
  Player mPlayerID;
};

class TCharacterSelect final : JDrama::TNameRef {
public:
  TCharacterSelect(u32 whitelist);
  virtual ~TCharacterSelect();

  virtual void load(JSUMemoryInputStream &) {}
  virtual void perform(u32, JDrama::TGraphics *);
  virtual void update();
  virtual void draw(const JDrama::TRect &);

  u32 getRowLength(u32 width) const;

  u32 getNumWhiteListed() const;
  u32 getWhiteList() const { return mWhiteList; }
  void setWhiteList(u32 whitelist);

  TCharacterIcon *getIconByIndex(u8 index) const {
    return mCharacterIcons[index];
  }
  s32 getSelectedIndex(u8 player) const { return mSelectedIndexList[player]; };

  void select(u8 player, s32 index);
private:
  JUTRect mMainPlacement;
  JUTRect mBannerPlacement;
  JUTRect mSelectPlacement;
  u32 mWhiteList;
  s8 mSelectedIndexList[SME_MAX_PLAYERS];
  bool mHasSelectedList[SME_MAX_PLAYERS];
  J2DPicture *mSelectBanner;
  J2DPicture *mSelectToken;
  TCharacterIcon *mCharacterIcons[32];
};