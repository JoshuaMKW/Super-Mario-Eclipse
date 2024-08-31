#include <SMS/Manager/FlagManager.hxx>
#include <SMS/Manager/ItemManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/stage.hxx>

#include "stage.hxx"

static void set_10bit_value(u32 *value, u32 mask, u32 shift, u16 new_value) {
    *value = (*value & ~mask) | ((new_value << shift) & mask);
}

static u16 get_10bit_value(u32 *value, u32 mask, u32 shift) { return (*value & mask) >> shift; }

static void set_10bit_flag(u32 *array, u8 index, u16 new_value) {
    u8 word_index = (index * 10) / 32;  // Determine which 32-bit word to use
    u8 bit_offset = (index * 10) % 32;  // Calculate the bit offset for big-endian

    if (bit_offset >= 22) {
        // If the 10-bit value spans two 32-bit words
        u8 shift = bit_offset - 21;
        set_10bit_value(&array[word_index], 0x3FF >> shift, 0, new_value >> shift);
        set_10bit_value(&array[word_index + 1], 0x3FF << (32 - shift), 32 - shift, new_value);
    } else {
        // If the 10-bit value fits within one 32-bit word
        u8 shift = 21 - bit_offset;
        set_10bit_value(&array[word_index], 0x3FF << shift, shift, new_value);
    }
}

static u16 get_10bit_flag(u32 *array, u8 index) {
    u8 word_index = (index * 10) / 32;  // Determine which 32-bit word to use
    u8 bit_offset = (index * 10) % 32;  // Calculate the bit offset for big-endian

    if (bit_offset >= 22) {
        // If the 10-bit value spans two 32-bit words
        u8 shift = bit_offset - 21;
        return (get_10bit_value(&array[word_index], 0x3FF >> shift, 0) << shift) |
               get_10bit_value(&array[word_index + 1], 0x3FF << (32 - shift), 32 - shift);
    } else {
        // If the 10-bit value fits within one 32-bit word
        u8 shift = 21 - bit_offset;
        return get_10bit_value(&array[word_index], 0x3FF << shift, shift);
    }
}

static void flagManager2XSetter(TFlagManager *manager, u32 flags, s32 val) {
    flags &= 0xFFFF;
    if (flags < 0xE || flags == 0x12 || flags == 0x13 || flags == 0x14) {
        *(u32 *)((u8 *)manager + 0x78 + (flags << 2)) = val;
    } else {
        if (flags > 0x14) {
            flags -= 3;
        }
        flags -= 0xE;
        return set_10bit_flag((u32 *)((u8 *)manager + 0x78) + 0xE, flags, val);
    }
}
SMS_WRITE_32(SMS_PORT_REGION(0x803DEE40, 0x803D6620, 0, 0), flagManager2XSetter);

static u32 flagManager2XGetter(TFlagManager *manager, u32 flags) {
    flags &= 0xFFFF;
    if (flags < 0xE || flags == 0x12 || flags == 0x13 || flags == 0x14) {
        return *(u32 *)((u8 *)manager + 0x78 + (flags << 2));
    } else {
        if (flags > 0x14) {
            flags -= 3;
        }
        flags -= 0xE;
        return get_10bit_flag((u32 *)((u8 *)manager + 0x78) + 0xE, flags);
    }
    return 0;
}
SMS_WRITE_32(SMS_PORT_REGION(0x803DEE6C, 0x803D664C, 0, 0), flagManager2XGetter);

static u32 shineStageCoinRecordIndex(u32 normalStage) {
    u32 stage = SMS_getShineStage__FUc(normalStage);
    switch (stage) {
    case SME::STAGE_ERTO:
        return 9;
    case SME::STAGE_LIGHTHOUSE:
        return 10;
    case SME::STAGE_WARSHIP:
        return 11;
    case SME::STAGE_LACRIMA:
        return 12;
    case (SME::STAGE_MARIO_DREAM - 2):
        return 16;
    case (SME::STAGE_LANCIA - 2):
        return 17;
    case (SME::STAGE_VAPORWAVE - 2):
        return 18;
    case (SME::STAGE_YOSHI_VILLAGE - 2):
        return 19;
    case (SME::STAGE_RED_LILY - 2):
        return 20;
    case (SME::STAGE_PEACH_BEACH - 2):
        return 21;
    case (SME::STAGE_SPETTRO_CASINO - 2):
        return 22;
    default:
        return stage;
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8017495C, 0, 0, 0), shineStageCoinRecordIndex);

constexpr static const char *sShineKey =
    "\x83\x56\x83\x83\x83\x43\x83\x93\x81\x69\x82\x50\x82\x4F\x82\x4F\x96\x87\x83"
    "\x52\x83\x43\x83\x93\x97\x70\x81\x6A\x00\x00";

constexpr static const char *sShineDemoKey =
    "\x83\x56\x83\x83\x83\x43\x83\x93\x81\x69\x82\x50\x82\x4F\x82\x4F\x96\x87\x83"
    "\x52\x83\x43\x83\x93\x97\x70\x81\x6A\x83\x4A\x83\x81\x83\x89\x00\x00";

static void spawn100CoinShine(const TVec3f &position) {
    auto *nameref = TMarNameRefGen::getInstance()->getRootNameRef();

    u16 keycode = JDrama::TNameRef::calcKeyCode(sShineKey);
    if (nameref->searchF(keycode, sShineKey)) {
        gpItemManager->makeShineAppearWithDemo(sShineKey, sShineDemoKey, position.x, position.y,
                                               position.z);
    }
}

