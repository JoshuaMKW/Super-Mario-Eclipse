#pragma once

#include <BetterSMS/stage.hxx>

namespace SME {

    enum Stage {
        STAGE_ERTO              = 61,
        STAGE_LIGHTHOUSE        = 62,
        STAGE_WARSHIP           = 63,
        STAGE_LACRIMA           = 64,
        STAGE_LACRIMA_INSIDE    = 65,
        STAGE_LACRIMA_BACKHOUSE = 66,
        STAGE_PIANTA_PIT        = 67,
        STAGE_CRUISER           = 68,
        STAGE_MARIO_DREAM       = 69,
        STAGE_LANCIA            = 70,
        STAGE_VAPORWAVE         = 71,
        STAGE_YOSHI_VILLAGE     = 72,
        STAGE_UNUSED_73         = 73,
        STAGE_RED_LILY          = 74,
        STAGE_PEACH_BEACH       = 75,
        STAGE_SPETTRO_CASINO    = 76,
        STAGE_PEACH_CASTLE      = 77,
        STAGE_ISLE_DELFINO      = 78,
        STAGE_LIGHTHOUSE_BOSS   = 79,
        STAGE_TUTORIAL          = 80,
        STAGE_ERTO_EX           = 81,
        STAGE_LIGHTHOUSE_EX     = 82,
        STAGE_WARSHIP_EX        = 83,
        STAGE_LACRIMA_EX0       = 84,
        STAGE_CRUISER_EX        = 85,
        STAGE_LANCIA_EX         = 86,
        STAGE_VAPORWAVE_EX      = 87,
        STAGE_YOSHI_EX          = 88,
        STAGE_LACRIMA_EX1       = 89,
        STAGE_RED_LILY_EX       = 90,
        STAGE_PEACH_BEACH_EX    = 91,
        STAGE_SPETTRO_EX        = 92,
        STAGE_PEACH_CASTLE_EX0  = 93,
        STAGE_PEACH_CASTLE_EX1  = 94,
        STAGE_PEACH_CASTLE_EX2  = 95,
        STAGE_PEACH_CASTLE_EX3  = 96,
        STAGE_PEACH_CASTLE_EX4  = 97,
        STAGE_PEACH_CASTLE_EX5  = 98,
        STAGE_PEACH_CASTLE_EX6  = 99,
        STAGE_PEACH_CASTLE_EX7  = 100,
        STAGE_PEACH_CASTLE_EX8  = 101,
        STAGE_PEACH_CASTLE_EX9  = 102,
        STAGE_PEACH_CASTLE_EX10 = 103,
        STAGE_PEACH_CASTLE_EX11 = 104,
        STAGE_PEACH_CASTLE_EX12 = 105,
        STAGE_PEACH_CASTLE_EX13 = 106,
        STAGE_PEACH_CASTLE_EX14 = 107,
        STAGE_PEACH_CASTLE_EX15 = 108,
        STAGE_PEACH_CASTLE_EX16 = 109,
    };

    void setSpawnTransRot(s32 stage, TVec3f &trans, TVec3f &rot, s32 from_stage = -1,
                          s32 from_episode = -1);
    bool getSpawnTransRot(TApplication *app, TVec3f &trans, TVec3f &rot);

}  // namespace SME