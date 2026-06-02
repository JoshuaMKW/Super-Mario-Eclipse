#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Map/MapCollisionData.hxx>
#include <SMS/Player/MarioDraw.hxx>
#include <SMS/rand.h>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>

#include "smparticle.hxx"
#include <BetterSMS/libs/constmath.hxx>

constexpr int PortalAnimationID = 197;

BETTER_SMS_FOR_CALLBACK void initializePortalAnimation(TApplication *app) {
    Player::addAnimationDataEx(PortalAnimationID, "draw_p", false, true);
}

void smPlayerInit(TMario *player) {
    SME::CharacterID charID = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);
    if (charID == SME::CharacterID::SHADOW_MARIO) {
        Player::registerData(player, "SMPData", new SMParticleData);
    }
}

void smParticleInit(JPAResourceManager *manager, const char *poggies, u8 numbah) {
    gpResourceManager->load(poggies, numbah);
    gpResourceManager->load("ms_kgm_move_a.jpa", 426);
    gpResourceManager->load("ms_kgm_move_b.jpa", 427);
    gpResourceManager->load("ms_kgm_change.jpa", 237);
}
SMS_PATCH_BL(0x802b44e8, smParticleInit);

// Returns true if the ray hits the triangle, and outputs the exact distance 't'
static bool rayIntersectsTriangle(const TVec3f &ray_origin, const TVec3f &ray_dir, const TVec3f &v0,
                                  const TVec3f &v1, const TVec3f &v2, f32 &out_t) {
    const f32 EPSILON = 0.00001f;
    TVec3f edge1, edge2, h, s, q;
    f32 a, f, u, v;

    // Find vectors for two edges sharing v0
    edge1.x = v1.x - v0.x;
    edge1.y = v1.y - v0.y;
    edge1.z = v1.z - v0.z;
    edge2.x = v2.x - v0.x;
    edge2.y = v2.y - v0.y;
    edge2.z = v2.z - v0.z;

    // h = ray_dir cross edge2
    h.x = ray_dir.y * edge2.z - ray_dir.z * edge2.y;
    h.y = ray_dir.z * edge2.x - ray_dir.x * edge2.z;
    h.z = ray_dir.x * edge2.y - ray_dir.y * edge2.x;

    // a = edge1 dot h
    a = edge1.x * h.x + edge1.y * h.y + edge1.z * h.z;

    // If 'a' is near zero, the ray is parallel to the triangle.
    if (a > -EPSILON && a < EPSILON)
        return false;

    f = 1.0f / a;

    // s = ray_origin - v0
    s.x = ray_origin.x - v0.x;
    s.y = ray_origin.y - v0.y;
    s.z = ray_origin.z - v0.z;

    // Calculate U parameter and test bounds (EARLY OUT 1)
    u = f * (s.x * h.x + s.y * h.y + s.z * h.z);
    if (u < 0.0f || u > 1.0f)
        return false;

    // q = s cross edge1
    q.x = s.y * edge1.z - s.z * edge1.y;
    q.y = s.z * edge1.x - s.x * edge1.z;
    q.z = s.x * edge1.y - s.y * edge1.x;

    // Calculate V parameter and test bounds (EARLY OUT 2)
    v = f * (ray_dir.x * q.x + ray_dir.y * q.y + ray_dir.z * q.z);
    if (v < 0.0f || u + v > 1.0f)
        return false;

    // Calculate t, ray intersects triangle!
    f32 t = f * (edge2.x * q.x + edge2.y * q.y + edge2.z * q.z);

    // Check if the hit is in front of the camera, not behind it
    if (t > EPSILON) {
        out_t = t;
        return true;
    }
    return false;
}

