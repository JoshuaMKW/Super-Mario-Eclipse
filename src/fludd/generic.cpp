#include "sms/actor/Mario.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "sms/sound/MSound.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "SME.hxx"

using namespace SME;

static TWaterGun *bindFluddtojoint()
{
  TMario *player;
  SME_FROM_GPR(31, player);

  if (!SME::TGlobals::getPlayerData(player))
    return player->mFludd;

  player->mBindBoneIDArray[0] =
      SME::TGlobals::getPlayerData(player)->getNozzleBoneID(
          static_cast<TWaterGun::NozzleType>(player->mFludd->mCurrentNozzle));

  return player->mFludd;
}
//SME_PATCH_BL(SME_PORT_REGION(0x8024D53C, 0x802452C8, 0, 0), bindFluddtojoint);