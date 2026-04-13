#include "object/emario_portal.hxx"

#include <BetterSMS/libs/constmath.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>

#include <SMS/Camera/PolarSubCamera.hxx>

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

static TVec3f MapVectorThroughPortal(const TVec3f &v_in, const TVec3f &n1, const TVec3f &n2) {
    // 1. Flip the vector along the entry normal (N1)
    f32 dot_v_n1 = v_in.x * n1.x + v_in.y * n1.y + v_in.z * n1.z;
    TVec3f v_flipped;
    v_flipped.x = v_in.x - 2.0f * dot_v_n1 * n1.x;
    v_flipped.y = v_in.y - 2.0f * dot_v_n1 * n1.y;
    v_flipped.z = v_in.z - 2.0f * dot_v_n1 * n1.z;

    // 2. Rotate the flipped vector from N1 to N2 using Rodrigues' formula
    f32 c = n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;

    // Edge Case A: Normals are exactly opposite (e.g., floor to ceiling)
    if (c < -0.999f) {
        TVec3f axis = {1.0f, 0.0f, 0.0f};
        if (fabs(n1.x) > 0.9f)
            axis = {0.0f, 1.0f, 0.0f};

        f32 dot_v_axis = v_flipped.x * axis.x + v_flipped.y * axis.y + v_flipped.z * axis.z;
        TVec3f v_out;
        v_out.x = 2.0f * dot_v_axis * axis.x - v_flipped.x;
        v_out.y = 2.0f * dot_v_axis * axis.y - v_flipped.y;
        v_out.z = 2.0f * dot_v_axis * axis.z - v_flipped.z;
        return v_out;
    }

    // Edge Case B: Normals are exactly identical
    if (c > 0.999f)
        return v_flipped;

    // Standard Case: Calculate rotation axis via cross product
    TVec3f cross_n;
    cross_n.x = n1.y * n2.z - n1.z * n2.y;
    cross_n.y = n1.z * n2.x - n1.x * n2.z;
    cross_n.z = n1.x * n2.y - n1.y * n2.x;

    TVec3f cxv;
    cxv.x = cross_n.y * v_flipped.z - cross_n.z * v_flipped.y;
    cxv.y = cross_n.z * v_flipped.x - cross_n.x * v_flipped.z;
    cxv.z = cross_n.x * v_flipped.y - cross_n.y * v_flipped.x;

    TVec3f cxcxv;
    cxcxv.x = cross_n.y * cxv.z - cross_n.z * cxv.y;
    cxcxv.y = cross_n.z * cxv.x - cross_n.x * cxv.z;
    cxcxv.z = cross_n.x * cxv.y - cross_n.y * cxv.x;

    f32 factor = 1.0f / (1.0f + c);

    TVec3f v_out;
    v_out.x = v_flipped.x + cxv.x + cxcxv.x * factor;
    v_out.y = v_flipped.y + cxv.y + cxcxv.y * factor;
    v_out.z = v_flipped.z + cxv.z + cxcxv.z * factor;

    return v_out;
}

