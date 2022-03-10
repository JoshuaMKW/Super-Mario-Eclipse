#include "sms/actor/Mario.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "sms/sound/MSound.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "SME.hxx"

#if SME_FLUDD_GOOP_MOD

static void sprayGoopMap(TPollutionManager *gpPollutionManager, f32 x, f32 y,
                         f32 z, f32 r) {
  const SME::Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerData(gpMarioAddress);
  const SME::Class::TPlayerParams *prm = playerParams->getParams();

  if (!playerParams->isMario())
    clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);

  if (prm->mCleaningType.get() !=
      SME::Class::TPlayerParams::FluddCleanType::NONE) {
    if (prm->mCleaningType.get() ==
        SME::Class::TPlayerParams::FluddCleanType::CLEAN)
      clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);
    else if (prm->mCleaningType.get() ==
             SME::Class::TPlayerParams::FluddCleanType::GOOP)
      stamp__17TPollutionManagerFUsffff(gpPollutionManager, 1, x, y, z, r);
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x800678C4, 0x80060F64, 0, 0), sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x801A3ED0, 0x8019C758, 0, 0), sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x801B42D8, 0x801AC190, 0, 0), sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x8024E710, 0x8024649C, 0, 0), sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x8027F7DC, 0x80277568, 0, 0), sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x8027F94C, 0x802776D8, 0, 0), sprayGoopMap);

static bool canCleanSeals(TModelWaterManager *gpWaterManager) {
  return gpWaterManager->mWaterCardType != 0 ||
         SME::TGlobals::getPlayerData(gpMarioAddress)
             ->getParams()
             ->mCanCleanSeals.get();
}
SME_PATCH_BL(SME_PORT_REGION(0x800FED3C, 0x800F83DC, 0, 0), canCleanSeals);
SME_WRITE_32(SME_PORT_REGION(0x800FED40, 0x800F83E0, 0, 0), 0x2C030000);

#endif