// Disgusting AI slop because I'm lazy
static bool renderSMParticleAtLookPoint(TMario *player, TVec3f &look_point_out,
                                        TVec3f &look_nrm_out) {
    SME::Player::PlayerState *player_state =
        (SME::Player::PlayerState *)Player::getRegisteredData(player, SME::Player::data_key);

    float screenVLerp = 1.0f - player_state->mPortalScreenVLerp;

    // Only allow portals when toggled or in Y cam
    CPolarSubCamera *camera = gpCamera;
    if (!camera->isLButtonCameraSpecifyMode(camera->mMode)) {
        if (!player_state->mPortalToggle) {
            return false;
        }
    } else {
        screenVLerp = 0.5f;
    }

    const TVec3f ray_origin = camera->mTranslation;

    // Get the base forward vector
    TVec3f forward;
    forward.x = camera->mTargetPos.x - ray_origin.x;
    forward.y = camera->mTargetPos.y - ray_origin.y;
    forward.z = camera->mTargetPos.z - ray_origin.z;
    PSVECNormalize(forward, forward);

    // Get the global world up vector
    TVec3f world_up = camera->mUpVector;
    PSVECNormalize(world_up, world_up);

    // Calculate true RIGHT vector (Forward cross World Up)
    TVec3f right;
    right.x = forward.y * world_up.z - forward.z * world_up.y;
    right.y = forward.z * world_up.x - forward.x * world_up.z;
    right.z = forward.x * world_up.y - forward.y * world_up.x;
    PSVECNormalize(right, right);

    // Calculate true LOCAL UP vector (Right cross Forward)
    TVec3f cam_up;
    cam_up.x = right.y * forward.z - right.z * forward.y;
    cam_up.y = right.z * forward.x - right.x * forward.z;
    cam_up.z = right.x * forward.y - right.y * forward.x;
    PSVECNormalize(cam_up, cam_up);

    // Map the 0.0->1.0 lerp to a -1.0->1.0 Normalized Device Coordinate
    // Usually, screenVLerp 0.0 is the top (+1.0) and 1.0 is the bottom (-1.0).
    f32 vertical_ndc = 1.0f - (screenVLerp * 2.0f);

    // Calculate vertical spread based on the camera's Field of View.
    // Try to find the real FOV in your camera struct (e.g., camera->mFovy).
    // If you don't have it readily available in CPolarSubCamera, ~45-60
    // degrees is standard. 45 is a safe fallback.
    f32 fov_deg         = gpCamera->mProjectionFovy;
    f32 fov_rad         = fov_deg * (3.14159f / 180.0f);
    f32 vertical_offset = tanf(fov_rad * 0.5f) * vertical_ndc;

    // Build the final un-projected ray direction
    TVec3f ray_direction;
    ray_direction.x = forward.x + (cam_up.x * vertical_offset);
    ray_direction.y = forward.y + (cam_up.y * vertical_offset);
    ray_direction.z = forward.z + (cam_up.z * vertical_offset);
    PSVECNormalize(ray_direction, ray_direction);

    const f32 CAMERA_CLEARANCE =
        150.0f;  // Tweak this to be slightly larger than the camera's collision radius
    TVec3f advanced_origin;
    advanced_origin.x = ray_origin.x + (ray_direction.x * CAMERA_CLEARANCE);
    advanced_origin.y = ray_origin.y + (ray_direction.y * CAMERA_CLEARANCE);
    advanced_origin.z = ray_origin.z + (ray_direction.z * CAMERA_CLEARANCE);

    const f32 STEP_SIZE     = 39.0f;
    const f32 push_out_dist = 15.0f;

    f32 closest_t                       = 99999.0f;
    const TBGCheckData *closest_surface = nullptr;
    f32 current_ray_dist                = 0.0f;

    for (size_t i = 0; i < 150; ++i) {  // 150 * 39 + 150 = 6000 max distance
    next_sample_point:

        TVec3f sample_pos;
        sample_pos.x = advanced_origin.x + (ray_direction.x * current_ray_dist);
        sample_pos.y = advanced_origin.y + (ray_direction.y * current_ray_dist);
        sample_pos.z = advanced_origin.z + (ray_direction.z * current_ray_dist);

        // Check if the sample point is within the radius of each portal
        for (size_t j = 0; j < 2; ++j) {
            const TEMarioPortal *portal = player_state->mPortals[j];

            const TVec3f &portal_pos = portal->mTranslation;
            f32 dist_sq = (sample_pos.x - portal_pos.x) * (sample_pos.x - portal_pos.x) +
                          (sample_pos.y - portal_pos.y) * (sample_pos.y - portal_pos.y) +
                          (sample_pos.z - portal_pos.z) * (sample_pos.z - portal_pos.z);

            if (dist_sq < portal->mReceiveRadius * portal->mReceiveRadius) {
                return false;  // If we're inside a portal's radius, we can't trust the collision
                               // data. Bail out.
            }
        }

        const TBGCheckData *surfaces_to_test[3] = {nullptr, nullptr, nullptr};
        size_t test_count                       = 0;

        // 1. DIRECTIONAL CULLING FOR GROUND/ROOF
        if (ray_direction.y < 0.9f) {
            gpMapCollisionData->checkGround(sample_pos.x, sample_pos.y + STEP_SIZE, sample_pos.z, 0,
                                            &surfaces_to_test[test_count]);
            if (surfaces_to_test[test_count])
                test_count++;
        }

        if (ray_direction.y > -0.9f) {
            gpMapCollisionData->checkRoof(sample_pos.x, sample_pos.y - STEP_SIZE, sample_pos.z, 0,
                                          &surfaces_to_test[test_count]);
            if (surfaces_to_test[test_count])
                test_count++;
        }

        // 2. WALL QUERY
        TBGWallCheckRecord record;
        record.mPosition    = sample_pos;
        record.mCollideMax  = 1;
        record.mIgnoreFlags = 0;
        record.mRadius      = STEP_SIZE;  // Sweep radius matches step size

        if (gpMapCollisionData->checkWalls(&record) > 0) {
            surfaces_to_test[test_count] = record.mWalls[0];
            test_count++;
        }

        // 3. FAST MÖLLER-TRUMBORE INTERSECTION
        for (size_t j = 0; j < test_count; ++j) {
            const TBGCheckData *surf = surfaces_to_test[j];
            f32 hit_t                = 0.0f;

            if (PSVECDotProduct(surf->mNormal, ray_direction) >= 0.0f) {
                continue;  // Skip back-facing surfaces to avoid hitting the inside of walls
            }

            if (rayIntersectsTriangle(advanced_origin, ray_direction, surf->mVertices[0],
                                      surf->mVertices[1], surf->mVertices[2], hit_t)) {

                // If this is the closest hit we've seen so far, save it
                if (hit_t < closest_t) {
                    closest_t       = hit_t;
                    closest_surface = surf;
                }
            }
        }

        // EARLY OUT: If we found a hit, AND the hit happened inside or behind our current
        // sweep sphere, we don't need to keep marching forward. We found the closest wall.
        if (closest_surface && closest_t <= current_ray_dist + STEP_SIZE) {
            break;
        }

        current_ray_dist += STEP_SIZE;
    }

    // ==========================================
    // 4. FINALIZE POINT
    // ==========================================
    if (closest_surface) {
        look_nrm_out = closest_surface->mNormal;

        // Calculate exact point based on closest_t
        look_point_out.x =
            advanced_origin.x + (ray_direction.x * closest_t) + (look_nrm_out.x * push_out_dist);
        look_point_out.y =
            advanced_origin.y + (ray_direction.y * closest_t) + (look_nrm_out.y * push_out_dist);
        look_point_out.z =
            advanced_origin.z + (ray_direction.z * closest_t) + (look_nrm_out.z * push_out_dist);
        return true;
    }

    return false;
}

