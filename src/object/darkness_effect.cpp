#include <Dolphin/GX.h>
#include <Dolphin/MTX.h>
#include <Dolphin/types.h>

#include <JSystem/J3D/J3DModel.hxx>
#include <JSystem/J3D/J3DModelLoaderDataBase.hxx>
#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/Enemy/Conductor.hxx>
#include <SMS/M3DUtil/MActor.hxx>
#include <SMS/M3DUtil/MActorKeeper.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Map/BGCheck.hxx>
#include <SMS/MapObj/MapObjBall.hxx>
#include <SMS/MapObj/MapObjInit.hxx>
#include <SMS/Player/Mario.hxx>
#include <SMS/Player/Watergun.hxx>
#include <SMS/raw_fn.hxx>

#include "object/darkness_effect.hxx"
#include <BetterSMS/libs/constmath.hxx>

#include "darkness.hxx"

void TDarknessEffect::load(JSUMemoryInputStream &in) {
    JDrama::TViewObj::load(in);

    TVec3f position;
    f32 scale, layer_scale;
    u8 type, brightness, layer_count;
    JUtility::TColor color;

    in.readData(&position, sizeof(TVec3f));
    in.readData(&scale, sizeof(f32));
    in.readData(&layer_scale, sizeof(f32));
    in.readData(&type, sizeof(u8));
    in.readData(&color, sizeof(JUtility::TColor));
    in.readData(&brightness, sizeof(u8));
    in.readData(&layer_count, sizeof(u8));

    initializeParameters(static_cast<TLightContext::ActiveType>(type), position, layer_count, color,
                         scale, layer_scale, brightness);
}