static bool sIs100ShineSpawned = false;

static void shineStageCoinRecordUpdate() {
    u32 normalStageID = gpApplication.mCurrentScene.mAreaID;
    u32 shineStageID  = shineStageCoinRecordIndex(normalStageID);
    if (normalStageID > TGameSequence::AREA_CORONABOSS) {
        if (shineStageID == SMS_getShineStage__FUc(normalStageID)) {
            // Under this circumstance the area has no record, see above.
            TFlagManager::smInstance->incFlag(0x40002, 1);

            // BetterSMS fixes this in incGoldCoinFlag path
            // so we only do it ourselves when the func isn't called.
            size_t coins = TFlagManager::smInstance->getFlag(0x40002);
            if (BetterSMS::areExploitsPatched()) {
                if (!sIs100ShineSpawned && coins > 99) {
                    spawn100CoinShine(*gpMarioPos);
                    sIs100ShineSpawned = true;
                }
            } else if (coins == 100) {
                spawn100CoinShine(*gpMarioPos);
            }
            return;
        }
    }
    TFlagManager::smInstance->incGoldCoinFlag(shineStageID, 1);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x801BECD8, 0, 0, 0), shineStageCoinRecordUpdate);

static void setBlueCoinFlagOverride(TFlagManager *manager, u8 normalStage, u8 coinID) {
    s32 shineStage = SMS_getShineStage__FUc(normalStage);
    if (shineStage == TGameSequence::AREA_AIRPORT) {
        shineStage = TGameSequence::AREA_DOLPIC;
    }

    if (shineStage >= SME::STAGE_ERTO && shineStage < SME::STAGE_PEACH_CASTLE) {
        if (shineStage >= SME::STAGE_UNUSED_73 - 2) {
            shineStage -= 1;
        }
        shineStage -= SME::STAGE_ERTO;
        if (coinID >= 20) {
            coinID = 0;
        }
        u32 flagID = shineStage * 20 + coinID + 0x1023A;
        if (!manager->getBool(flagID)) {
            manager->incFlag(0x40001, 1);
            manager->setBool(true, flagID);
        }
        return;
    }

    if (shineStage > TGameSequence::AREA_MARE) {
        shineStage = TGameSequence::AREA_DOLPIC;
    }

    if (coinID >= 50) {
        coinID = 0;
    }

    u32 flagID = (shineStage - 1) * 50 + coinID + 0x10078;
    if (!manager->getBool(flagID)) {
        manager->incFlag(0x40001, 1);
        manager->setBool(true, flagID);
    }
}
SMS_PATCH_B(SMS_PORT_REGION(0x802944CC, 0, 0, 0), setBlueCoinFlagOverride);

static bool getBlueCoinFlagOverride(TFlagManager *manager, u8 normalStage, u8 coinID) {
    s32 shineStage = SMS_getShineStage__FUc(normalStage);
    if (shineStage == TGameSequence::AREA_AIRPORT) {
        shineStage = TGameSequence::AREA_DOLPIC;
    }

    if (shineStage >= SME::STAGE_ERTO && shineStage < SME::STAGE_PEACH_CASTLE) {
        if (shineStage >= SME::STAGE_UNUSED_73 - 2) {
            shineStage -= 1;
        }
        shineStage -= SME::STAGE_ERTO;
        if (coinID >= 20) {
            coinID = 0;
        }
        return manager->getBool(shineStage * 20 + coinID + 0x1023A);
    }

    if (shineStage > TGameSequence::AREA_MARE) {
        shineStage = TGameSequence::AREA_DOLPIC;
    }

    if (coinID >= 50) {
        coinID = 0;
    }

    return manager->getBool((shineStage - 1) * 50 + coinID + 0x10078);
}
SMS_PATCH_B(SMS_PORT_REGION(0x80294580, 0, 0, 0), getBlueCoinFlagOverride);

static size_t countBlueCoinShines() {
    size_t count = 0;
    for (u32 i = 0x46; i < 0x56; i++) {
        count += TFlagManager::smInstance->getBool(0x10000 + i);
    }
    for (u32 i = 0x6C; i < 0x74; i++) {
        count += TFlagManager::smInstance->getBool(0x10000 + i);
    }
    for (u32 i = 221; i < 239; ++i) {
        count += TFlagManager::smInstance->getShineFlag(i);
    }
    return count;
}
SMS_PATCH_BL(0x80146bfc, countBlueCoinShines);
SMS_WRITE_32(0x80146c00, 0x7c791b78);
SMS_PATCH_B(0x80146c04, 0x80146c50);

SMS_PATCH_BL(0x801475c8, countBlueCoinShines);
SMS_WRITE_32(0x801475cc, 0x7c7c1b78);
SMS_PATCH_B(0x801475d0, 0x8014761c);

SMS_PATCH_BL(0x8014df4c, countBlueCoinShines);
SMS_WRITE_32(0x8014df50, 0x7c771b78);
SMS_PATCH_B(0x8014df54, 0x8014dfa0);

SMS_PATCH_BL(0x8015120c, countBlueCoinShines);
SMS_WRITE_32(0x80151210, 0x7c781b78);
SMS_PATCH_B(0x80151214, 0x80151260);

// STATIC RESETTER
BETTER_SMS_FOR_EXPORT void reset100CoinState(TMarDirector *director) { sIs100ShineSpawned = false; }