static bool s_is_valid_look_point = false;
static TVec3f s_look_point, s_look_nrm;

static char s_look_point_thread_stack[0x2000];
static OSThread s_look_point_thread;

static OSMutex s_look_point_mutex;

static OSMessage s_look_point_msg;
static OSMessageQueue s_look_point_msg_queue;

static void *computeLookPointForEMarioPortal(void *param) {
    TMario *player = (TMario *)param;
    while (true) {
        OSMessage dummy;
        OSReceiveMessage(&s_look_point_msg_queue, &dummy, OS_MESSAGE_BLOCK);

        TVec3f look_point, look_nrm;
        bool is_valid = renderSMParticleAtLookPoint(player, look_point, look_nrm);

        OSLockMutex(&s_look_point_mutex);

        s_is_valid_look_point = is_valid;
        if (s_is_valid_look_point) {
            s_look_point = look_point;
            s_look_nrm   = look_nrm;
        } else {
            s_look_point = TVec3f::zero();
            s_look_nrm   = TVec3f::zero();
        }

        OSUnlockMutex(&s_look_point_mutex);
    }
    return nullptr;
}

static J2DPicture *s_reticle_picture        = nullptr;
static J2DPicture *s_brush_picture          = nullptr;
static J2DPicture *s_brush_disabled_picture = nullptr;
static J2DPicture *s_x_button_picture       = nullptr;
static J2DPicture *s_back_picture           = nullptr;
static J2DPane *s_brush_card_pane           = nullptr;

