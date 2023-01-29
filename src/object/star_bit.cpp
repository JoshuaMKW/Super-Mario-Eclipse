#include "obj/StarBit.hxx"
#include "sms/rand.h"
#include "types.h"

#include "J2D/J2DTexture.hxx"
#include "J3D/J3DTexture.hxx"
#include "J3D/J3DMaterial.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "JUT/JUTColor.hxx"
#include "sms/actor/item/Coin.hxx"

const JUtility::TColor TStarBit::sStarBitColorArray[6]{
    {0xEB, 0x57, 0x3D, 0xFF}, // RED
    {0xFF, 0xF1, 0x32, 0xFF}, // YELLOW
    {0x26, 0xEB, 0x28, 0xFF}, // GREEN
    {0x4C, 0x90, 0xFF, 0xFF}, // BLUE
    {0xCB, 0x47, 0xFF, 0xFF}, // PURPLE
    {0xD0, 0xD0, 0xD0, 0xFF}  // SILVER
};

TStarBit::TStarBit(const char *name)
    : TCoin(name), mIsMoving(false), mIsBouncing(false), mBounceSpeedY(50.0f),
      mGlowEffect(nullptr), mGlowSize(0.0f), mGlowAnmSpeed(1.0f) {
  mColor = getColorFromIndex(rand() % sizeof(sStarBitColorArray));
}

TStarBit::~TStarBit() {}

void TStarBit::perform(u32 flags, JDrama::TGraphics *graphics) {
    TCoin::perform(flags, graphics);
}

void TStarBit::control() {
  if (!mIsMoving) {
    mGravity = 0.0f;
    mSpeed = {0.0f, 0.0f, 0.0f};
    return;
  }

  mGravity = 0.6f;
  if (!mIsBouncing || mGroundY > (mPosition.y - 20.0f))
    return;

  mSpeed.y = mBounceSpeedY;
}

void TStarBit::touchWater(THitActor *actor) { taken(actor); }

void TStarBit::taken(THitActor *actor) {
    TCoin::taken(actor);
    // ...
}