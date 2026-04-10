#include "object/emario_portal.hxx"

#include <BetterSMS/libs/constmath.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>

static TVec3f NormalToEulerAngles(const TVec3f &normal) {
    TVec3f euler = TVec3f::zero();

    // 1. Calculate the horizontal distance (projection onto the XZ plane)
    f32 horizontal_dist = sqrtf(normal.x * normal.x + normal.z * normal.z);

    // 2. Calculate Yaw (Rotation around the Y axis)
    // atan2 safely handles z = 0
    euler.y = atan2f(normal.x, normal.z);

    // 3. Calculate Pitch (Rotation around the X axis)
    // We use -normal.y because looking "up" requires a negative X rotation in SMS
    euler.x = atan2f(-normal.y, horizontal_dist);

    // 4. Calculate Roll (Rotation around the Z axis)
    // Kept at 0 to prevent the particle from spinning on its local axis
    euler.z = 0.0f;

    euler.x = radiansToAngle(euler.x);
    euler.y = radiansToAngle(euler.y);
    euler.z = radiansToAngle(euler.z);

    euler.x += 90.0f;  // Adjust pitch to align the portal correctly

    return euler;
}

TEMarioPortal::TEMarioPortal(const char *name) : TMapObjGeneral(name) {
    m_is_closed  = false;
    m_is_closing = false;
    m_is_opening = false;

    m_init_scale = mScale;
    m_target_scale = mScale;

    m_interp = 0.0f;
}

void TEMarioPortal::openPortal(const TVec3f &at, const TVec3f &look_nrm) {
    mTranslation = at;
    mRotation    = NormalToEulerAngles(look_nrm);
    m_look_nrm   = look_nrm;

    // mActorData->setBck("emario_portal_open");
    // auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    // ctrl->mAnimState = J3DFrameCtrl::ONCE;
    // ctrl->mFrameRate = 1.0f;
    // mModelLoadFlags &= ~0x100;

    makeObjAppeared();

    if (gpMSound->gateCheck(MSD_SE_IT_APPEAR)) {
        MSoundSE::startSoundActor(MSD_SE_IT_APPEAR, reinterpret_cast<Vec *>(&mTranslation), 0,
                                  nullptr, 0, 4);
    }

    m_init_scale   = {0.0f, 0.0f, 0.0f};
    m_target_scale = {1.0f, 1.0f, 1.0f};
    m_interp       = 0.0f;

    m_is_opening = true;
    m_is_closed  = false;
}

void TEMarioPortal::closePortal() {
    makeObjDead();

    // mActorData->setBck("emario_portal_close");
    // auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    // ctrl->mAnimState = J3DFrameCtrl::ONCE;
    // ctrl->mFrameRate = 1.0f;
    // mModelLoadFlags &= ~0x100;

    m_init_scale = mScale;
    m_target_scale = {0.0f, 0.0f, 0.0f};
    m_interp       = 0.0f;

    m_is_closing = true;
}

#define PORTAL_THICKNESS 100.0f

void TEMarioPortal::control() {
    if (!m_is_closed) {
        if (gpMSound->gateCheck(MSD_SE_OBJ_QUAKE)) {
            JAISound *sound = MSoundSE::startSoundActor(
                MSD_SE_OBJ_QUAKE, reinterpret_cast<Vec *>(&mTranslation), 0, nullptr, 0, 4);
            if (sound) {
                sound->setVolume(0.1f, 0, 0);
            }
        }
    }

    if (m_is_closing) {
        float in_out_t = m_interp * m_interp * (3.0f - 2.0f * m_interp);
        mScale.x       = lerp<f32>(m_init_scale.x, m_target_scale.x, in_out_t);
        mScale.y       = lerp<f32>(m_init_scale.y, m_target_scale.y, in_out_t);
        mScale.z       = lerp<f32>(m_init_scale.z, m_target_scale.z, in_out_t);
        m_interp += SMSGetAnmFrameRate() * 0.01f;
        if (m_interp >= 1.0f) {
            m_is_closing = false;
            //m_is_closed  = true;
            m_interp     = 0.0f;
        }
    } else if (m_is_opening) {
        float in_out_t = m_interp * m_interp * (3.0f - 2.0f * m_interp);
        mScale.x       = lerp<f32>(m_init_scale.x, m_target_scale.x, in_out_t);
        mScale.y       = lerp<f32>(m_init_scale.y, m_target_scale.y, in_out_t);
        mScale.z       = lerp<f32>(m_init_scale.z, m_target_scale.z, in_out_t);
        m_interp += SMSGetAnmFrameRate() * 0.01f;
        if (m_interp >= 1.0f) {
            m_is_opening = false;
            m_is_closed  = false;
            m_interp     = 0.0f;
        }
    }
    
    if (m_linked_portal->m_is_closed) {
        OSReport("Linked portal is closed, skipping collision check.\n");
        return;  // Don't check for collisions if the linked portal is closed
    }

    for (size_t i = 0; i < mNumObjs; ++i) {
        if (mCollidingObjs[i]->mObjectID != OBJECT_ID_MARIO) {
            continue;
        }

        // Get player's position
        TMario *player = static_cast<TMario *>(mCollidingObjs[i]);

        // Get Mario's velocity
        const TVec3f &player_vel = player->mSpeed;

        // Check if Mario's movement is towards the portal
        f32 approach_dot =
            player_vel.x * m_look_nrm.x + player_vel.y * m_look_nrm.y + player_vel.z * m_look_nrm.z;
        OSReport("Approach dot product: %f\n", approach_dot);
        if (approach_dot >= 0) {
            continue;  // Mario is moving away from the portal
        }

        const TVec3f &player_pos = player->mTranslation;

        // 1. Vector from Portal Center to Mario
        const TVec3f v = player_pos - mTranslation;

        // 2. Thickness Check (Dot product with normal)
        // How far is Mario "above" or "below" the face of the portal?
        f32 dist_along_normal = v.x * m_look_nrm.x + v.y * m_look_nrm.y + v.z * m_look_nrm.z;

        OSReport("Distance along portal normal: %f\n", dist_along_normal);
        // Check if Mario is within the half-thickness threshold
        if (dist_along_normal > (PORTAL_THICKNESS * 0.5f) ||
            dist_along_normal < -(PORTAL_THICKNESS * 0.5f)) {
            continue;  // Mario is too high or too deep
        }

        // 3. Radius Check (Pythagorean theorem)
        // Total squared distance to Mario
        f32 total_dist_sq = v.x * v.x + v.y * v.y + v.z * v.z;

        // Horizontal distance squared = Total distance squared - Vertical distance squared
        f32 radial_dist_sq = total_dist_sq - (dist_along_normal * dist_along_normal);

            OSReport("Radial distance squared: %f\n", radial_dist_sq);
        OSReport("Portal receive radius squared: %f\n", mReceiveRadius * mReceiveRadius);
        // Check if Mario is inside the circle
        if (radial_dist_sq <= mReceiveRadius * mReceiveRadius) {
            OSReport("Mario entered the portal!\n");
            //  Adjust Mario's velocity to match the linked portal's normal
            TVec3f linked_normal = m_linked_portal->m_look_nrm;
            linked_normal.scale(
                approach_dot);  // Scale by the original approach speed to maintain momentum
            player->mSpeed = linked_normal;

            TVec3f push_out = m_linked_portal->m_look_nrm;
            push_out.scale(50.0f);  // Push Mario slightly out of the linked portal to prevent
                                    // immediate re-collision

            //  Teleport Mario to the linked portal's position
            player->mTranslation = m_linked_portal->mTranslation + push_out;

            if (gpMSound->gateCheck(MSD_SE_MA_WARP)) {
                JAISound *sound = MSoundSE::startSoundActor(MSD_SE_MA_WARP_EX,
                                          reinterpret_cast<Vec *>(&player->mTranslation), 0,
                                          nullptr, 0, 4);
                if (sound) {
                    sound->setTempoProportion(1.5f, 0);
                }
            }
        }
    }
}