static int s_x_button_counter = 0;

BETTER_SMS_FOR_CALLBACK void createLookPointThreadOnPlayerInit(TMario *player, bool isMario) {
    if (!isMario) {
        return;
    }

    OSInitMutex(&s_look_point_mutex);
    OSInitMessageQueue(&s_look_point_msg_queue, &s_look_point_msg, 1);
    OSCreateThread(&s_look_point_thread, computeLookPointForEMarioPortal, player,
                   s_look_point_thread_stack + sizeof(s_look_point_thread_stack),
                   sizeof(s_look_point_thread_stack), (OSPriority)22, OS_THREAD_ATTR_DETACH);
    OSResumeThread(&s_look_point_thread);

    s_x_button_counter = 0;
}

BETTER_SMS_FOR_CALLBACK void killLookPointThreadOnStageExit(TApplication *app) {
    OSCancelThread(&s_look_point_thread);

    s_reticle_picture        = nullptr;
    s_brush_picture          = nullptr;
    s_brush_disabled_picture = nullptr;
    s_x_button_picture       = nullptr;
    s_back_picture           = nullptr;
    s_brush_card_pane        = nullptr;
}

static int s_wakeup_thread_counter = 0;
static JUTRect s_brush_card_rect;

void doSMParticle(TMario *player, bool isMario) {
    if (!isMario) {
        return;
    }

    J2DScreen *the_screen = gpMarDirector->mGCConsole->mMainScreen;

    SME::CharacterID charID = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);
    if (charID != SME::CharacterID::SHADOW_MARIO) {
        ((J2DPicture *)the_screen->search('sm_0'))->mIsVisible = false;
        return;
    }

    SME::Player::PlayerState *player_state =
        (SME::Player::PlayerState *)Player::getRegisteredData(player, SME::Player::data_key);

    if (player->mState != TMario::STATE_IDLE) {
        player_state->mPortalCasting   = false;  // Portal casting is a completely visual flag
        player_state->mPortalCastTimer = 0;
    }

    if (!player_state->mPortals[0] || !player_state->mPortals[1]) {
        player_state->mPortals[0] = new TEMarioPortal("__PortalBegin");
        player_state->mPortals[1] = new TEMarioPortal("__PortalEnd");

        player_state->mPortals[0]->initAndRegister("emario_portal");
        player_state->mPortals[1]->initAndRegister("emario_portal");

        player_state->mPortals[0]->linkTo(player_state->mPortals[1]);
        player_state->mPortals[1]->linkTo(player_state->mPortals[0]);
    }

    if (!s_reticle_picture) {
        s_reticle_picture = (J2DPicture *)the_screen->search('s_rt');
        SMS_ASSERT(s_reticle_picture, "Couldn't find reticle picture for portal casting!");

        s_brush_picture = (J2DPicture *)the_screen->search('s_br');
        SMS_ASSERT(s_brush_picture, "Couldn't find brush picture for portal casting!");

        s_brush_disabled_picture = (J2DPicture *)the_screen->search('s_bd');
        SMS_ASSERT(s_brush_disabled_picture,
                   "Couldn't find brush disabled picture for portal casting!");

        s_x_button_picture = (J2DPicture *)the_screen->search('xbsm');
        SMS_ASSERT(s_x_button_picture,
                   "Couldn't find brush disabled picture for portal casting!");

        s_back_picture = (J2DPicture *)the_screen->search('smba');
        SMS_ASSERT(s_back_picture,
                   "Couldn't find brush disabled picture for portal casting!");

        s_brush_disabled_picture->mIsVisible = true;

        {
            s_brush_card_pane = the_screen->search('sm_0');
            s_brush_card_rect = s_brush_card_pane->mRect;

            s_brush_card_pane->mRect.mX1 =
                s_brush_card_rect.mX1 + BetterSMS::getScreenRatioAdjustX();
            s_brush_card_pane->mRect.mX2 =
                s_brush_card_rect.mX2 + BetterSMS::getScreenRatioAdjustX();
        }
    }

    if (gpMarDirector->mCurState != TMarDirector::STATE_NORMAL) {
        s_reticle_picture->mIsVisible = false;
        return;  // Only allow portal casting in normal gameplay, not cutscenes or demos
    }

    if (gpMarDirector->mNextStateA == 1 || gpMarDirector->mNextStateA == 2) {
        s_reticle_picture->mIsVisible = false;
        return;  // Don't allow portal casting if we're talking to NPCs
    }

    OSLockMutex(&s_look_point_mutex);

    const bool is_y_cam = gpCamera->isLButtonCameraSpecifyMode(gpCamera->mMode);

    if (player_state->mPortalToggle) {
        player_state->mPortalScreenVLerp += 0.01f * (player->mController->mButtons.mAnalogR - 0.5f);
    } else {
        player_state->mPortalScreenVLerp = 0.5f;
    }

    player_state->mPortalScreenVLerp = Clamp(player_state->mPortalScreenVLerp, 0.0f, 1.0f);

    if (!is_y_cam) {
        if ((player->mController->mButtons.mFrameInput & TMarioGamePad::X)) {
            player_state->mPortalToggle ^= true;
            player_state->mPortalScreenVLerp = 0.5f;

            if (player_state->mPortalToggle) {
                if (gpMSound->gateCheck(MSD_SE_SY_E3_MENU_CURSOR2)) {
                    JAISound *sound = MSoundSE::startSoundSystemSE(MSD_SE_SY_E3_MENU_CURSOR2, 0, nullptr, 0);
                    if (sound) {
                        sound->setVolume(2.0f, 0, 0);
                    }
                }
            } else {
                if (gpMSound->gateCheck(MSD_SE_SY_E3_MENU_CURSOR1)) {
                    JAISound *sound =
                        MSoundSE::startSoundSystemSE(MSD_SE_SY_E3_MENU_CURSOR1, 0, nullptr, 0);
                    if (sound) {
                        sound->setVolume(2.0f, 0, 0);
                    }
                }
            }
        }
    }

    // const f32 screen_height       = SMSGetGameRenderHeight__Fv();
    const f32 screen_height = (f32)(the_screen->mRect.mY2 - the_screen->mRect.mY1);  // Usually 480
    const f32 efb_height =
        SMSGetGameRenderHeight__Fv();  // The true 3D render height of Super Mario Sunshine

    if (is_y_cam) {
        player_state->mPortalScreenVLerp = 0.5f;

        s_reticle_picture->mIsVisible        = true;
        s_brush_picture->mIsVisible          = true;
        s_brush_disabled_picture->mIsVisible = false;

        int reticle_width  = s_reticle_picture->mRect.mX2 - s_reticle_picture->mRect.mX1;
        int reticle_height = s_reticle_picture->mRect.mY2 - s_reticle_picture->mRect.mY1;

        int screen_x = ((the_screen->mRect.mX2 - the_screen->mRect.mX1) - reticle_width) / 2;

        // Anchor at the center of the UI
        int screen_y = screen_height * 0.5f;
        screen_y -= reticle_height / 2;  // Center the PNG vertically

        s_reticle_picture->mRect.move(screen_x, screen_y);

        if (!s_is_valid_look_point) {
            s_reticle_picture->mColorMask =
                JUtility::TColor(255, 100, 0, 255);  // Red tint if we don't have a valid point
        } else {
            s_reticle_picture->mColorMask =
                JUtility::TColor(255, 255, 255, 255);  // Red tint if we don't have a valid point
        }
    } else if (player_state->mPortalToggle) {
        s_reticle_picture->mIsVisible        = true;
        s_brush_picture->mIsVisible          = true;
        s_brush_disabled_picture->mIsVisible = false;

        int reticle_width  = s_reticle_picture->mRect.mX2 - s_reticle_picture->mRect.mX1;
        int reticle_height = s_reticle_picture->mRect.mY2 - s_reticle_picture->mRect.mY1;

        int screen_x = ((the_screen->mRect.mX2 - the_screen->mRect.mX1) - reticle_width) / 2;

        f32 vertical_ndc = (player_state->mPortalScreenVLerp * 2.0f) - 1.0f;

        // --- THE FIX ---
        // 1. Find the physical center of the screen (keeps your reticle perfectly centered)
        f32 center_y = screen_height * 0.5f;

        // 2. Set the max travel distance to the 3D bounds (restricts the overshoot)
        f32 movement_amplitude = efb_height * 0.5f;

        // 3. Start at the center, and move by the scaled amplitude
        int screen_y = center_y - (vertical_ndc * movement_amplitude);

        // 4. Offset the pivot so the image centers correctly on the coordinate
        screen_y -= reticle_height / 2;

        s_reticle_picture->mRect.move(screen_x, screen_y);

        if (!s_is_valid_look_point) {
            s_reticle_picture->mColorMask =
                JUtility::TColor(255, 100, 0, 255);  // Red tint if we don't have a valid point
        } else {
            s_reticle_picture->mColorMask =
                JUtility::TColor(255, 255, 255, 255);  // Red tint if we don't have a valid point
        }
    } else {
        s_reticle_picture->mIsVisible        = false;
        s_brush_picture->mIsVisible          = false;
        s_brush_disabled_picture->mIsVisible = true;
    }

    if (player_state->mPortalTimer > 0) {
        player_state->mPortalTimer -= 1;
    }

    if (player_state->mPortalCasting) {
        if (player_state->mPortalCastTimer == 0 && gpMSound->gateCheck(MSD_SE_MA_ROLL_B)) {
            MSoundSE::startSoundActor(MSD_SE_MA_ROLL_B, player->mTranslation, 0, nullptr, 0, 4);
        }

        if (player_state->mPortalCastTimer == 30 && gpMSound->gateCheck(MSD_SE_MA_ROLL_D)) {
            MSoundSE::startSoundActor(MSD_SE_MA_ROLL_D, player->mTranslation, 0, nullptr, 0, 4);
        }

        if (player_state->mPortalCastTimer == 70 && gpMSound->gateCheck(MSD_SE_MA_ROLL_F)) {
            MSoundSE::startSoundActor(MSD_SE_MA_ROLL_F, player->mTranslation, 0, nullptr, 0, 4);
        }

        if (player_state->mPortalCastTimer == 110 && gpMSound->gateCheck(MSD_SE_MA_ROLL_C)) {
            MSoundSE::startSoundActor(MSD_SE_MA_ROLL_C, player->mTranslation, 0, nullptr, 0, 4);
        }

        player_state->mPortalCastTimer++;
    }

    s_x_button_counter++;

    if (s_x_button_picture->mIsVisible) {
        if (s_x_button_counter > 180) {
            s_x_button_picture->mIsVisible = false;
            s_x_button_counter             = 0;
        }
    } else {
        if (s_x_button_counter > 60) {
            s_x_button_picture->mIsVisible = true;
            s_x_button_counter = 0;
        }
    }

    if (s_is_valid_look_point) {
        if ((s_wakeup_thread_counter % 9) == 0) {
            if (JPABaseEmitter *emitter =
                    gpMarioParticleManager->emit(237, &s_look_point, 0, nullptr)) {
                emitter->mSize1 = {0.1f, 0.1f, 0.1f};
            }
        }

        if ((player->mController->mButtons.mFrameInput & TMarioGamePad::R) == TMarioGamePad::R) {
            TEMarioPortal *portal = player_state->mPortals[player_state->mWhichPortal];
            if (portal) {
                portal->mTranslation = s_look_point;
                portal->mRotation.y  = atan2f(player->mTranslation.x - s_look_point.x,
                                              player->mTranslation.z - s_look_point.z);
                portal->closePortal();
                portal->openPortal(s_look_point, s_look_nrm);
            }
            player_state->mWhichPortal       = player_state->mWhichPortal == 0 ? 1 : 0;
            player_state->mPortalScreenVLerp = 0.5f;
            player_state->mPortalCasting     = true;
            player_state->mPortalCastTimer   = 0;
        }
    }
    OSUnlockMutex(&s_look_point_mutex);

    if ((s_wakeup_thread_counter++ % 3) == 0) {
        OSSendMessage(&s_look_point_msg_queue, (OSMessage)1, OS_MESSAGE_NOBLOCK);
    }

    SMParticleData *data = (SMParticleData *)Player::getRegisteredData(player, "SMPData");

    const TVec3f yOffset  = {0.0f, 50.0f, 0.0f};
    data->mParticleOffset = player->mTranslation + yOffset;

    bool mIsLongJump = (player->mState == 0x2000880) && (player->mAnimationID == 246) &&
                       !player->mFludd->isEmitting();

    if (mIsLongJump) {
        data->mParticle1 =
            gpMarioParticleManager->emitAndBindToPosPtr(426, &data->mParticleOffset, 1, nullptr);
        data->mParticle2 =
            gpMarioParticleManager->emitAndBindToPosPtr(427, &data->mParticleOffset, 1, nullptr);
        player->mAttributes.mIsInvisible = true;
    } else if (!mIsLongJump) {
        player->mAttributes.mIsInvisible = false;
    }
    if (!data->mLastLongJump && mIsLongJump) {  // Start
        gpMarioParticleManager->emit(237, &player->mTranslation, 0, nullptr);
        JAISound *sound = MSoundSE::startSoundActor(MSD_SE_MA_BOTTLE_WIND, player->mTranslation, 0,
                                                    nullptr, 0x0, 4);
        if (sound) {
            sound->setPitch(0.9f + 0.2f * ((f32)rand() / 32767.0f), 3, 0);
            sound->setTempoProportion(1.5f, 3);
        }
    }
    if (data->mLastLongJump && !mIsLongJump) {  // End
        gpMarioParticleManager->emit(237, &player->mTranslation, 0, nullptr);
    }
    data->mLastLongJump = mIsLongJump;
}

