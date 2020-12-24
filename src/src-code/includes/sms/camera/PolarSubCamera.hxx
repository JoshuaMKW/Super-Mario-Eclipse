#ifndef POLARSUBCAMERA_H
#define POLARSUBCAMERA_H

#include "types.h"
#include "sms/JDrama.hxx"
#include "sms/camera/CameraInbetween.hxx"

class CPolarSubCamera : public JDrama::TLookAtCamera
{

public:
    u32 _00[0x1C / 4];            //0x0050
    TCameraInbetween *mInbetween; //0x006C
    u32 _01[0x34 / 4];            //0x0070
    u16 _02;                      //0x00A4
    u16 mHorizontalAngle;         //0x00A6
    f32 mInterpolateDistance;   //0x00A8
};

#endif