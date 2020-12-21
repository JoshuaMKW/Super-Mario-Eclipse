#ifndef WARPCOLLISION_H
#define WARPCOLLISION_H

#include "types.h"
#include "sms/JGeometry.hxx"
#include "sms/SMS.hxx"

#include "sms/collision/BGCheck.hxx" 

#include "Vector3D.hxx"

class CollisionLink
{

public:
    TBGCheckData *mColTriangle; //0x0000
    u8 mTargetID;               //0x0004
    u8 mThisID;                 //0x0005

    CollisionLink() {}
    CollisionLink(TBGCheckData *tri, u8 targetID, u8 selfID)
    {
        this->mColTriangle = tri;
        this->mTargetID = targetID;
        this->mThisID = selfID;
    }
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
            return NULL;

        return this->getNearestTarget(colTriangle);
    }

    TBGCheckData *getNearestTarget(TBGCheckData *colTriangle)
    {
        Vector3D colVector;
        Vector3D targetVector;

        colVector.a = colTriangle->mVertexA;
        colVector.b = colTriangle->mVertexB;
        colVector.c = colTriangle->mVertexC;

        CollisionLink members[0xFF];
        f32 nearestDist = 10000000.0f;
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
        JGeometry::TVec3<f32> _set;
        index = -1;

        for (u32 i = 0; i <= index; ++i)
        {
            targetVector.a = members[i].mColTriangle->mVertexA;
            targetVector.b = members[i].mColTriangle->mVertexB;
            targetVector.c = members[i].mColTriangle->mVertexC;

            a = colVector.center();
            b = targetVector.center();

            _set = JGeometry::TVec3<f32>(b.x - a.x, b.y - a.y, b.z - a.z);

            if (_set.magnitude() < nearestDist)
            {
                nearestDist = _set.magnitude();
                index = i;
            }
        }
        if (index == -1)
            return NULL;

        return members[index].mColTriangle;
    }
};

#endif