// Shadow Mario does a portal cast animation only when idle, and it's cancellable
static void setIdleAnimationBasedOnPortalCast(TMario *player, int animID, f32 animSpeed) {
    SME::Player::PlayerState *player_state =
        (SME::Player::PlayerState *)Player::getRegisteredData(player, SME::Player::data_key);

    if (player->mAnimationID == PortalAnimationID) {
        if (player->mModelData->mFrameCtrl->mAnimFlags.mIsAnmDead) {
            player_state->mPortalCasting   = false;
            player_state->mPortalCastTimer = 0;
        }
    }

    if (player_state->mPortalCasting) {
        if (player_state->mPortalCastTimer == 0) {
            player->setAnimation(0, 1.0f);
        }
        animID = PortalAnimationID;  // Portal cast animation
    }

    player->setAnimation(animID, animSpeed);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x802660F0, 0, 0, 0), setIdleAnimationBasedOnPortalCast);

BETTER_SMS_FOR_CALLBACK void checkForHideReticleOnPaused(TMarDirector *director,
                                                         const J2DOrthoGraph *graph) {
    if (director->mCurState == TMarDirector::STATE_PAUSE_MENU) {
        if (s_reticle_picture) {
            s_reticle_picture->mIsVisible = false;
        }
    }
}

//static bool hasWaterCardOpen() {
//    TGCConsole2 *gcConsole;
//    SMS_FROM_GPR(31, gcConsole);
//
//    SME::CharacterID charID = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);
//    if (charID == SME::CharacterID::SHADOW_MARIO) {
//        return gcConsole->mIsWaterCard;
//    }
//
//    if (gpMarioAddress->mYoshi->mState != TYoshi::State::MOUNTED &&
//        !gpMarioAddress->mAttributes.mHasFludd && !gcConsole->mWaterCardFalling &&
//        gcConsole->mIsWaterCard)
//        startDisappearTank__11TGCConsole2Fv(gcConsole);
//    else if (gpMarioAddress->mYoshi->mState == TYoshi::State::MOUNTED)
//        gpMarioAddress->mAttributes.mHasFludd = true;
//
//    return gcConsole->mIsWaterCard;
//}
//SMS_PATCH_BL(SMS_PORT_REGION(0x8014206C, 0x80136C80, 0, 0), hasWaterCardOpen);
//SMS_WRITE_32(SMS_PORT_REGION(0x80142070, 0x80136C84, 0, 0), 0x28030000);