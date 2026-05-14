#include <Dolphin/types.h>

#include <SMS/GC2D/ConsoleStr.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>

#include "stage.hxx"

static u8 s_current_area = 0;

struct AreaProgressionInfo {
    u8 m_stage_shines_collected = 0;
    u8 m_stage_shines_max       = 0;

    u8 m_stage_blues_collected = 0;
    u8 m_stage_blues_max       = 0;
};

static AreaProgressionInfo s_area_progression_info = {};

static size_t s_shine_counter = 0;
static size_t s_blue_counter  = 0;

static J2DTextBox *s_area_text_front  = nullptr;
static J2DTextBox *s_area_text_back   = nullptr;
static J2DTextBox *s_shine_text_front = nullptr;
static J2DTextBox *s_shine_text_back  = nullptr;
static J2DTextBox *s_blue_text_front  = nullptr;
static J2DTextBox *s_blue_text_back   = nullptr;

static char s_area_text_buffer[64]  = {};
static char s_shine_text_buffer[64] = {};
static char s_blue_text_buffer[64]  = {};

struct ShineInfo {
    u16 m_100_flag;
    u16 m_flag_start;
    u16 m_count;
};

// Enum for cycling direction
enum class CycleDirection { Previous, Next };

static void calcShineBlueTrackerForAreaEpisode(u8 shine_stage, AreaProgressionInfo *out);

static bool isStageVisibleToTracker(TMarDirector *director, u8 stage) {
    if (stage == SMS_getShineStage__FUc(director->mAreaID)) {
        return true;
    }

    AreaProgressionInfo info = {};
    calcShineBlueTrackerForAreaEpisode(stage, &info);

    if (info.m_stage_shines_max == 0 && info.m_stage_blues_max == 0) {
        return false;
    }

    if (info.m_stage_shines_collected == 0 && info.m_stage_blues_collected == 0) {
        return false;
    }

    return true;
}

static u32 getAdjacentTrackerStage(u32 currentStage, CycleDirection direction) {
    static const u32 trackerStages[] = {TGameSequence::AREA_AIRPORT,
                                        TGameSequence::AREA_DOLPIC,
                                        TGameSequence::AREA_BIANCO,
                                        TGameSequence::AREA_RICCO,
                                        TGameSequence::AREA_MAMMA,
                                        TGameSequence::AREA_PINNABEACH,
                                        TGameSequence::AREA_SIRENA,
                                        TGameSequence::AREA_MONTE - 1,
                                        TGameSequence::AREA_MARE - 1,
                                        9,  // Corona Mountain
                                        SME::STAGE_ERTO,
                                        SME::STAGE_LIGHTHOUSE,
                                        SME::STAGE_WARSHIP,
                                        SME::STAGE_LACRIMA,
                                        SME::STAGE_PIANTA_PIT - 2,
                                        SME::STAGE_CRUISER - 2,
                                        SME::STAGE_MARIO_DREAM - 2,
                                        SME::STAGE_LANCIA - 2,
                                        SME::STAGE_VAPORWAVE - 2,
                                        SME::STAGE_YOSHI_VILLAGE - 2,
                                        SME::STAGE_RED_LILY - 2,
                                        SME::STAGE_PEACH_BEACH - 2,
                                        SME::STAGE_SPETTRO_CASINO - 2,
                                        SME::STAGE_ISLE_DELFINO - 2};

    static const int stageCount = sizeof(trackerStages) / sizeof(trackerStages[0]);

    int currentIndex = 0;  // Default to start if unknown stage is passed
    for (int i = 0; i < stageCount; ++i) {
        if (trackerStages[i] == currentStage) {
            currentIndex = i;
            break;
        }
    }

    int targetIndex = currentIndex;
    while (true) {
        if (direction == CycleDirection::Next) {
            targetIndex = (targetIndex + 1) % stageCount;
        } else {
            targetIndex = (targetIndex - 1 + stageCount) % stageCount;
        }

        if (targetIndex == currentIndex) {
            // We've looped through all stages without finding a visible one
            return currentStage;  // Return the original stage
        }

        if (isStageVisibleToTracker(gpMarDirector, trackerStages[targetIndex])) {
            return trackerStages[targetIndex];
        }
    }
}

static void applyStageName(u8 area) {
    void *stage_bmg     = JKRFileLoader::getGlbResource("/common/2d/stagename.bmg");
    const char *message = (const char *)SMSGetMessageData__FPvUl(stage_bmg, area);
    snprintf(s_area_text_buffer, sizeof(s_area_text_buffer), "<  %s  >", message);
    s_area_text_front->setString(s_area_text_buffer);
    s_area_text_back->setString(s_area_text_buffer);
}

