#pragma once

#include "JSU/JSUList.hxx"
#include "JPABaseParticle.hxx"

class JPABaseEmitter : public JSUPtrLink {
public:
    JPABaseEmitter();

    void calc();

    JPABaseParticle *calcCreateParticle();
    void calcCurrentRateTimerStep();
    void calcEmitterGlobalParams();
    void calcKeyFrameAnime();
    bool checkMaxFrame();
    bool checkStartFrame();
    JPABaseParticle *createChildParticle(JPABaseParticle *particle);
    JPABaseParticle *createParticle();
    void deleteAllParticle();
    void doChildParticle();
    void doParticle();
    void drawEmitterCallback();
    f32 getAspect();
    void getEmitterGlobalTranslation(JGeometry::TVec3<f32> &);
    f32 getFovy();
    void loadBaseEmitterBlock(JPABaseParticle * /*JPADataBlock **/);
    void setGlobalRTMatrix(Mtx);
    void setGlobalSRTMatrix(Mtx);

    u32 _00[0x164 / 4];
    JGeometry::TVec3<f32> mSize;
    u32 _180[0xA0 / 4];
};