#pragma once
#include <JParticle/JPAResourceManager.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <player.hxx>
#include <globals.hxx>

struct SMParticleData {
    JPABaseEmitter *mParticle1 = nullptr;
    JPABaseEmitter *mParticle2 = nullptr;
    TVec3f mParticleOffset     = {0.f, 1.f, 0.f};
    bool mLastLongJump         = false;
};

void smPlayerInit(TMario *player);
void smParticleInit(JPAResourceManager *manager, const char *poggies, u8 numbah);
void doSMParticle(TMario *player, bool cool);
