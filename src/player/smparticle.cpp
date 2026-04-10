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


static bool isPointInsideTriangle(const TVec3f &p, const TVec3f &a, const TVec3f &b,
                                  const TVec3f &c) {
    // Calculate edge vectors
    TVec3f v0, v1, v2;
    v0.x = c.x - a.x;
    v0.y = c.y - a.y;
    v0.z = c.z - a.z;
    v1.x = b.x - a.x;
    v1.y = b.y - a.y;
    v1.z = b.z - a.z;
    v2.x = p.x - a.x;
    v2.y = p.y - a.y;
    v2.z = p.z - a.z;

    // Calculate dot products
    const f32 dot00 = v0.x * v0.x + v0.y * v0.y + v0.z * v0.z;
    const f32 dot01 = v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
    const f32 dot02 = v0.x * v2.x + v0.y * v2.y + v0.z * v2.z;
    const f32 dot11 = v1.x * v1.x + v1.y * v1.y + v1.z * v1.z;
    const f32 dot12 = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

    // Calculate barycentric coordinates
    const f32 denom = (dot00 * dot11 - dot01 * dot01);

    // Prevent divide by zero if the triangle is degenerate
    if (denom > -0.0001f && denom < 0.0001f) {
        return false;
    }

    const f32 invDenom = 1.0f / denom;
    const f32 u        = (dot11 * dot02 - dot01 * dot12) * invDenom;
    const f32 v        = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Added a tiny -0.01f epsilon to prevent rays from slipping through cracks
    return (u >= -0.01f) && (v >= -0.01f) && (u + v <= 1.01f);
}

static bool renderSMParticleAtLookPoint(TMario *player, TVec3f &look_point_out) {
    CPolarSubCamera *camera = gpCamera;
    //if (!camera->isLButtonCameraSpecifyMode(*(u32 *)camera->_00)) {
    //    return false;  // It should only render in Y-cam
    //}

    // Cast a ray from the camera to the look point and check if it hits something
    const TVec3f ray_origin = camera->mTranslation;

    TVec3f ray_direction = camera->mTargetPos - ray_origin;
    PSVECNormalize(ray_direction, ray_direction);

    TVec3f ray_dir_step = ray_direction;
    ray_dir_step.scale(50.0f);

    const TBGCheckData *surfaces[4] = {};

    // Check for collision with the stage geometry
    TVec3f sample_pos       = ray_origin;
    const f32 push_out_dist = 15.0f;

    for (size_t i = 0; i < 60; ++i) {
        TVec3f next_pos = sample_pos + ray_dir_step;

        const TBGCheckData *hit_surface = nullptr;

        // ==========================================
        // 1. GROUND CHECK
        // ==========================================
        if (true) {
            const TBGCheckData *groundSurface = nullptr;
            const f32 ground_height           = gpMapCollisionData->checkGround(
                next_pos.x, next_pos.y + 50.0f, next_pos.z, 0, &groundSurface);

            // If the ray dipped below the sloped floor's height at this step
            if (groundSurface && sample_pos.y > ground_height && next_pos.y <= ground_height) {
                hit_surface = groundSurface;
            }
        }

        // ==========================================
        // 2. ROOF CHECK
        // ==========================================
        if (!hit_surface) {
            const TBGCheckData *roofSurface = nullptr;
            const f32 roof_height = gpMapCollisionData->checkRoof(next_pos.x, next_pos.y - 50.0f,
                                                                  next_pos.z, 0, &roofSurface);

            if (roofSurface && sample_pos.y < roof_height && next_pos.y >= roof_height) {
                hit_surface = roofSurface;
            }
        }

        // ==========================================
        // 3. WALL CHECK
        // ==========================================
        if (!hit_surface) {
            TBGWallCheckRecord record;
            record.mPosition    = next_pos;
            record.mCollideMax  = 1;
            record.mIgnoreFlags = 0;
            record.mRadius      = 50.0f;

            if (gpMapCollisionData->checkWalls(&record) > 0) {
                hit_surface = record.mWalls[0];
            }
        }

        // ==========================================
        // 4. EXACT RAY-PLANE INTERSECTION
        // ==========================================
        if (hit_surface) {
            const TVec3f &nrm = hit_surface->mNormal;
            const f32 denom =
                nrm.x * ray_direction.x + nrm.y * ray_direction.y + nrm.z * ray_direction.z;

            // Ensure we are facing the surface
            if (denom < -0.001f) {
                const f32 t = ((hit_surface->mVertices[0].x - ray_origin.x) * nrm.x +
                               (hit_surface->mVertices[0].y - ray_origin.y) * nrm.y +
                               (hit_surface->mVertices[0].z - ray_origin.z) * nrm.z) /
                              denom;

                // Reconstruct the hit point
                TVec3f hit_point = ray_origin;
                hit_point.x += ray_direction.x * t;
                hit_point.y += ray_direction.y * t;
                hit_point.z += ray_direction.z * t;

                // Did we actually hit the triangle, or just its infinite plane?
                if (isPointInsideTriangle(hit_point, hit_surface->mVertices[0],
                                          hit_surface->mVertices[1], hit_surface->mVertices[2])) {

                    // Push out along the normal.
                    look_point_out.x = hit_point.x + (nrm.x * push_out_dist);
                    look_point_out.y = hit_point.y + (nrm.y * push_out_dist);
                    look_point_out.z = hit_point.z + (nrm.z * push_out_dist);

                    if (JPABaseEmitter *emitter =
                            gpMarioParticleManager->emit(237, &look_point_out, 0, nullptr)) {
                        emitter->mSize1 = {0.2f, 0.2f, 0.2f};
                    }
                    return true;
                }
                // If we missed the triangle, do nothing. The loop will continue.
            }
        }

        sample_pos = next_pos;
    }

    return false;
}

void doSMParticle(TMario *player, bool cool) {
    SME::CharacterID charID = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);
    if (charID != SME::CharacterID::SHADOW_MARIO) {
        return;
    }

    TVec3f look_point;
    if (renderSMParticleAtLookPoint(player, look_point)) {
        //OSReport("Look point: (%f, %f, %f)\n", look_point.x, look_point.y, look_point.z);
    }

    SMParticleData *data = (SMParticleData *)Player::getRegisteredData(player, "SMPData");
    SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress) == SME::CharacterID::SHADOW_MARIO;

    const TVec3f yOffset  = {0.0f, 50.0f, 0.0f};
    data->mParticleOffset = player->mTranslation + yOffset;

    bool mIsLongJump = (player->mState == 0x2000880) && (player->mAnimationID == 246) &&
                       !player->mFludd->isEmitting();

    if (mIsLongJump) {
        data->mParticle1 =
            gpMarioParticleManager->emitAndBindToPosPtr(426, &data->mParticleOffset, 1, nullptr);
        data->mParticle2 =
            gpMarioParticleManager->emitAndBindToPosPtr(427, &data->mParticleOffset, 1, nullptr);
        player->mAttributes.mIsVisible = true;  // mIsVisible is a bad name i hate it
    } else if (!mIsLongJump) {
        player->mAttributes.mIsVisible = false;
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