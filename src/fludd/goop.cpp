#include <SMS/Player/Mario.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Manager/PollutionManager.hxx>
#include <SMS/Player/Watergun.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>

//static void sprayGoopMap(TPollutionManager *gpPollutionManager, f32 x, f32 y,
//                         f32 z, f32 r) {
//  auto *playerData =
//      Player::getData(gpMarioAddress);
//  auto *prm = playerData->getParams();
//
//  if (!playerData->isMario())
//    clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);
//
//  if (prm->mCleaningType.get() !=
//      Player::TPlayerParams::FluddCleanType::NONE) {
//    if (prm->mCleaningType.get() ==
//        Player::TPlayerParams::FluddCleanType::CLEAN)
//        clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);
//    else if (prm->mCleaningType.get() ==
//        Player::TPlayerParams::FluddCleanType::GOOP)
//      stamp__17TPollutionManagerFUsffff(gpPollutionManager, 1, x, y, z, r);
//  }
//}
//SMS_PATCH_BL(SMS_PORT_REGION(0x800678C4, 0x80060F64, 0, 0), sprayGoopMap);
//SMS_PATCH_BL(SMS_PORT_REGION(0x801A3ED0, 0x8019C758, 0, 0), sprayGoopMap);
//SMS_PATCH_BL(SMS_PORT_REGION(0x801B42D8, 0x801AC190, 0, 0), sprayGoopMap);
//SMS_PATCH_BL(SMS_PORT_REGION(0x8024E710, 0x8024649C, 0, 0), sprayGoopMap);
//SMS_PATCH_BL(SMS_PORT_REGION(0x8027F7DC, 0x80277568, 0, 0), sprayGoopMap);
//SMS_PATCH_BL(SMS_PORT_REGION(0x8027F94C, 0x802776D8, 0, 0), sprayGoopMap);
//
//static bool canCleanSeals(TModelWaterManager *gpWaterManager) {
//  return gpWaterManager->mWaterCardType != 0 ||
//         Player::getData(gpMarioAddress)
//             ->getParams()
//             ->mCanCleanSeals.get();
//}
//SMS_PATCH_BL(SMS_PORT_REGION(0x800FED3C, 0x800F83DC, 0, 0), canCleanSeals);
//SMS_WRITE_32(SMS_PORT_REGION(0x800FED40, 0x800F83E0, 0, 0), 0x2C030000);