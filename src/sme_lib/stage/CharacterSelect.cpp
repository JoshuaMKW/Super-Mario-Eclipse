#include "stage/CharacterSelect.hxx"
#include "funcs.hxx"
#include "sms/SMS.hxx"
#include "Globals.hxx"

using namespace SME;

constexpr int BannerPosX = 120;
constexpr int BannerPosY = 120;
constexpr int BannerSizeX = 400;
constexpr int BannerSizeY = 208;

void TCharacterIcon::drawGrid(int x, int y, int row, int column) {
  (void)0;
}

u32 TCharacterSelect::getNumWhiteListed() const {
  u32 num = 0;
  for (u32 i = 0; i < 32; ++i) {
    num += (mWhiteList >> i) & 1;
  }
  return num;
}

void TCharacterSelect::select(u8 player, s32 index) {
  TGlobals::sCharacterIDList[player] = getIconByIndex(index)->getPlayerID();
  mHasSelectedList[player] = true;
}

void TCharacterSelect::update() {
  for (u32 i = 0; i < TGlobals::getActivePlayers(); ++i) {
    TMarioGamePad *controller = (&gpApplication.mGamePad1)[i];

    const u32 numWhiteListed = getNumWhiteListed();

    if (controller->mButtons.mFrameInput & TMarioGamePad::A) {
        select(i, mSelectedIndexList[i]);
        return;
    }

    if ((controller->mButtons.mFrameInput & TMarioGamePad::DPAD_RIGHT) ||
        controller->mControlStick.mStickX > 0.5f) {
        mSelectedIndexList[i] += 1;
    } else if ((controller->mButtons.mFrameInput & TMarioGamePad::DPAD_LEFT) ||
                controller->mControlStick.mStickX < -0.5f) {
        mSelectedIndexList[i] -= 1;
    } else if ((controller->mButtons.mFrameInput & TMarioGamePad::DPAD_DOWN) ||
                controller->mControlStick.mStickY < -0.5f) {
        mSelectedIndexList[i] += getRowLength(mMainPlacement.mX1 + mMainPlacement.mX2);
    } else if ((controller->mButtons.mFrameInput & TMarioGamePad::DPAD_UP) ||
                controller->mControlStick.mStickY > 0.5f) {
        mSelectedIndexList[i] -= getRowLength(mMainPlacement.mX1 + mMainPlacement.mX2);
    }

    if (mSelectedIndexList[i] > numWhiteListed) {
        mSelectedIndexList[i] -= numWhiteListed;
    } else if (mSelectedIndexList[i] < 0) {
        mSelectedIndexList[i] += numWhiteListed;
    }
  }
}

void TCharacterSelect::draw(const JDrama::TRect &rect) {
  mSelectBanner->draw(BannerPosX, BannerPosY, BannerSizeX, BannerSizeY, false, false, false);

  s32 trueIndex = 0;
  s32 row = 0;
  s32 column = 0;
  const u32 rowLength = getRowLength(mMainPlacement.mX1 + mMainPlacement.mX2);
  for (u32 i = 0; i < 32; ++i) {
    column = trueIndex % rowLength;
    row = trueIndex / rowLength;

    mCharacterIcons[i]->drawGrid(rect.mX1, rect.mY1, row, column);
    ++trueIndex;
  }
  // fill the rest later... draw cursor/token and each icon at dynamic positions!
  // maybe translate and resize according to the passed TRect reference...
}

u32 TCharacterSelect::getRowLength(u32 width) const {
  constexpr u32 iconWidth = TCharacterIcon::getWidth();
  return width / (iconWidth + (iconWidth / 4));
}