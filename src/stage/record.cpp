#include <SMS/Manager/FlagManager.hxx>
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
    u8 bit_offset = (index * 10) % 32;  // Determine the bit offset within the word

    if (bit_offset <= 22) {
        // If the 10-bit value fits within one 32-bit word
        set_10bit_value(&array[word_index], 0x3FF << bit_offset, bit_offset, new_value);
    } else {
        // If the 10-bit value spans two 32-bit words
        u8 remaining_bits = 10 - (32 - bit_offset);
        set_10bit_value(&array[word_index], 0x3FF << bit_offset, bit_offset,
                        new_value << remaining_bits);
        set_10bit_value(&array[word_index + 1], (1 << remaining_bits) - 1, 0,
                        new_value >> (10 - remaining_bits));
    }
}

static u16 get_10bit_flag(u32 *array, u8 index) {
    u8 word_index = (index * 10) / 32;  // Determine which 32-bit word to use
    u8 bit_offset = (index * 10) % 32;  // Determine the bit offset within the word

    OSReport("Word index: %d, Bit Offset: %d\n", word_index, bit_offset);

    if (bit_offset <= 22) {
        // If the 10-bit value fits within one 32-bit word
        return get_10bit_value(&array[word_index], 0x3FF << bit_offset, bit_offset);
    } else {
        // If the 10-bit value spans two 32-bit words
        u8 remaining_bits = 10 - (32 - bit_offset);
        u16 lower_part =
            get_10bit_value(&array[word_index], 0x3FF << bit_offset, bit_offset) >> remaining_bits;
        u16 upper_part = get_10bit_value(&array[word_index + 1], (1 << remaining_bits) - 1, 0);
        return (upper_part << (10 - remaining_bits)) | lower_part;
    }
}

static void flagManager2XSetter(TFlagManager *manager, u32 flags, s32 val) {
    flags &= 0xFFFF;
    if (flags < 0xE || flags == 0x12 || flags == 0x13 || flags == 0x14) {
        *(u32 *)((u8 *)manager + 0x78 + (flags << 2)) = val;
    } else {
        if (flags > 0x14) {
            flags -= 1;
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
            flags -= 1;
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
    case SME::STAGE_MARIO_DREAM:
        return 13;
    case SME::STAGE_LANCIA:
        return 14;
    case SME::STAGE_VAPORWAVE:
        return 15;
    case SME::STAGE_YOSHI_VILLAGE:
        return 16;
    case SME::STAGE_RED_LILY:
        return 18;
    case SME::STAGE_PEACH_BEACH:
        return 19;
    case SME::STAGE_SPETTRO_CASINO:
        return 20;
    default:
        return stage;
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8017495C, 0, 0, 0), shineStageCoinRecordIndex);
SMS_PATCH_BL(SMS_PORT_REGION(0x801BECC8, 0, 0, 0), shineStageCoinRecordIndex);

static void setBlueCoinFlagOverride(TFlagManager *manager, u8 normalStage, u8 coinID) {
    s32 shineStage = SMS_getShineStage__FUc(normalStage);
    if (shineStage == TGameSequence::AREA_AIRPORT) {
        shineStage = TGameSequence::AREA_DOLPIC;
    }

    if (shineStage >= SME::STAGE_ERTO && shineStage < SME::STAGE_PEACH_CASTLE) {
        if (shineStage >= SME::STAGE_UNUSED_73) {
            shineStage -= 1;
        }
        shineStage -= SME::STAGE_ERTO;
        if (coinID >= 20) {
            coinID = 0;
        }
        u32 flagID = shineStage * 20 + coinID + 0x1023A;
        if (!manager->getBool(flagID)) {
            manager->incFlag(0x40001, 1);
            manager->setBool(flagID, true);
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
        manager->setBool(flagID, true);
    }
}
SMS_PATCH_B(SMS_PORT_REGION(0x802944CC, 0, 0, 0), setBlueCoinFlagOverride);

static bool getBlueCoinFlagOverride(TFlagManager *manager, u8 normalStage, u8 coinID) {
    s32 shineStage = SMS_getShineStage__FUc(normalStage);
    if (shineStage == TGameSequence::AREA_AIRPORT) {
        shineStage = TGameSequence::AREA_DOLPIC;
    }

    if (shineStage >= SME::STAGE_ERTO && shineStage < SME::STAGE_PEACH_CASTLE) {
        if (shineStage >= SME::STAGE_UNUSED_73) {
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