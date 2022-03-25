#include "types.h"

#include "SME.hxx"

#if SME_EXTRA_OBJECTS
static void *loadFromGlobalAndScene(const char *mdl, u32 unk_0,
                                    const char *path) {
  u32 **sdlModel = reinterpret_cast<u32 **>(
      loadModelData__16TModelDataKeeperFPCcUlPCc(mdl, unk_0, path));
  if (*sdlModel == nullptr) {
    delete sdlModel;
    sdlModel =
        reinterpret_cast<u32 **>(loadModelData__16TModelDataKeeperFPCcUlPCc(
            mdl, unk_0, "/common/mapobj"));
  }
  return sdlModel;
}
SME_PATCH_BL(SME_PORT_REGION(0x8021CD34, 0, 0, 0), loadFromGlobalAndScene);
#endif

// READY GO TEXT PATCH FOR THIS BULLSHIT THING DADDY NINTENDO DID
SME_WRITE_32(SME_PORT_REGION(0x80171C30, 0, 0, 0), 0x2C000005);
SME_WRITE_32(SME_PORT_REGION(0x80171C38, 0, 0, 0), 0x38000005);

// Title Screen Never Fades to THP
SME_WRITE_32(SME_PORT_REGION(0x8016D53C, 0, 0, 0), 0x48000344);

// Load msound.aaf from AudioRes folder (NTSC-U) [Xayrga/JoshuaMK]
SME_WRITE_32(SME_PORT_REGION(0x80014F9C, 0, 0, 0), 0x60000000);

/* -- PATCHES -- */

#if SME_BUGFIXES
// Restore Chao Seed
SME_WRITE_32(SME_PORT_REGION(0x802FD1A0, 0, 0, 0), 0x808D8C70);

// Fix Infinte Flutter
SME_WRITE_32(SME_PORT_REGION(0x8028113C, 0, 0, 0), 0xC002F824);

// Fix Health Meter Not Rising Underwater
SME_WRITE_32(SME_PORT_REGION(0x801410E4, 0, 0, 0), 0x60000000);
#endif

// Sunscript logging restoration
SME_WRITE_32(SME_PORT_REGION(0x8003DB3C, 0, 0, 0), 0x48306B08);

#if SME_EXCEPTION_HANDLER
// Show Exception Handler
SME_WRITE_32(SME_PORT_REGION(0x8029D0BC, 0, 0, 0), 0x60000000);

// Extend Exception Handler
SME_WRITE_32(SME_PORT_REGION(0x802C7638, 0, 0, 0), 0x60000000); // gpr info
// SME_WRITE_32(SME_PORT_REGION(0x802C7690, 0, 0, 0), 0x60000000); // float
#endif

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
// Skip FMVs
SME_WRITE_32(SME_PORT_REGION(0x802B5E8C, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x802B5EF4, 0, 0, 0), 0x38600001);
#endif

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
// Level Select
SME_WRITE_32(SME_PORT_REGION(0x802A6788, 0, 0, 0), 0x3BC00009);
#endif

// Remove Dive While Wall Sliding
SME_WRITE_32(SME_PORT_REGION(0x8024BC10, 0, 0, 0), 0x48000068);

// Flood Till Corona Beat
SME_WRITE_32(SME_PORT_REGION(0x8029961C, 0, 0, 0), 0x38840077);

// Map on D Pad down
#if !SME_DEMO
SME_WRITE_32(SME_PORT_REGION(0x8017A830, 0, 0, 0), 0x5400077B);
SME_WRITE_32(SME_PORT_REGION(0x80297A60, 0, 0, 0), 0x5400077B);
#else
SME_WRITE_32(SME_PORT_REGION(0x8017A834, 0, 0, 0), 0x4800000C);
SME_WRITE_32(SME_PORT_REGION(0x80297A64, 0, 0, 0), 0x4800000C);
#endif

// Global surfing bloopies
SME_WRITE_32(SME_PORT_REGION(0x801B74F8, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B74FC, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B7500, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B7504, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B7508, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B750C, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B7510, 0, 0, 0), 0x387E0780);
SME_WRITE_32(SME_PORT_REGION(0x801B7514, 0, 0, 0), 0x4810BA9D);
SME_WRITE_32(SME_PORT_REGION(0x801B7518, 0, 0, 0), 0x28030000);
SME_WRITE_32(SME_PORT_REGION(0x801B751C, 0, 0, 0), 0x418200A4);

// Remove blue coin prompts
SME_WRITE_32(SME_PORT_REGION(0x8029A73C, 0, 0, 0), 0x60000000);

// Fruit don't time out
SME_WRITE_32(SME_PORT_REGION(0x8040C918, 0, 0, 0), 0x7FFFFFFF);