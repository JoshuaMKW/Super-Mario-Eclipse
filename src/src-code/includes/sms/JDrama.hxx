#ifndef JDRAMA_H
#define JDRAMA_H

#include "types.h"
#include "sms/JGeometry.hxx"

namespace JDrama
{

    class TDisplay
    {
    };

    class TGraphics
    {
    };

    template <typename T>
    class TFlagT
    {

    public:
        T mFlag; //0x0000
    };

    class TNameRef
    {

    public:
        void *mVTable;   //0x0000
        char *mTypeName; //0x0004
        u16 mKeyCode;    //0x0008
    };

    class TViewObj : public TNameRef
    {

    public:
        TFlagT<u16> mFlags; //0x000C
    };

    class TPlacement : public TViewObj
    {

    public:
        JGeometry::TVec3<f32> mPosition; //0x0010
        TFlagT<u16> mFlags;              //0x001C
    };

    class TCamera : public TPlacement
    {

    public:
        u32 _00[0x10 / 4];                  //0x0020
    };

    class TLookAtCamera : public TCamera
    {

    public:
        JGeometry::TVec3<f32> mUpVector;       //0x0030
        JGeometry::TVec3<f32> mTargetPosition; //0x003C
        f32 mProjectionFovy;                   //0x0048
        f32 mProjectionAspect;                 //0x004C
    };

    class TActor : public TPlacement
    {

    public:
        void *vTable;                    //0x0020
        JGeometry::TVec3<f32> mSize;     //0x0024
        JGeometry::TVec3<f32> mRotation; //0x0030
        u32 _02[0x8 / 4];                //0x003C
    };

}; // namespace JDrama

#endif