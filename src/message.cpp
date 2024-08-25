#include <BetterSMS/libs/boundbox.hxx>
#include <BetterSMS/module.hxx>

#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include "globals.hxx"
#include "message.hxx"
#include "stage.hxx"

static AtomBalloonMessageViewer *s_message_queue[10] = {0};
static size_t s_message_queue_head                   = 0;
static size_t s_message_queue_size                   = 0;
static s32 s_message_duration                        = -1;

void AtomBalloonMessageViewer::pushMessage(AtomBalloonMessageViewer *message) {
    if (s_message_queue_size < 10) {
        s_message_queue[(s_message_queue_head + s_message_queue_size) % 10] = message;
        s_message_queue_size++;
    }
}

AtomBalloonMessageViewer *AtomBalloonMessageViewer::peekMessage() {
    if (s_message_queue_size > 0) {
        return s_message_queue[s_message_queue_head];
    }
    return nullptr;
}

void AtomBalloonMessageViewer::popMessage() {
    if (s_message_queue_size > 0) {
        s_message_queue_head = (s_message_queue_head + 1) % 10;
        s_message_queue_size--;
    }
}

BETTER_SMS_FOR_CALLBACK void resetBalloonMessagePool(TMarDirector *director) {
    s_message_queue_head = 0;
    s_message_queue_size = 0;
    s_message_duration   = -1;
}

BETTER_SMS_FOR_CALLBACK void checkBalloonMessagePool(TMarDirector *director) {
    bool is_console_ready = *(u16 *)((u8 *)director->mGCConsole + 0x3F4) == 0;
    bool is_water_card    = director->mGCConsole->mIsWaterCard;

    AtomBalloonMessageViewer *message = AtomBalloonMessageViewer::peekMessage();
    if (!message || !is_water_card ||
        SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress) == SME::CharacterID::PIANTISSIMO) {
        return;
    }

    if (s_message_duration == -1 && is_console_ready) {
        s_message_duration = 0;
        startAppearBalloon__11TGCConsole2FUlb(director->mGCConsole, message->id(), false);
        return;
    }

    if (!is_console_ready) {
        s_message_duration++;
    }

    if (s_message_duration == message->duration()) {
        startDisappearBalloon__11TGCConsole2FUlb(director->mGCConsole, message->id(), true);
    } else if (is_console_ready) {
        AtomBalloonMessageViewer::popMessage();

        AtomBalloonMessageViewer *next_message = AtomBalloonMessageViewer::peekMessage();
        if (next_message) {
            s_message_duration = 0;
            startAppearBalloon__11TGCConsole2FUlb(director->mGCConsole, next_message->id(), false);
        } else {
            s_message_duration = -1;
        }
    }
}

static AtomBalloonMessageViewer s_erto_message_a(111, 200);
static AtomBalloonMessageViewer s_erto_message_b(112, 200);

static AtomBalloonMessageViewer s_warship_message_c(115, 200);
static AtomBalloonMessageViewer s_warship_message_d(116, 200);
static AtomBalloonMessageViewer s_warship_message_e(117, 200);

static AtomBalloonMessageViewer s_lighthouse_message_a(118, 200);

static AtomBalloonMessageViewer s_lacrima_message_a(119, 200);
static AtomBalloonMessageViewer s_lacrima_message_b(123, 200);
static AtomBalloonMessageViewer s_lacrima_inside_message_a(122, 200);
static AtomBalloonMessageViewer s_lacrima_mafia_message_a(125, 200);

static AtomBalloonMessageViewer s_pianta_pit_message_a(126, 200);
static AtomBalloonMessageViewer s_pianta_pit_message_b(127, 200);

static AtomBalloonMessageViewer s_cruiser_message_a(128, 200);
static AtomBalloonMessageViewer s_cruiser_message_b(129, 200);
static AtomBalloonMessageViewer s_cruiser_message_c(130, 200);

static AtomBalloonMessageViewer s_yoshi_village_message_a(131, 200);
static AtomBalloonMessageViewer s_yoshi_village_message_b(132, 200);

static AtomBalloonMessageViewer s_red_lily_message_a(133, 200);
static AtomBalloonMessageViewer s_red_lily_message_b(134, 200);

static AtomBalloonMessageViewer s_spettro_message_a(135, 200);

static AtomBalloonMessageViewer s_castle_message_a(136, 200);

static f32 getDistance2D(const TVec3f &a, const TVec3f &b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z));
}

static bool isPlayerWithinDisc(const TVec3f &point, f32 radius, f32 height) {
    if (gpMarioAddress->mTranslation.y < point.y ||
        gpMarioAddress->mTranslation.y > point.y + height)
        return false;
    return getDistance2D(gpMarioAddress->mTranslation, point) < radius;
}

BETTER_SMS_FOR_CALLBACK void fluddMessageSubsystemReset(TMarDirector *director) {
    s_erto_message_a.reset();
    s_erto_message_b.reset();
    s_warship_message_c.reset();
    s_warship_message_d.reset();
    s_warship_message_e.reset();
    s_lighthouse_message_a.reset();
    s_lacrima_message_a.reset();
    s_lacrima_message_b.reset();
    s_lacrima_inside_message_a.reset();
    s_lacrima_mafia_message_a.reset();
    s_pianta_pit_message_a.reset();
    s_pianta_pit_message_b.reset();
    s_cruiser_message_a.reset();
    s_cruiser_message_b.reset();
    s_cruiser_message_c.reset();
    s_yoshi_village_message_a.reset();
    s_yoshi_village_message_b.reset();
    s_red_lily_message_a.reset();
    s_red_lily_message_b.reset();
    s_spettro_message_a.reset();
    s_castle_message_a.reset();
}

