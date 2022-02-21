#include "sms/actor/item/Shine.hxx"
#include "JGeometry.hxx"
#include "sms/game/Conductor.hxx"
#include "sms/sound/MSBGM.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;

// 0x80293B10
static u32 extendShineIDLogic(TFlagManager *flagManager, u32 flagID) {
  if ((flagID & 0xFFFF) > 0x77)
    flagID += (0x60040 - 0x78);
  if (flagID > 0x60334)
    flagID = 0;
  return flagManager->getFlag(flagID);
}
SME_PATCH_BL(SME_PORT_REGION(0x80293B10, 0, 0, 0),
             extendShineIDLogic);

static void shineFlagSetter(TFlagManager *flagManager, u32 flag,
                                   s32 val) {
  if (flag < 0x60400) {
    flag &= 0xFFFF;
    if (flag < 0x40)
      ((u32 *)&flagManager->Type6Flag)[flag] = val;
    else {
      flag -= 0x40;

      const u32 mask = (flag & 7);
      u8 *flagField = &((u8 *)(&flagManager->Type6Flag) + 0x100)[flag >> 3];

      *flagField &= 1 << mask;
      *flagField |= (val & 1) << mask;
    }
  }
}
SME_WRITE_32(SME_PORT_REGION(0x803DEE50, 0, 0, 0),
             shineFlagSetter);

static u32 shineFlagGetter(TFlagManager *flagManager, u32 flag) {
  if (flag < 0x60400) {
    flag &= 0xFFFF;
    if (flag < 0x40)
      return ((u32 *)&flagManager->Type6Flag)[flag];
    else {
      flag -= 0x40;
      
      const u32 mask = (flag & 7);
      u8 *flagField = &((u8 *)(&flagManager->Type6Flag) + 0x100)[flag >> 3];

      return (*flagField >> mask) & 1;
    }
  }
  return false;
}
SME_WRITE_32(SME_PORT_REGION(0x803DEE7C, 0, 0, 0),
             shineFlagGetter);

// 0x802946D4
static u32 shineSetClamper(TFlagManager *flagManager, u32 flag) {
  flag &= 0x7FFFF;
  if (flag >= 0x10078)
    flag -= 0x10000;

  return flag;
}
SME_PATCH_BL(SME_PORT_REGION(0x802946D4, 0, 0, 0),
             shineSetClamper);

// 0x8029474C
static u32 shineGetClamper(TFlagManager *flagManager, u32 flag) {
  flag &= 0x7FFFF;
  if (flag >= 0x10078)
    flag -= 0x10000;

  return flagManager->getFlag(flag);
}
SME_WRITE_32(SME_PORT_REGION(0x80294744, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x8029474C, 0, 0, 0),
             shineGetClamper);

// 0x80294334
static void extendFlagManagerLoad(JSUMemoryInputStream &stream) {
  stream.read(((u8 *)TFlagManager::smInstance + 0x1F4), 0x8C);
  stream.skip(0x120);
}
SME_PATCH_BL(SME_PORT_REGION(0x80294334, 0, 0, 0),
             extendFlagManagerLoad);

// 0x802939B8
static void extendFlagManagerSave(JSUMemoryOutputStream &stream) {
  stream.write(((u8 *)TFlagManager::smInstance + 0x1F4), 0x8C);
  stream.skip(0x120, 0);
}
SME_PATCH_BL(SME_PORT_REGION(0x802939B8, 0, 0, 0),
             extendFlagManagerSave);

/*Shine casts, fix light*/
SME_WRITE_32(SME_PORT_REGION(0x80412548, 0, 0, 0), f32(SME_MAX_SHINES));
SME_WRITE_32(SME_PORT_REGION(0x80293AF8, 0, 0, 0), 0x3BFF03E7);
SME_WRITE_32(SME_PORT_REGION(0x802946B8, 0, 0, 0), 0x280003E7);
SME_WRITE_32(SME_PORT_REGION(0x8017BE78, 0, 0, 0), 0x5464037E);
SME_WRITE_32(SME_PORT_REGION(0x8017BEF4, 0, 0, 0), 0x5464037E);
SME_WRITE_32(SME_PORT_REGION(0x8017BF34, 0, 0, 0), 0x5464037E);
SME_WRITE_32(SME_PORT_REGION(0x801BCE30, 0, 0, 0), 0x5404037E);
SME_WRITE_32(SME_PORT_REGION(0x801FF850, 0, 0, 0), 0x5404037E);
SME_WRITE_32(SME_PORT_REGION(0x802946B4, 0, 0, 0), 0x5480043E);
SME_WRITE_32(SME_PORT_REGION(0x80294730, 0, 0, 0), 0x5480043E);
SME_WRITE_32(SME_PORT_REGION(0x80294734, 0, 0, 0), 0x280003E7);
SME_WRITE_32(SME_PORT_REGION(0x80297BA0, 0, 0, 0), 0x5404037E);
SME_WRITE_32(SME_PORT_REGION(0x80294338, 0, 0, 0), 0x48000010);
SME_WRITE_32(SME_PORT_REGION(0x802939BC, 0, 0, 0), 0x48000014);
SME_WRITE_32(SME_PORT_REGION(0x801BCD24, 0, 0, 0), 0x28030002);
SME_WRITE_32(SME_PORT_REGION(0x801BCD40, 0, 0, 0), 0x28030001);