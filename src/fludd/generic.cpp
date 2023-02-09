#include <SMS/Player/Mario.hxx>
#include <SMS/Player/Watergun.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>

using namespace BetterSMS;

static TWaterGun *bindFluddtojoint()
{
  TMario *player;
  SMS_FROM_GPR(31, player);

  if (!Player::getData(player))
    return player->mFludd;

  /*player->mBindBoneIDArray[0] =
      Player::getData(player)->getNozzleBoneID(
          static_cast<TWaterGun::TNozzleType>(player->mFludd->mCurrentNozzle));*/

  return player->mFludd;
}
//SME_PATCH_BL(SME_PORT_REGION(0x8024D53C, 0x802452C8, 0, 0), bindFluddtojoint);