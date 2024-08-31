#include <Dolphin/types.h>

#include <SMS/GC2D/ConsoleStr.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>

#include "stage.hxx"

static size_t s_stage_shines_collected = 0;
static size_t s_stage_shines_max       = 0;

static size_t s_stage_blues_collected = 0;
static size_t s_stage_blues_max       = 0;

static size_t s_shine_counter = 0;
static size_t s_blue_counter  = 0;

static J2DTextBox *s_shine_text_front = nullptr;
static J2DTextBox *s_shine_text_back  = nullptr;
static J2DTextBox *s_blue_text_front  = nullptr;
static J2DTextBox *s_blue_text_back   = nullptr;

static char s_shine_text_buffer[64] = {};
static char s_blue_text_buffer[64]  = {};

struct ShineInfo {
    u16 m_100_flag;
    u16 m_flag_start;
    u16 m_count;
};

BETTER_SMS_FOR_CALLBACK void initializeShineBlueTracker(TMarDirector *director) {
    s_stage_shines_collected = 0;
    s_stage_blues_collected  = 0;

    s_stage_shines_max = 0;
    s_stage_blues_max  = 0;

    s_shine_counter = TFlagManager::smInstance->getFlag(0x40000);
    s_blue_counter  = TFlagManager::smInstance->getFlag(0x40001);

    u32 shineStage = SMS_getShineStage__FUc(director->mAreaID);
    switch (shineStage) {
    case TGameSequence::AREA_AIRPORT: {
        s_stage_shines_collected += TFlagManager::smInstance->getFlag(0x10056);
        s_stage_shines_max = 1;
        s_stage_blues_max  = 0;
        break;
    }
    case TGameSequence::AREA_DOLPIC: {
        // Blue coin shines
        for (u32 i = 0x10046; i <= 0x10055; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
            s_stage_shines_max += 1;
        }

        // Blue coin shines
        for (u32 i = 0x1006C; i <= 0x10073; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
            s_stage_shines_max += 1;
        }

        // Blue coin shines (Eclipse)
        for (u32 i = 221; i <= 238; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
            s_stage_shines_max += 1;
        }

        for (u32 i = 0x10057; i <= 0x10063; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
            s_stage_shines_max += 1;
        }

        for (u32 i = 0x10074; i <= 0x10077; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
            s_stage_shines_max += 1;
        }

        for (u32 i = 0; i < 50; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }

        s_stage_shines_collected += TFlagManager::smInstance->getFlag(0x1006B);  // 100 coins
        s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(239);  // 100 coins
        s_stage_shines_max += 2;

        s_stage_blues_max = 20;
        break;
    }
    case TGameSequence::AREA_BIANCO:
    case TGameSequence::AREA_RICCO:
    case TGameSequence::AREA_MAMMA:
    case TGameSequence::AREA_PINNABEACH:
    case TGameSequence::AREA_SIRENA: {
        u32 shine_flag = 0x10000 + (shineStage - 2) * 10;
        for (u32 i = shine_flag; i < shine_flag + 10; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
        }
        for (u32 i = 0; i < 50; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_collected +=
            TFlagManager::smInstance->getFlag(0x10064 + (shineStage - 2));  // 100 coins
        s_stage_shines_max = 11;
        s_stage_blues_max  = 30;
        break;
    }
    case (TGameSequence::AREA_MONTE - 1): {
        u32 shine_flag = 0x10000 + 6 * 10;
        for (u32 i = shine_flag; i < shine_flag + 10; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
        }
        for (u32 i = 0; i < 50; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_collected += TFlagManager::smInstance->getFlag(0x10064 + 6);  // 100 coins
        s_stage_shines_max = 11;
        s_stage_blues_max  = 30;
        break;
    }
    case (TGameSequence::AREA_MARE - 1): {
        u32 shine_flag = 0x10000 + 5 * 10;
        for (u32 i = shine_flag; i < shine_flag + 10; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
        }
        for (u32 i = 0; i < 50; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_collected += TFlagManager::smInstance->getFlag(0x10064 + 5);  // 100 coins
        s_stage_shines_max = 11;
        s_stage_blues_max  = 30;
        break;
    }
    case 9: {  // Corona Mountain
        for (u32 i = 0; i < 50; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_max = 0;
        s_stage_blues_max  = 10;
        break;
    }
    case SME::STAGE_ERTO:
    case SME::STAGE_LIGHTHOUSE:
    case SME::STAGE_WARSHIP:
    case SME::STAGE_LACRIMA: {
        const u32 shine_id = 120 + (shineStage - SME::STAGE_ERTO) * 10;
        for (u32 i = shine_id; i < shine_id + 10; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 20; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(
            207 + (shineStage - SMS_getShineStage__FUc(SME::STAGE_ERTO)));  // 100 coins
        s_stage_shines_max = 11;
        s_stage_blues_max  = 20;
        break;
    }
    case (SME::STAGE_PIANTA_PIT - 2): {
        const u32 shine_id = 160;
        for (u32 i = shine_id; i < shine_id + 6; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 20; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(211);  // 100 coins
        s_stage_shines_max = 7;
        s_stage_blues_max  = 20;
        break;
    }
    case (SME::STAGE_CRUISER - 2): {
        const u32 shine_id = 166;
        for (u32 i = shine_id; i < shine_id + 5; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 10; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(212);  // 100 coins
        s_stage_shines_max = 6;
        s_stage_blues_max  = 10;
        break;
    }
    case (SME::STAGE_MARIO_DREAM - 2):
    case (SME::STAGE_LANCIA - 2):
    case (SME::STAGE_VAPORWAVE - 2):
    case (SME::STAGE_YOSHI_VILLAGE - 2): {
        const u32 shine_id =
            171 + (shineStage - SMS_getShineStage__FUc(SME::STAGE_MARIO_DREAM)) * 4;
        for (u32 i = shine_id; i < shine_id + 4; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 10; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(
            213 + (shineStage - SMS_getShineStage__FUc(SME::STAGE_MARIO_DREAM)));  // 100 coins
        s_stage_shines_max = 5;
        s_stage_blues_max  = 10;
        break;
    }
    case (SME::STAGE_RED_LILY - 2):
    case (SME::STAGE_PEACH_BEACH - 2):
    case (SME::STAGE_SPETTRO_CASINO - 2): {
        const u32 shine_id = 187 + (shineStage - SMS_getShineStage__FUc(SME::STAGE_RED_LILY)) * 4;
        for (u32 i = shine_id; i < shine_id + 4; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 10; ++i) {
            s_stage_blues_collected +=
                TFlagManager::smInstance->getBlueCoinFlag(director->mAreaID, i);
        }
        s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(
            217 + (shineStage - SMS_getShineStage__FUc(SME::STAGE_RED_LILY)));  // 100 coins
        s_stage_shines_max = 5;
        s_stage_blues_max  = 10;
        break;
    }
    case (SME::STAGE_ISLE_DELFINO - 2): {
        const u32 shine_id = 199;
        for (u32 i = shine_id; i < shine_id + 8; ++i) {
            s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        s_stage_shines_collected += TFlagManager::smInstance->getShineFlag(220);  // Train car
        s_stage_shines_max = 9;
        s_stage_blues_max  = 0;
        break;
    }
    }

    s_shine_text_front = new J2DTextBox(gpSystemFont->mFont, "");
    s_shine_text_back  = new J2DTextBox(gpSystemFont->mFont, "");
    s_blue_text_front  = new J2DTextBox(gpSystemFont->mFont, "");
    s_blue_text_back   = new J2DTextBox(gpSystemFont->mFont, "");

    s_shine_text_front->mStrPtr = s_shine_text_buffer;
    s_shine_text_back->mStrPtr  = s_shine_text_buffer;

    s_shine_text_front->mNewlineSize    = 13;
    s_shine_text_front->mCharSizeX      = 13;
    s_shine_text_front->mCharSizeY      = 14;
    s_shine_text_back->mNewlineSize     = 14;
    s_shine_text_back->mCharSizeX       = 13;
    s_shine_text_back->mCharSizeY       = 14;
    s_shine_text_front->mGradientTop    = {220, 220, 140, 255};
    s_shine_text_front->mGradientBottom = {210, 190, 100, 255};
    s_shine_text_back->mGradientTop     = {0, 0, 0, 255};
    s_shine_text_back->mGradientBottom  = {0, 0, 0, 255};

    s_blue_text_front->mNewlineSize    = 13;
    s_blue_text_front->mCharSizeX      = 13;
    s_blue_text_front->mCharSizeY      = 14;
    s_blue_text_back->mNewlineSize     = 14;
    s_blue_text_back->mCharSizeX       = 13;
    s_blue_text_back->mCharSizeY       = 14;
    s_blue_text_front->mGradientTop    = {120, 120, 230, 255};
    s_blue_text_front->mGradientBottom = {100, 80, 210, 255};
    s_blue_text_back->mGradientTop     = {0, 0, 0, 255};
    s_blue_text_back->mGradientBottom  = {0, 0, 0, 255};

    s_blue_text_front->mStrPtr = s_blue_text_buffer;
    s_blue_text_back->mStrPtr  = s_blue_text_buffer;
}

BETTER_SMS_FOR_CALLBACK void updateShineBlueTracker(TMarDirector *director) {
    size_t shine_count = TFlagManager::smInstance->getFlag(0x40000);
    size_t blue_count  = TFlagManager::smInstance->getFlag(0x40001);

    if (shine_count != s_shine_counter) {
        s_stage_shines_collected += (shine_count - s_shine_counter);
        s_shine_counter = shine_count;
    }

    if (blue_count != s_blue_counter) {
        s_stage_blues_collected += (blue_count - s_blue_counter);
        s_blue_counter = blue_count;
    }

    snprintf(s_shine_text_buffer, 64, "Shine Sprites: %lu/%lu", s_stage_shines_collected,
             s_stage_shines_max);
    snprintf(s_blue_text_buffer, 64, "Blue Coins: %lu/%lu", s_stage_blues_collected,
             s_stage_blues_max);
}

BETTER_SMS_FOR_CALLBACK void renderShineBlueTracker(TMarDirector *director,
                                                    const J2DOrthoGraph *graph) {
    if (director->mCurState != TMarDirector::STATE_PAUSE_MENU) {
        return;
    }

    s16 adjust = BetterSMS::getScreenRatioAdjustX();
    s_shine_text_back->draw(20 - adjust + 1, 270 + 1);
    s_shine_text_front->draw(20 - adjust, 270);

    s_blue_text_back->draw(20 - adjust + 1, 290 + 1);
    s_blue_text_front->draw(20 - adjust, 290);
}