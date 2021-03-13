#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/sound/JAISound.hxx"

namespace MSoundSESystem
{
    class MSoundSE
    {
        public:
        static void startSoundNpcActor(u32, JGeometry::TVec3<f32> const *, u32, JAISound **, u32,  u8);
        static bool checkSoundArea(u32, JGeometry::TVec3<f32> const &);
        static void startSoundActorWithInfo(u32, JGeometry::TVec3<f32> const *, JGeometry::TVec3<f32> *, f32, u32, u32, JAISound **, u32,  u8); 
        static void startSoundSystemSE(u32, u32, JAISound **, u32);
        static void startSoundActor(u32, JGeometry::TVec3<f32> const *, u32, JAISound **, u32,  u8);
        static u32 getRandomID(u32);
        static void construct();
    };
};