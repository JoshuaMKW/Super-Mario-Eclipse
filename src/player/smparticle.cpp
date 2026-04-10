#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Map/MapCollisionData.hxx>
#include <SMS/rand.h>
#include <smparticle.hxx>

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
    CPolarSubCamera *camera = gpCamera;
    if (!camera->isLButtonCameraSpecifyMode(camera->mMode))
        return false;

    const TVec3f ray_origin = camera->mTranslation;
    TVec3f ray_direction    = camera->mTargetPos - ray_origin;
    PSVECNormalize(ray_direction, ray_direction);

    // OPTIMIZATION: We can take larger steps now.
    const f32 STEP_SIZE     = 80.0f;
    const f32 push_out_dist = 15.0f;

    f32 closest_t                       = 99999.0f;
    const TBGCheckData *closest_surface = nullptr;
    f32 current_ray_dist                = 0.0f;

    for (size_t i = 0; i < 38; ++i) {  // 38 * 80 = 3040 max distance
        TVec3f sample_pos;
        sample_pos.x = ray_origin.x + (ray_direction.x * current_ray_dist);
        sample_pos.y = ray_origin.y + (ray_direction.y * current_ray_dist);
        sample_pos.z = ray_origin.z + (ray_direction.z * current_ray_dist);

        const TBGCheckData *surfaces_to_test[3] = {nullptr, nullptr, nullptr};
        size_t test_count                       = 0;

        // 1. DIRECTIONAL CULLING FOR GROUND/ROOF
        if (ray_direction.y < 0.0f) {
            gpMapCollisionData->checkGround(sample_pos.x, sample_pos.y + STEP_SIZE, sample_pos.z, 0,
                                            &surfaces_to_test[test_count]);
            if (surfaces_to_test[test_count])
                test_count++;
        } else if (ray_direction.y > 0.0f) {
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

            if (rayIntersectsTriangle(ray_origin, ray_direction, surf->mVertices[0],
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
            ray_origin.x + (ray_direction.x * closest_t) + (look_nrm_out.x * push_out_dist);
        look_point_out.y =
            ray_origin.y + (ray_direction.y * closest_t) + (look_nrm_out.y * push_out_dist);
        look_point_out.z =
            ray_origin.z + (ray_direction.z * closest_t) + (look_nrm_out.z * push_out_dist);

        if (JPABaseEmitter *emitter =
                gpMarioParticleManager->emit(237, &look_point_out, 0, nullptr)) {
            emitter->mSize1 = {0.2f, 0.2f, 0.2f};
        }
        return true;
    }

    return false;
}

void doSMParticle(TMario *player, bool cool) {
    SME::CharacterID charID = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);
    if (charID != SME::CharacterID::SHADOW_MARIO) {
        return;
    }

    SME::Player::PlayerState *player_state =
        (SME::Player::PlayerState *)Player::getRegisteredData(player, SME::Player::data_key);

    if (!player_state->mPortals[0] || !player_state->mPortals[1]) {
        player_state->mPortals[0] = new TEMarioPortal("__PortalBegin");
        player_state->mPortals[1] = new TEMarioPortal("__PortalEnd");

        player_state->mPortals[0]->initAndRegister("emario_portal");
        player_state->mPortals[1]->initAndRegister("emario_portal");

        player_state->mPortals[0]->linkTo(player_state->mPortals[1]);
        player_state->mPortals[1]->linkTo(player_state->mPortals[0]);
    }

    TVec3f look_point, look_nrm;
    if (renderSMParticleAtLookPoint(player, look_point, look_nrm)) {
        if ((player->mController->mButtons.mFrameInput & TMarioGamePad::R) == TMarioGamePad::R) {

            TEMarioPortal *portal = player_state->mPortals[player_state->mWhichPortal];
            if (portal) {
                portal->mTranslation = look_point;
                portal->mRotation.y  = atan2f(player->mTranslation.x - look_point.x,
                                              player->mTranslation.z - look_point.z);
                portal->closePortal();
                portal->openPortal(look_point, look_nrm);
            }
            player_state->mWhichPortal = player_state->mWhichPortal == 0 ? 1 : 0;
        }
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