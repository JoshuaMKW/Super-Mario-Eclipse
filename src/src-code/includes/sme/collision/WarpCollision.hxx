#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/collision/BGCheck.hxx" 

#include "libs/sGeometry.hxx"

class CollisionLink
{

public:
    TBGCheckData *mColTriangle; //0x0000
    u8 mTargetID;               //0x0004
    u8 mThisID;                 //0x0005

    CollisionLink() = default;
    CollisionLink(TBGCheckData *tri, u8 targetID, u8 selfID) : mColTriangle(tri), mTargetID(targetID), mThisID(selfID) {}
};

class WarpCollisionList
{
public:
    u8 arrayLength;              //0x0000
    CollisionLink mColList[0xFF]; //0x0004

    WarpCollisionList();

    TBGCheckData *resolveCollisionWarp(TBGCheckData *colTriangle)
    {
        if ((u8)(colTriangle->mValue4 >> 8) == 0xFF)
            return nullptr;

        return this->getNearestTarget(colTriangle);
    }

    TBGCheckData *getNearestTarget(TBGCheckData *colTriangle)
    {
        TVectorTriangle colVector;
        TVectorTriangle targetVector;

        colVector.a = colTriangle->mVertexA;
        colVector.b = colTriangle->mVertexB;
        colVector.c = colTriangle->mVertexC;

        CollisionLink members[0xFF];
        f32 nearestDist = 10000000000.0f;
        s32 index = 0;

        for (u32 i = 0; i < this->arrayLength; ++i)
        {
            if (this->mColList[i].mThisID == 0xFF)
                continue;
            if (this->mColList[i].mThisID == (u8)(colTriangle->mValue4 >> 8) && this->mColList[i].mColTriangle != colTriangle)
            {
                members[index] = this->mColList[i];
                index++;
            }
        }

        JGeometry::TVec3<f32> a;
        JGeometry::TVec3<f32> b;
        index = -1;

        for (u32 i = 0; i <= index; ++i)
        {
            targetVector.a = members[i].mColTriangle->mVertexA;
            targetVector.b = members[i].mColTriangle->mVertexB;
            targetVector.c = members[i].mColTriangle->mVertexC;

            JGeometry::TVec3<f32> a(colVector.center());
            JGeometry::TVec3<f32> b(targetVector.center());

            JGeometry::TVec3<f32> _set(b.x - a.x, b.y - a.y, b.z - a.z);

            if (_set.magnitude() < nearestDist)
            {
                nearestDist = _set.magnitude();
                index = i;
            }
        }
        if (index == -1)
            return nullptr;

        return members[index].mColTriangle;
    }
};