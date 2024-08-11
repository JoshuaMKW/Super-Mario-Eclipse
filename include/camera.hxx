#pragma once

#include <Dolphin/types.h>
#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/Strategic/LiveActor.hxx>

namespace SME {

    bool isCameraFixedMode();
    void setCameraFixedMode(bool fixed, bool lerped);

    TVec3f getCameraFixedPos();
    void setCameraFixedPos(const TVec3f &pos);

    TVec3f getCameraFixedLookAt();
    void setCameraFixedLookAt(const TVec3f &lookat);

    JDrama::TActor *getCameraFixedFollowActor();
    void setCameraFixedFollowActor(JDrama::TActor *actor);

    f32 getCameraFixedFovy();
    void setCameraFixedFovy(f32 fovy);

}  // namespace SME