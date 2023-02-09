#include <SMS/GC2D/GCConsole2.hxx>
#include <BetterSMS/module.hxx>

#include "settings.hxx"

using namespace BetterSMS;

void manageE3HealthMeter(TMarDirector *director, J2DGrafContext *context) {
    switch (getHUDKind()) {
    case HUDSetting::E3: {
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
}