TEMarioPortal::TEMarioPortal(const char *name) : TMapObjGeneral(name) {
    m_is_closed  = true;
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

    mActorData->setBtk("emario_portal.btk");
    mModelLoadFlags &= ~0x100;

    makeObjAppeared();

    if (gpMSound->gateCheck(MSD_SE_MA_WARP_EX)) {
        JAISound *sound = MSoundSE::startSoundActor(
            MSD_SE_MA_WARP_EX, mTranslation, 0, nullptr, 0, 4);
        if (sound) {
            //sound->setVolume(0.2f, 0, 0);
            sound->setPitch(0.5f, 3, 0);
            sound->setTempoProportion(1.5f, 0);
        }
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

    //mActorData->

    m_init_scale = mScale;
    m_target_scale = {0.0f, 0.0f, 0.0f};
    m_interp       = 0.0f;

    m_is_closing = true;
}

void TEMarioPortal::control() {
    if (!m_is_closed) {
        if (gpMSound->gateCheck(MSD_SE_OBJ_QUAKE)) {
            JAISound *sound = MSoundSE::startSoundActor(
                MSD_SE_OBJ_QUAKE, reinterpret_cast<Vec *>(&mTranslation), 0, nullptr, 0, 4);
            if (sound) {
                sound->setVolume(0.2f, 0, 0);
                sound->setPitch(1.5f, 3, 0);
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
            m_interp     = 1.0f;
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
            m_interp     = 1.0f;
        }
    }
    
    if (m_linked_portal->m_is_closed) {
        return;  // Don't check for collisions if the linked portal is closed
    }

    for (size_t i = 0; i < mNumObjs; ++i) {
        if (mCollidingObjs[i]->mObjectID == OBJECT_ID_MARIO) {
            transportPlayer(reinterpret_cast<TMario *>(mCollidingObjs[i]));
        } else if ((mCollidingObjs[i]->mObjectID & 0x18000000) == 0) {
            transportActor(reinterpret_cast<TLiveActor *>(mCollidingObjs[i]));
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

void TEMarioPortal::transportActor(TLiveActor *actor) {
    // Get Mario's velocity
    const TVec3f &actor_vel = actor->mSpeed;

    // Check if Mario's movement is towards the portal
    f32 approach_dot =
        actor_vel.x * m_look_nrm.x + actor_vel.y * m_look_nrm.y + actor_vel.z * m_look_nrm.z;

    const TVec3f &actor_pos = actor->mTranslation;

    // 1. Vector from Portal Center to Mario
    const TVec3f v = actor_pos - mTranslation;

    // 2. Thickness Check (Dot product with normal)
    // How far is Mario "above" or "below" the face of the portal?
    f32 dist_along_normal = v.x * m_look_nrm.x + v.y * m_look_nrm.y + v.z * m_look_nrm.z;

    const f32 BASE_THICKNESS = 50.0f;  // Total base thickness
    f32 front_threshold      = BASE_THICKNESS * 0.5f;
    f32 back_threshold       = -(BASE_THICKNESS * 0.5f);

    // 1. Forward Prediction (Scale thickness based on speed)
    // If approach_dot is negative, Mario is moving INTO the portal.
    // We stretch the front threshold outward to catch his previous frame's position.
    if (approach_dot < 0.0f) {
        front_threshold -= approach_dot * 4.0f;  // Subtracting a negative adds to the threshold
    } else {
        back_threshold +=
            approach_dot * 4.0f;  // If Mario is moving out, stretch the back threshold
    }

    // 2. The Dynamic Thickness Check
    if (dist_along_normal > front_threshold || dist_along_normal < back_threshold) {
        return;
    }

    // 3. Radius Check (Pythagorean theorem)
    // Total squared distance to Mario
    f32 total_dist_sq = v.x * v.x + v.y * v.y + v.z * v.z;

    // Horizontal distance squared = Total distance squared - Vertical distance squared
    f32 radial_dist_sq = total_dist_sq - (dist_along_normal * dist_along_normal);

    // Check if Mario is inside the circle
    if (radial_dist_sq > mReceiveRadius * mReceiveRadius) {
        return;
    }

    // 1. Map actor's FULL 3D velocity vector through the portal
    TVec3f mapped_vel =
        MapVectorThroughPortal(actor->mSpeed, m_look_nrm, m_linked_portal->m_look_nrm);

    actor->mSpeed = mapped_vel;

    // 3. Map actor's visual 3D rotation
    f32 current_pitch_rad = angleToRadians(actor->mRotation.x);
    f32 current_yaw_rad   = angleToRadians(actor->mRotation.y);

    TVec3f forward_in;
    f32 cos_pitch = cosf(current_pitch_rad);
    forward_in.x  = sinf(current_yaw_rad) * cos_pitch;
    forward_in.y  = -sinf(current_pitch_rad);  // Negative because looking UP is usually -X rotation
    forward_in.z  = cosf(current_yaw_rad) * cos_pitch;

    TVec3f forward_out =
        MapVectorThroughPortal(forward_in, m_look_nrm, m_linked_portal->m_look_nrm);

    // 4. Determine final facing trajectory
    TVec3f final_forward;
    f32 speed_sq =
        mapped_vel.x * mapped_vel.x + mapped_vel.y * mapped_vel.y + mapped_vel.z * mapped_vel.z;

    if (speed_sq > 0.01f) {
        // If moving, force his rotation to perfectly match his new physical trajectory
        final_forward = mapped_vel;
    } else {
        // If stationary, use the mapped visual forward vector
        final_forward = forward_out;
    }

    // 5. Convert the final 3D vector back into Pitch and Yaw Euler angles
    f32 horizontal_dist =
        sqrtf(final_forward.x * final_forward.x + final_forward.z * final_forward.z);

    f32 final_yaw_rad   = atan2f(final_forward.x, final_forward.z);
    f32 final_pitch_rad = atan2f(-final_forward.y, horizontal_dist);

    // Assign the new angles to the TVec3f
    //actor->mRotation.x = radiansToAngle(final_pitch_rad);
    actor->mRotation.y = radiansToAngle(final_yaw_rad);
    // actor->mRotation.z remains untouched to prevent unwanted sideways barrel-rolling

    // 6. Push out and teleport
    TVec3f push_out = m_linked_portal->m_look_nrm;
    push_out.scale(80.0f);

    if (gpMSound->gateCheck(MSD_SE_MA_WARP_EX)) {
        JAISound *sound = MSoundSE::startSoundActor(
            MSD_SE_MA_WARP_EX, reinterpret_cast<Vec *>(&actor->mTranslation), 0, nullptr, 0, 4);
        if (sound) {
            sound->setTempoProportion(1.5f, 0);
        }
    }

    actor->mTranslation = m_linked_portal->mTranslation + push_out;
}

void TEMarioPortal::transportPlayer(TMario *player) {
    // Get Mario's velocity
    const TVec3f &player_vel = player->mSpeed;

    // Check if Mario's movement is towards the portal
    f32 approach_dot =
        player_vel.x * m_look_nrm.x + player_vel.y * m_look_nrm.y + player_vel.z * m_look_nrm.z;
    if (approach_dot >= 0) {
        return;
    }

    const TVec3f &player_pos = player->mTranslation;

    // 1. Vector from Portal Center to Mario
    const TVec3f v = player_pos - mTranslation;

    // 2. Thickness Check (Dot product with normal)
    // How far is Mario "above" or "below" the face of the portal?
    f32 dist_along_normal = v.x * m_look_nrm.x + v.y * m_look_nrm.y + v.z * m_look_nrm.z;

    const f32 BASE_THICKNESS = 50.0f;  // Total base thickness
    f32 front_threshold      = BASE_THICKNESS * 0.5f;
    f32 back_threshold       = -(BASE_THICKNESS * 0.5f);

    // 1. Forward Prediction (Scale thickness based on speed)
    // If approach_dot is negative, Mario is moving INTO the portal.
    // We stretch the front threshold outward to catch his previous frame's position.
    if (approach_dot < 0.0f) {
        front_threshold -= approach_dot * 2.0f;  // Subtracting a negative adds to the threshold
    } else {
        back_threshold +=
            approach_dot * 2.0f;  // If Mario is moving out, stretch the back threshold
    }

    // 2. The Dynamic Thickness Check
    if (dist_along_normal > front_threshold || dist_along_normal < back_threshold) {
        return;
    }

    // 3. Radius Check (Pythagorean theorem)
    // Total squared distance to Mario
    f32 total_dist_sq = v.x * v.x + v.y * v.y + v.z * v.z;

    // Horizontal distance squared = Total distance squared - Vertical distance squared
    f32 radial_dist_sq = total_dist_sq - (dist_along_normal * dist_along_normal);

    // Check if Mario is inside the circle
    if (radial_dist_sq > mReceiveRadius * mReceiveRadius) {
        return;
    }

    // 1. Map Mario's FULL 3D velocity vector through the portal
    // This handles gravity becoming forward momentum (and vice versa) automatically
    TVec3f mapped_vel =
        MapVectorThroughPortal(player->mSpeed, m_look_nrm, m_linked_portal->m_look_nrm);

    // 2. Reconstruct SMS Physics variables from the mapped 3D velocity
    // The new vertical speed goes directly back into mSpeed.y
    player->mSpeed.y = mapped_vel.y;

    // The new horizontal speed is the magnitude of the mapped X and Z components
    player->mForwardSpeed = sqrtf(mapped_vel.x * mapped_vel.x + mapped_vel.z * mapped_vel.z);

    // 3. Map Mario's visual rotation (Forward Vector)
    // We do this to ensure he faces the right way even if he enters a portal while standing
    // still
    TVec3f forward_in;
    // ** NOTE: Adjust this line to use whatever macro converts your mRotation.y into Radians **
    f32 current_yaw_rad = angleToRadians(player->mRotation.y);

    forward_in.x = sinf(current_yaw_rad);
    forward_in.y = 0.0f;
    forward_in.z = cosf(current_yaw_rad);

    TVec3f forward_out =
        MapVectorThroughPortal(forward_in, m_look_nrm, m_linked_portal->m_look_nrm);

    // 4. Determine final facing angle
    f32 final_yaw_rad;
    if (player->mForwardSpeed > 0.1f) {
        // If he is moving, his angle MUST match his trajectory to prevent sideways sliding
        final_yaw_rad = atan2f(mapped_vel.x, mapped_vel.z);
    } else {
        // If stationary, use his mapped visual forward vector
        final_yaw_rad = atan2f(forward_out.x, forward_out.z);
    }

    // Apply the ABSOLUTE new angle (no longer a delta!)
    player->mRotation.y = radiansToAngle(final_yaw_rad);
    player->mAngle.y    = convertAngleFloatToS16(radiansToAngle(final_yaw_rad));

    // 5. Push out and teleport
    TVec3f push_out = m_linked_portal->m_look_nrm;
    push_out.scale(80.0f);

    player->mTranslation = m_linked_portal->mTranslation + push_out;

    TVec3f camera_pos = gpCamera->mTranslation;
    TVec3f look_at    = gpCamera->mTargetPos;

    // 1. Get the offset pointing FROM the target TO the camera
    TVec3f offset_vec = camera_pos - look_at;

    // 2. Map the camera offset perfectly through 3D space using our new helper!
    // This automatically handles Gimbal Lock, floor-to-wall transitions, etc.
    TVec3f new_offset = MapVectorThroughPortal(offset_vec, m_look_nrm, m_linked_portal->m_look_nrm);

    // 3. Set the new Target (Mario's new teleported position)
    Vec new_look_at = {player->mTranslation.x, player->mTranslation.y, player->mTranslation.z};

    // 4. Apply the fully mapped 3D offset to place the camera relative to Mario
    Vec new_camera_pos = {new_look_at.x + new_offset.x,
                          new_look_at.y +
                              new_offset.y,  // We use the newly mapped Y, not the original!
                          new_look_at.z + new_offset.z};

    gpCamera->warpPosAndAt(new_camera_pos, new_look_at);

    if (gpMSound->gateCheck(MSD_SE_MA_WARP_EX)) {
        JAISound *sound = MSoundSE::startSoundActor(
            MSD_SE_MA_WARP_EX, reinterpret_cast<Vec *>(&player->mTranslation), 0, nullptr, 0, 4);
        if (sound) {
            sound->setTempoProportion(1.5f, 0);
        }
    }

    if (player->mState == TMario::STATE_SLAMSTART) {
        player->changePlayerStatus(TMario::STATE_FALL, 0, false);
    }
}

static hit_data emarioPortal_hit_data{.mAttackRadius  = 290.0f,
                                      .mAttackHeight  = 290.0f,
                                      .mReceiveRadius = 290.0f,
                                      .mReceiveHeight = 290.0f};

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
                         .mUnkFlags         = 0x8000 /*0x02130100*/,
                         .mKeyCode          = cexp_calcKeyCode("EMarioPortal")};

// Work around to make indirect texturing on portal not crash the game
SMS_WRITE_32(SMS_PORT_REGION(0x802281F4, 0, 0, 0), 0x60000000);