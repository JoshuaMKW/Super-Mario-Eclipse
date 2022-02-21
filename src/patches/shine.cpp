#include "sms/manager/FlagManager.hxx"

#include "SME.hxx"

#if SME_BUGFIXES

static bool sIs100ShineSpawned = false;
static bool is100CoinShine(TFlagManager *manager, u32 id) {
  if (!sIs100ShineSpawned && manager->getFlag(id) > 99) {
    sIs100ShineSpawned = true;
    return true;
  }
  return false;
}
SME_PATCH_BL(SME_PORT_REGION(0x801BED3C, 0, 0, 0), is100CoinShine);
SME_WRITE_32(SME_PORT_REGION(0x801BED40, 0, 0, 0), 0x2C030001);

// STATIC RESETTER
void patches_staticResetter() { sIs100ShineSpawned = false; }

#endif