// void TEMarioPortal::init() {
//     mLiveManager = manager;
//     mLiveManager->manageActor(this);
//
//     // Initialize the model
//     mActorKeeper              = new TMActorKeeper(mLiveManager, 15);
//     mActorKeeper->mModelFlags = 0x102F0000;
//     mActorData                = mActorKeeper->createMActor("emario_portal.bmd", 0);
//     mActorData->setLightType(1);
//     mActorData->calc();
//     mActorData->viewCalc();
//
//     initHitActor(0x8000011, 10, 0x81000000, 370.0f, 800.0f, 370.0f, 800.0f);
//     initAnmSound();
// }

void TEMarioPortal::initMapObj() {
    TMapObjGeneral::initMapObj();
    closePortal();
    //mActorData->setBtkFromIndex(0);
    //mActorData->setFrameRate(SMSGetAnmFrameRate(), MActor::BTK);
    // if (mCollisionManager) {
    //     mCollisionManager->changeCollision(0);
    //     mCollisionManager->mCurrentMapCollision->setAllActor(this);
    // }
}

bool TEMarioPortal::receiveMessage(THitActor *sender, u32 message) {
    // if (sender->mObjectID == 0x1000002B || sender->mObjectID == 0x4000022E) {
    //     playFragmentAnim();

    //    if (PSVECDistance(gpMarioAddress->mTranslation, mTranslation) < 600.0f) {
    //        // Hurt player
    //        gpMarioAddress->decHP(gpMarioAddress->mHealth);
    //        gpMarioAddress->mAttributes.mIsVisible = true;
    //        gpMarioAddress->mTranslation.y         = mTranslation.y - 200.0f;
    //    }

    //    m_is_hit = true;

    //    return true;
    //}
    OSReport("[EMarioPortal] Received msg %lu for sender 0x%08X\n", message, sender);

    if (TMapObjGeneral::receiveMessage(sender, message)) {
        return true;
    }

    return false;
}

TBGCheckData *TEMarioPortal::getNearestSurface(const TVec3f &at, const TVec3f &look_nrm) {
    return nullptr;
}

TVec3f TEMarioPortal::getRotationFromNormal(const TVec3f &nrm) { return TVec3f(); }

static hit_data emarioPortal_hit_data{.mAttackRadius  = 280.0f,
                                      .mAttackHeight  = 280.0f,
                                      .mReceiveRadius = 280.0f,
                                      .mReceiveHeight = 280.0f};

static obj_hit_info emarioPortal_collision_data{
    ._00 = 1, .mType = 0xDC000000, ._08 = 0, .mHitData = &emarioPortal_hit_data};

ObjData emarioPortalData{.mMdlName          = "emario_portal",
                         .mObjectID         = 0x80000412 /*0x80000FFF*/,
                         .mLiveManagerName  = gLiveManagerName,
                         .mObjKey           = gUnkManagerName,
                         .mAnimInfo         = nullptr,
                         .mObjCollisionData = &emarioPortal_collision_data,
                         .mMapCollisionInfo = nullptr,
                         .mSoundInfo        = nullptr,
                         .mPhysicalInfo     = nullptr,
                         .mSinkData         = nullptr,
                         ._28               = nullptr,
                         .mBckMoveData      = nullptr,
                         ._30               = 50.0f,
                         .mUnkFlags         = 0x0 /*0x02130100*/,
                         .mKeyCode          = cexp_calcKeyCode("EMarioPortal")};