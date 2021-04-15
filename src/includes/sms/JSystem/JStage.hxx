#pragma once

#include "types.h"
#include "GX.h"

namespace JStage
{
    enum TECameraProjection
    {
    };

    enum TECameraView
    {
    };

    enum TEObject
    {
    };

    class TObject
    {
    public:
        virtual ~TObject();

        virtual unsigned long JSGGetName() const;
        virtual unsigned long JSGGetFlag() const;
        virtual void JSGSetFlag(unsigned long);
        virtual unsigned long JSGGetData(unsigned long, void *, unsigned long) const;
        virtual void JSGSetData(unsigned long, void const *, unsigned long);
        virtual void JSGGetParent(TObject **, unsigned long *) const;
        virtual void JSGSetParent(TObject *, unsigned long);
        virtual void JSGSetRelation(bool, TObject *, unsigned long);
    };

    class TSystem : public TObject
    {
    public:
        virtual ~TSystem();
        
        virtual unsigned long JSGFGetType() const;
        virtual unsigned long JSGGetSystemData(unsigned long);
        virtual void JSGSetSystemData(unsigned long, unsigned long);
        virtual TObject *JSGFindObject(const char *, TEObject) const;
        virtual unsigned long JSGCreateObject(const char *, TEObject);
        virtual void JSGDestroyObject(TObject *);
    };

    class TActor : public TObject
    {
    public:
        virtual ~TActor();

        virtual unsigned long JSGFGetType() const;
        virtual void JSGGetTranslation(Vec *) const;
        virtual void JSGSetTranslation(const Vec &);
        virtual void JSGGetScaling(Vec *) const;
        virtual void JSGSetScaling(const Vec &);
        virtual void JSGGetRotation(Vec *) const;
        virtual void JSGSetRotation(const Vec &);

        virtual unsigned long JSGGetShape() const;
        virtual void JSGSetShape(unsigned long);
        virtual unsigned long JSGGetAnimation() const;
        virtual void JSGSetAnimation(unsigned long);
        virtual f32 JSGGetAnimationFrame() const;
        virtual void JSGSetAnimationFrame(f32);
        virtual f32 JSGGetAnimationFrameMax() const;
    };

    class TCamera : public TObject
    {
    public:
        virtual ~TCamera();

        virtual s32 JSGFGetType() const;
        virtual s32 JSGGetProjectionType() const;
        virtual void JSGSetProjectionType(TECameraProjection);
        virtual f32 JSGGetProjectionNear() const;
        virtual void JSGSetProjectionNear(f32);
        virtual f32 JSGGetProjectionFar() const;
        virtual void JSGSetProjectionFar(f32);
        virtual f32 JSGGetProjectionFovy() const;
        virtual void JSGSetProjectionFovy(f32);
        virtual f32 JSGGetProjectionAspect() const;
        virtual void JSGSetProjectionAspect(f32);
        virtual f32* JSGGetProjectionField() const;
        virtual void JSGSetProjectionField(f32 const *);
        virtual s32 JSGGetViewType() const;
        virtual void JSGSetViewType(TECameraView);
        
    };

    class TAmbientLight : public TObject
    {
    public:
        virtual ~TAmbientLight();

        virtual unsigned long JSGFGetType() const;
    };
};