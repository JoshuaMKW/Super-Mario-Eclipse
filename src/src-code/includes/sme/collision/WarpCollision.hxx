#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/collision/BGCheck.hxx" 

#include "libs/sGeometry.hxx"

namespace SME::Class
{

struct TCollisionLink
{
    TCollisionLink() = default;
    TCollisionLink(TBGCheckData *tri, u8 targetID, u8 selfID) : mColTriangle(tri), mTargetID(targetID), mThisID(selfID) {}

    TBGCheckData *mColTriangle; //0x0000
    u8 mTargetID;               //0x0004
    u8 mThisID;                 //0x0005
};

class TWarpCollisionList
{
public:
    TWarpCollisionList() : mArrayLength(0), mColList() {};
    TWarpCollisionList(TCollisionLink colList[]);

    TBGCheckData *resolveCollisionWarp(TBGCheckData *colTriangle);
    TBGCheckData *getNearestTarget(TBGCheckData *colTriangle);

private:
    u8 mArrayLength;              //0x0000
    TCollisionLink mColList[0xFF]; //0x0004
};

}