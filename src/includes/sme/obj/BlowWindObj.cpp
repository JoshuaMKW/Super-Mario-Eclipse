#include "SME.hxx"
#include "BlowWindObj.hxx"

#if 0
TBlowWindObj::TBlowWindObj(const char *name) : TMapObjBase(name) {}
TBlowWindObj::~TBlowWindObj() {}

void TBlowWindObj::load(JSUMemoryInputStream &stream) {
  stream.read(&mStrength, sizeof(f32));
  stream.read(&mMode, sizeof(TBlowWindObj::GradientMode));
}

void TBlowWindObj::perform(u32 flags, JDrama::TGraphics *graphics) {
  for (u32 i = 0; i < mNumObjs; ++i) {
    THitActor *actor = mCollidingObjs[i];
    if (!actor)
      continue;

    const f32 distFactor = getDistance(actor->mPosition) / mSize.y;
    if (distFactor > 1.0f)
      continue;

    f32 speed;
    switch (mMode) {
    case GradientMode::CONSTANT:
      speed = mStrength;
      break;
    case GradientMode::LINEAR:
    default:
      speed = Math::lerp<f32>(mStrength, 0.0f, distFactor);
      break;
    case GradientMode::EXPONENTIAL:
      speed = powf(5.0f, (-distFactor - 0.85f) * mStrength);
      break;
    case GradientMode::INVERSE:
      speed = powf(mStrength - 5.0f, (-distFactor - 0.85f) * mStrength);
      break;
    }

    JGeometry::TQuat4<f32> quat;
    quat.x = mRotation.x;
    quat.y = mRotation.y;
    quat.z = mRotation.z;
    quat.w = 1.0f;

    JGeometry::TVec3<f32> offset;
    quat.rotate(JGeometry::TVec3<f32>{0.0f, 0.0f, 1.0f}, offset);

    actor->mPosition.add(offset);
  }
}
#endif