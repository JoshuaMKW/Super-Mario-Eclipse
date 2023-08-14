#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>
#include <JSystem/JDrama/JDRNameRef.hxx>
#include <JSystem/JGeometry/JGMQuat.hxx>
#include <SMS/MapObj/MapObjBase.hxx>

#include "object/blow_wind_obj.hxx"

TBlowWindMapObj::TBlowWindMapObj(const char *name) : TMapObjBase(name) {}
TBlowWindMapObj::~TBlowWindMapObj() {}

void TBlowWindMapObj::load(JSUMemoryInputStream &stream) {
    stream.read(&mStrength, sizeof(f32));
    stream.read(&mMode, sizeof(TBlowWindMapObj::GradientMode));
}

void TBlowWindMapObj::perform(u32 flags, JDrama::TGraphics *graphics) {
    for (u32 i = 0; i < mNumObjs; ++i) {
        THitActor *actor = mCollidingObjs[i];
        if (!actor)
            continue;

        const f32 distFactor = getDistance(actor->mTranslation) / mScale.y;
        if (distFactor > 1.0f)
            continue;

        f32 speed;
        switch (mMode) {
        case GradientMode::CONSTANT:
            speed = mStrength;
            break;
        case GradientMode::LINEAR:
        default:
            speed = lerp<f32>(mStrength, 0.0f, distFactor);
            break;
        case GradientMode::EXPONENTIAL:
            speed = powf(5.0f, (-distFactor - 0.85f) * mStrength * 1.4f);
            break;
        case GradientMode::INVERSE:
            speed = powf(mStrength - 5.0f, (-distFactor - 0.85f) * mStrength * 1.4f);
            break;
        }

        JGeometry::TQuat4<f32> quat;
        quat.v = mRotation;
        quat.s = 1.0f;

        TVec3f offset;
        quat.rotate(TVec3f{0.0f, 0.0f, 1.0f}, offset);

        actor->mTranslation.add(offset);
    }
}

ObjData blowWindData{.mMdlName  = "blowwind",
                     .mObjectID = 0x40000421,
                     .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                     .mObjKey           = nullptr,          // const_cast<char *>("blowwind"),
                     .mAnimInfo         = nullptr,
                     .mObjCollisionData = nullptr,
                     .mMapCollisionInfo = nullptr,
                     .mSoundInfo        = nullptr,
                     .mPhysicalInfo     = nullptr,
                     .mSinkData         = nullptr,
                     ._28               = nullptr,
                     .mBckMoveData      = nullptr,
                     ._30               = 50.0f,
                     .mUnkFlags         = 0,
                     .mKeyCode          = cexp_calcKeyCode("blowwind")};