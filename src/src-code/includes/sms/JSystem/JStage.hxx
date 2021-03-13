#pragma once

#include "types.h"
#include "GX.h"

namespace JStage
{
    enum class TECameraProjection
    {
    };

    enum class TECameraView
    {
    };

    enum class TEObject
    {
    };

    class TObject
    {
    public:
        virtual ~TObject();

        virtual u32 JSGGetName() const;
        virtual u32 JSGGetFlag() const;
        virtual void JSGSetFlag(u32);
        virtual u32 JSGGetData(u32, void *, u32) const;
        virtual void JSGSetData(u32, void const *, u32);
        virtual void JSGGetParent(TObject **, u32 *) const;
        virtual void JSGSetParent(TObject *, u32);
        virtual void JSGSetRelation(bool, TObject *, u32);
    };

    class TSystem : public TObject
    {
    public:
        virtual ~TSystem();
        
        virtual u32 JSGFGetType() const;
        virtual u32 JSGGetSystemData(u32);
        virtual void JSGSetSystemData(u32, u32);
        virtual TObject *JSGFindObject(const char *, TEObject) const;
        virtual u32 JSGCreateObject(const char *, TEObject);
        virtual void JSGDestroyObject(TObject *);
    };

    class TActor : public TObject
    {
    public:
        virtual ~TActor();

        virtual u32 JSGFGetType() const;
        virtual void JSGGetTranslation(Vec *) const;
        virtual void JSGSetTranslation(const Vec &);
        virtual void JSGGetScaling(Vec *) const;
        virtual void JSGSetScaling(const Vec &);
        virtual void JSGGetRotation(Vec *) const;
        virtual void JSGSetRotation(const Vec &);

        virtual u32 JSGGetShape() const;
        virtual void JSGSetShape(u32);
        virtual u32 JSGGetAnimation() const;
        virtual void JSGSetAnimation(u32);
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

        virtual u32 JSGFGetType() const;
    };
};