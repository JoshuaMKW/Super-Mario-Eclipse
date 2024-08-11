#include "camera.hxx"
#include <BetterSMS/sunscript.hxx>
#include <SMS/Enemy/Conductor.hxx>
#include <SMS/NPC/NpcBase.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/raw_fn.hxx>

using namespace BetterSMS;

void evSetFixedCameraState(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(2, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected int argument 1 for evSetFixedCamera!\n");
        return;
    }

    bool isFixed = arg_slice.mValue != 0;

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected int argument 2 for evSetFixedCamera!\n");
        return;
    }

    bool isLerp = arg_slice.mValue != 0;

    SME::setCameraFixedMode(isFixed, isLerp);
}

void evSetFixedCameraPos(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(3, &argc);

    TVec3f new_pos = TVec3f::zero();

    new_pos.z = Spc::Stack::popItem(spc).getDataFloat();
    new_pos.y = Spc::Stack::popItem(spc).getDataFloat();
    new_pos.x = Spc::Stack::popItem(spc).getDataFloat();

    SME::setCameraFixedPos(new_pos);
}

void evSetFixedCameraLookAt(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(3, &argc);

    TVec3f new_look_at = TVec3f::zero();

    new_look_at.z = Spc::Stack::popItem(spc).getDataFloat();
    new_look_at.y = Spc::Stack::popItem(spc).getDataFloat();
    new_look_at.x = Spc::Stack::popItem(spc).getDataFloat();

    SME::setCameraFixedLookAt(new_look_at);
}

void evSetFixedCameraFollowActor(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(1, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected int argument 1 for evSetFixedCameraFollowActor!\n");
        return;
    }

    JDrama::TActor *actor = (JDrama::TActor *)arg_slice.mValue;
    SME::setCameraFixedFollowActor(actor);
}

void evSetFixedCameraFov(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(1, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    SME::setCameraFixedFovy(arg_slice.getDataFloat());
}

void evFadeScreen(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(3, &argc);

    f32 delay_time = Spc::Stack::popItem(spc).getDataFloat();
    f32 fade_time  = Spc::Stack::popItem(spc).getDataFloat();
    u32 request    = Spc::Stack::popItem(spc).getDataInt();

    gpApplication.mFader->startWipe(request, fade_time, delay_time);
}

void evIsFadingScreen(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(0, &argc);

    bool fading = gpApplication.mFader->mFadeStatus != TSMSFader::FADE_OFF &&
                  gpApplication.mFader->mFadeStatus != TSMSFader::FADE_ON;
    Spc::Stack::pushItem(spc, fading, Spc::ValueType::INT);
}