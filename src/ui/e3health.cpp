#include "SME.hxx"
#include "sms/game/GCConsole2.hxx"

using namespace SME;

void manageE3HealthMeter(J2DScreen *screen, int x, int y,
                         J2DGrafContext *context) {
  switch (TGlobals::getUIKind()) {
  case Enum::UIKind::E3: {
    TGCConsole2 *console = gpMarDirector->mGCConsole;
    const s16 health = gpMarioAddress->mHealth;

    for (int i = 0; i < 9; ++i) {
      console->mHealthPoints[i].mActivePicture->mIsVisible = i == health;
      console->mHealthPoints[i].mInactivePicture->mIsVisible = i == health;
    }
    break;
  }
  default:
    break;
  }
  screen->draw(x, y, context);
}
SME_PATCH_BL(SME_PORT_REGION(0x80143F50, 0x80138B8C, 0, 0), manageE3HealthMeter);