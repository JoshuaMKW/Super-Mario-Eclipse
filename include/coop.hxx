#pragma once

#include <JSystem/JGeometry/JGMVec.hxx>

#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/Player/Mario.hxx>

namespace SMSCoop {

    void setCamera(int i);
    CPolarSubCamera *getCameraById(int i);
    int getActivePerspective();
    void setSkinForPlayer(int id, char const *path);
    TMario *getMarioById(int id);
    int getClosestMarioId(TVec3f *position);
    u8 getPlayerId(TMario *mario);
    int getPlayerCount();
    void setActiveMario(int id);

}  // namespace SMSCoop