BETTER_SMS_FOR_CALLBACK void fluddMessageSubsystem(TMarDirector *director) {
    if (director->mCurState != TMarDirector::STATE_NORMAL) {
        return;
    }

    bool is_open_episode = director->mEpisodeID > 0 && director->mEpisodeID < 6;

    const TVec3f &player_pos = gpMarioAddress->mTranslation;
    bool is_player_grounded  = player_pos.y <= gpMarioAddress->mFloorBelow + 4.0f &&
                              gpMarioAddress->mState != TMario::STATE_SHINE_C;
    bool is_player_swimming = (gpMarioAddress->mState & TMario::STATE_WATERBORN);

    switch (director->mAreaID) {
    case SME::STAGE_ERTO: {
        if (is_player_grounded) {
            if (isPlayerWithinDisc(TVec3f(850.0f, 2750.0f, -12700.0f), 2500.0f, 1000.0f)) {
                s_erto_message_a.show();
            } else if (isPlayerWithinDisc(TVec3f(-1580.0f, 14300.0f, -17850.0f), 1800.0f,
                                          1000.0f)) {
                s_erto_message_b.show();
            }
        }
        break;
    }
    case SME::STAGE_WARSHIP: {
        if (is_player_swimming && player_pos.x > 4200.0f && player_pos.y < -2300.0f &&
            player_pos.z > 8900.0f) {
            s_warship_message_c.show();
        } else if (isPlayerWithinDisc(TVec3f(-2700.0f, 400.0f, 4800.0f), 12000.0f, 8000.0f) &&
                   director->mEpisodeID == 0) {
            s_warship_message_d.show();
        } else if (isPlayerWithinDisc(TVec3f(6500.0f, -19300.0f, -3600.0f), 12000.0f, 5000.0f) &&
                   director->mEpisodeID == 7) {
            s_warship_message_e.show();
        }
        break;
    }
    case SME::STAGE_LIGHTHOUSE: {
        if (is_open_episode &&
            isPlayerWithinDisc(TVec3f(11300.0f, 600.0f, 3830.0f), 3600.0f, 1250.0f)) {
            s_lighthouse_message_a.show();
        }
        break;
    }
    case SME::STAGE_LACRIMA: {
        if (director->mEpisodeID == 0 &&
            isPlayerWithinDisc(TVec3f(-1150.0f, 0.0f, 4000.0f), 2500.0f, 3000.0f)) {
            s_lacrima_message_a.show();
        } else if (director->mEpisodeID == 6) {
            s_lacrima_message_b.show();
        }
        break;
    }
    case SME::STAGE_LACRIMA_INSIDE: {
        BoundingBox arcade_bounds = {TVec3f(680.0f, 500.0f, 8850.0f),
                                     TVec3f(500.0f, 500.0f, 1600.0f)};
        if (is_open_episode && arcade_bounds.contains(player_pos)) {
            s_lacrima_inside_message_a.show();
        }
        break;
    }
    case SME::STAGE_LACRIMA_BACKHOUSE: {
        if (director->mEpisodeID == 6) {
            s_lacrima_mafia_message_a.show();
        }
        break;
    }
    case SME::STAGE_PIANTA_PIT: {
        if (isPlayerWithinDisc(TVec3f(1400.0f, 13300.0f, 2000.0f), 700.0f, 1000.0f)) {
            s_pianta_pit_message_a.show();
        } else if (isPlayerWithinDisc(TVec3f(1000.0f, 1500.0f, -15500.0f), 2500.0f, 2000.0f)) {
            s_pianta_pit_message_b.show();
        }
        break;
    }
    case SME::STAGE_CRUISER: {
        if (is_player_swimming &&
            isPlayerWithinDisc(TVec3f(-18400.0f, 5600.0f, -1500.0f), 3000.0f, 1000.0f)) {
            s_cruiser_message_a.show();
        } else if (isPlayerWithinDisc(TVec3f(12500.0f, 5700.0f, -2100.0f), 2000.0f, 4000.0f)) {
            s_cruiser_message_b.show();
        } else if (!TFlagManager::smInstance->getShineFlag(166) &&
                   isPlayerWithinDisc(TVec3f(5000.0f, 1350.0f, 0.0f), 5000.0f, 2000.0f)) {
            s_cruiser_message_c.show();
        }
        break;
    }
    case SME::STAGE_YOSHI_VILLAGE: {
        if (isPlayerWithinDisc(TVec3f(-700.0f, 0.0f, -2000.0f), 4000.0f, 5000.0f)) {
            if (director->mEpisodeID == 0) {
                s_yoshi_village_message_a.show();
            } else {
                s_yoshi_village_message_b.show();
            }
        }
        break;
    }
    case SME::STAGE_RED_LILY: {
        if (isPlayerWithinDisc(TVec3f(250.0f, -600.0f, -1000.0f), 2000.0f, 1000.0f)) {
            s_red_lily_message_a.show();
        }
        break;
    }
    case SME::STAGE_SPETTRO_CASINO: {
        if (isPlayerWithinDisc(TVec3f(0.0f, -1000.0f, 0.0f), 10000.0f, 100000.0f)) {
            s_spettro_message_a.show();
        }
        break;
    }
    case SME::STAGE_PEACH_CASTLE: {
        if (director->mEpisodeID == 3 &&
            isPlayerWithinDisc(TVec3f(0.0f, -22600.0f, 0.0f), 10000.0f, 3000.0f)) {
            s_castle_message_a.show();
        }
        break;
    }
    default:
        break;
    }
}