static bool getBlueCoinFlag(u8 shine_stage, u32 coin_id) {
    if (shine_stage == TGameSequence::AREA_AIRPORT) {
        shine_stage = TGameSequence::AREA_DOLPIC;
    }

    if (shine_stage >= SME::STAGE_ERTO && shine_stage < SME::STAGE_PEACH_CASTLE) {
        if (shine_stage >= SME::STAGE_UNUSED_73 - 2) {
            shine_stage -= 1;
        }
        shine_stage -= SME::STAGE_ERTO;
        if (coin_id >= 20) {
            coin_id = 0;
        }
        return TFlagManager::smInstance->getBool(shine_stage * 20 + coin_id + 0x1023A);
    }

    if (shine_stage > TGameSequence::AREA_MARE) {
        shine_stage = TGameSequence::AREA_DOLPIC;
    }

    if (coin_id >= 50) {
        coin_id = 0;
    }

    return TFlagManager::smInstance->getBool((shine_stage - 1) * 50 + coin_id + 0x10078);
}

static void calcShineBlueTrackerForAreaEpisode(u8 shine_stage, AreaProgressionInfo *out) {
    out->m_stage_shines_collected = 0;
    out->m_stage_shines_max       = 0;
    out->m_stage_blues_collected  = 0;
    out->m_stage_blues_max        = 0;

    s_shine_counter = TFlagManager::smInstance->getFlag(0x40000);
    s_blue_counter  = TFlagManager::smInstance->getFlag(0x40001);

    switch (shine_stage) {
    case TGameSequence::AREA_AIRPORT: {
        out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(0x10056);
        out->m_stage_shines_max = 1;
        out->m_stage_blues_max  = 0;
        break;
    }
    case TGameSequence::AREA_DOLPIC: {
        // Blue coin shines
        for (u32 i = 0x10046; i <= 0x10055; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
            out->m_stage_shines_max += 1;
        }

        // Blue coin shines
        for (u32 i = 0x1006C; i <= 0x10073; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
            out->m_stage_shines_max += 1;
        }

        // Blue coin shines (Eclipse)
        for (u32 i = 221; i <= 238; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
            out->m_stage_shines_max += 1;
        }

        for (u32 i = 0x10057; i <= 0x10063; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
            out->m_stage_shines_max += 1;
        }

        for (u32 i = 0x10074; i <= 0x10077; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
            out->m_stage_shines_max += 1;
        }

        for (u32 i = 0; i < 50; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }

        out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(0x1006B);   // 100 coins
        out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(239);  // 100 coins
        out->m_stage_shines_max += 2;

        out->m_stage_blues_max = 20;
        break;
    }
    case TGameSequence::AREA_BIANCO:
    case TGameSequence::AREA_RICCO:
    case TGameSequence::AREA_MAMMA:
    case TGameSequence::AREA_PINNABEACH:
    case TGameSequence::AREA_SIRENA: {
        u32 shine_flag = 0x10000 + (shine_stage - 2) * 10;
        for (u32 i = shine_flag; i < shine_flag + 10; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
        }
        for (u32 i = 0; i < 50; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_collected +=
            TFlagManager::smInstance->getFlag(0x10064 + (shine_stage - 2));  // 100 coins
        out->m_stage_shines_max = 11;
        out->m_stage_blues_max  = 30;
        break;
    }
    case (TGameSequence::AREA_MONTE - 1): {
        u32 shine_flag = 0x10000 + 6 * 10;
        for (u32 i = shine_flag; i < shine_flag + 10; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
        }
        for (u32 i = 0; i < 50; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_collected +=
            TFlagManager::smInstance->getFlag(0x10064 + 6);  // 100 coins
        out->m_stage_shines_max = 11;
        out->m_stage_blues_max  = 30;
        break;
    }
    case (TGameSequence::AREA_MARE - 1): {
        u32 shine_flag = 0x10000 + 5 * 10;
        for (u32 i = shine_flag; i < shine_flag + 10; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getFlag(i);
        }
        for (u32 i = 0; i < 50; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_collected +=
            TFlagManager::smInstance->getFlag(0x10064 + 5);  // 100 coins
        out->m_stage_shines_max = 11;
        out->m_stage_blues_max  = 30;
        break;
    }
    case 9: {  // Corona Mountain
        for (u32 i = 0; i < 50; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_max = 0;
        out->m_stage_blues_max  = 10;
        break;
    }
    case SME::STAGE_ERTO:
    case SME::STAGE_LIGHTHOUSE:
    case SME::STAGE_WARSHIP:
    case SME::STAGE_LACRIMA: {
        const u32 shine_id = 120 + (shine_stage - SME::STAGE_ERTO) * 10;
        for (u32 i = shine_id; i < shine_id + 10; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 20; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(
            207 + (shine_stage - SMS_getShineStage__FUc(SME::STAGE_ERTO)));  // 100 coins
        out->m_stage_shines_max = 11;
        out->m_stage_blues_max  = 20;
        break;
    }
    case (SME::STAGE_PIANTA_PIT - 2): {
        const u32 shine_id = 160;
        for (u32 i = shine_id; i < shine_id + 6; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 20; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(211);  // 100 coins
        out->m_stage_shines_max = 7;
        out->m_stage_blues_max  = 20;
        break;
    }
    case (SME::STAGE_CRUISER - 2): {
        const u32 shine_id = 166;
        for (u32 i = shine_id; i < shine_id + 5; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 10; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(212);  // 100 coins
        out->m_stage_shines_max = 6;
        out->m_stage_blues_max  = 10;
        break;
    }
    case (SME::STAGE_MARIO_DREAM - 2):
    case (SME::STAGE_LANCIA - 2):
    case (SME::STAGE_VAPORWAVE - 2):
    case (SME::STAGE_YOSHI_VILLAGE - 2): {
        const u32 shine_id =
            171 + (shine_stage - SMS_getShineStage__FUc(SME::STAGE_MARIO_DREAM)) * 4;
        for (u32 i = shine_id; i < shine_id + 4; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 10; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(
            213 + (shine_stage - SMS_getShineStage__FUc(SME::STAGE_MARIO_DREAM)));  // 100 coins
        out->m_stage_shines_max = 5;
        out->m_stage_blues_max  = 10;
        break;
    }
    case (SME::STAGE_RED_LILY - 2):
    case (SME::STAGE_PEACH_BEACH - 2):
    case (SME::STAGE_SPETTRO_CASINO - 2): {
        const u32 shine_id = 187 + (shine_stage - SMS_getShineStage__FUc(SME::STAGE_RED_LILY)) * 4;
        for (u32 i = shine_id; i < shine_id + 4; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        for (u32 i = 0; i < 10; ++i) {
            out->m_stage_blues_collected += getBlueCoinFlag(shine_stage, i);
        }
        out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(
            217 + (shine_stage - SMS_getShineStage__FUc(SME::STAGE_RED_LILY)));  // 100 coins
        out->m_stage_shines_max = 5;
        out->m_stage_blues_max  = 10;
        break;
    }
    case (SME::STAGE_ISLE_DELFINO - 2): {
        const u32 shine_id = 199;
        for (u32 i = shine_id; i < shine_id + 8; ++i) {
            out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(i);
        }
        out->m_stage_shines_collected += TFlagManager::smInstance->getShineFlag(220);  // Train car
        out->m_stage_shines_max = 9;
        out->m_stage_blues_max  = 0;
        break;
    }
    }
}

BETTER_SMS_FOR_CALLBACK void initializeShineBlueTracker(TMarDirector *director) {
    s_current_area = SMS_getShineStage__FUc(director->mAreaID);
    calcShineBlueTrackerForAreaEpisode(s_current_area, &s_area_progression_info);

    s_area_text_front  = new J2DTextBox(gpSystemFont->mFont, "");
    s_area_text_back   = new J2DTextBox(gpSystemFont->mFont, "");
    s_shine_text_front = new J2DTextBox(gpSystemFont->mFont, "");
    s_shine_text_back  = new J2DTextBox(gpSystemFont->mFont, "");
    s_blue_text_front  = new J2DTextBox(gpSystemFont->mFont, "");
    s_blue_text_back   = new J2DTextBox(gpSystemFont->mFont, "");

    s_area_text_front->mNewlineSize    = 14;
    s_area_text_front->mCharSizeX      = 12;
    s_area_text_front->mCharSizeY      = 14;
    s_area_text_back->mNewlineSize     = 14;
    s_area_text_back->mCharSizeX       = 12;
    s_area_text_back->mCharSizeY       = 14;
    s_area_text_front->mGradientTop    = {200, 60, 160, 255};
    s_area_text_front->mGradientBottom = {160, 50, 140, 255};
    s_area_text_back->mGradientTop     = {0, 0, 0, 255};
    s_area_text_back->mGradientBottom  = {0, 0, 0, 255};

    s_area_text_front->mStrPtr = s_area_text_buffer;
    s_area_text_back->mStrPtr  = s_area_text_buffer;

    s_shine_text_front->mNewlineSize    = 14;
    s_shine_text_front->mCharSizeX      = 12;
    s_shine_text_front->mCharSizeY      = 14;
    s_shine_text_back->mNewlineSize     = 14;
    s_shine_text_back->mCharSizeX       = 12;
    s_shine_text_back->mCharSizeY       = 14;
    s_shine_text_front->mGradientTop    = {220, 220, 140, 255};
    s_shine_text_front->mGradientBottom = {210, 190, 100, 255};
    s_shine_text_back->mGradientTop     = {0, 0, 0, 255};
    s_shine_text_back->mGradientBottom  = {0, 0, 0, 255};

    s_shine_text_front->mStrPtr = s_shine_text_buffer;
    s_shine_text_back->mStrPtr  = s_shine_text_buffer;

    s_blue_text_front->mNewlineSize    = 14;
    s_blue_text_front->mCharSizeX      = 12;
    s_blue_text_front->mCharSizeY      = 14;
    s_blue_text_back->mNewlineSize     = 14;
    s_blue_text_back->mCharSizeX       = 12;
    s_blue_text_back->mCharSizeY       = 14;
    s_blue_text_front->mGradientTop    = {120, 120, 230, 255};
    s_blue_text_front->mGradientBottom = {100, 80, 210, 255};
    s_blue_text_back->mGradientTop     = {0, 0, 0, 255};
    s_blue_text_back->mGradientBottom  = {0, 0, 0, 255};

    s_blue_text_front->mStrPtr = s_blue_text_buffer;
    s_blue_text_back->mStrPtr  = s_blue_text_buffer;

    applyStageName(s_current_area);
}

BETTER_SMS_FOR_CALLBACK void updateShineBlueTracker(TMarDirector *director) {
#if 0
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
#else
    bool is_scrolled = false;
    if ((gpMarioAddress->mController->mButtons.mRapidInput & TMarioGamePad::L)) {
        s_current_area = getAdjacentTrackerStage(s_current_area, CycleDirection::Previous);
        is_scrolled    = true;
    }

    if ((gpMarioAddress->mController->mButtons.mRapidInput & TMarioGamePad::R)) {
        s_current_area = getAdjacentTrackerStage(s_current_area, CycleDirection::Next);
        is_scrolled    = true;
    }

    if (is_scrolled) {
        applyStageName(s_current_area);
        calcShineBlueTrackerForAreaEpisode(s_current_area, &s_area_progression_info);

        snprintf(s_shine_text_buffer, 64, "Shine Sprites: %lu/%lu",
                 (u32)s_area_progression_info.m_stage_shines_collected,
                 (u32)s_area_progression_info.m_stage_shines_max);
        snprintf(s_blue_text_buffer, 64, "Blue Coins: %lu/%lu",
                 (u32)s_area_progression_info.m_stage_blues_collected,
                 (u32)s_area_progression_info.m_stage_blues_max);
    } else if (s_current_area == SMS_getShineStage__FUc(director->mAreaID)) {
        size_t shine_count = TFlagManager::smInstance->getFlag(0x40000);
        size_t blue_count  = TFlagManager::smInstance->getFlag(0x40001);

        if (shine_count != s_shine_counter) {
            s_area_progression_info.m_stage_shines_collected += (shine_count - s_shine_counter);
            s_shine_counter = shine_count;
        }

        if (blue_count != s_blue_counter) {
            s_area_progression_info.m_stage_blues_collected += (blue_count - s_blue_counter);
            s_blue_counter = blue_count;
        }

        snprintf(s_shine_text_buffer, 64, "Shine Sprites: %lu/%lu",
                 (u32)s_area_progression_info.m_stage_shines_collected,
                 (u32)s_area_progression_info.m_stage_shines_max);
        snprintf(s_blue_text_buffer, 64, "Blue Coins: %lu/%lu",
                 (u32)s_area_progression_info.m_stage_blues_collected,
                 (u32)s_area_progression_info.m_stage_blues_max);
    }
#endif
}

BETTER_SMS_FOR_CALLBACK void renderShineBlueTracker(TMarDirector *director,
                                                    const J2DOrthoGraph *graph) {
    if (director->mCurState != TMarDirector::STATE_PAUSE_MENU) {
        return;
    }

    s16 adjust = BetterSMS::getScreenRatioAdjustX();

    s_area_text_back->draw(20 - adjust + 1, 340 + 1);
    s_area_text_front->draw(20 - adjust, 340);

    s_shine_text_back->draw(20 - adjust + 1, 360 + 1);
    s_shine_text_front->draw(20 - adjust, 360);

    s_blue_text_back->draw(20 - adjust + 1, 380 + 1);
    s_blue_text_front->draw(20 - adjust, 380);
}