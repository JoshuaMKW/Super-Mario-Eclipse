#pragma once

#include "types.h"
#include "J3D/J3DJoint.hxx"

// JointObj represents joints of a model that can be slept or awoke

class TJointObj {
public:
    TJointObj();

    virtual void initJointObj(J3DJoint *);
    virtual void alive();
    virtual void awake();
    virtual void stand();
    virtual void kill();
    virtual void sleep();
    virtual void sit();
    virtual TJointObj *newJointObj() const;

    void initChildren();

    u32 _04;
    u32 _08;
    u32 _0C;
    size_t mChildNum;
    TJointObj **mChildren;
    size_t mUnkNum;
    void **_1C;
};