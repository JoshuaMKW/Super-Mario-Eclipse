#pragma once

#include "types.h"
#include "sms/J3D.hxx"


class MActorAnmBase
{
public:
	MActorAnmBase();

	virtual void setModel(J3DModel *);
	virtual void updateIn();
	virtual void updateOut();
	virtual void checkUseMaterialIDInit(u16 *);
	virtual void checkUseMaterialID(u16 *);
	virtual void setAnmFromIndex(int, u16 *) = 0;
	virtual void setAnm(const char *, u16 *) = 0;
	virtual void verifySuffix(const char *) = 0;

	J3DFrameCtrl mFrameCtrl;
	J3DModel *mModel;
	u32 _1C;
};

template <typename T>
class MActorAnmEach : public MActorAnmBase
{
public:
	virtual void setAnm(const char *, u16 *) override;

	void setFrameCtrl(int);
};

template <typename T>
class MActorAnmMatEach : public MActorAnmEach<T>
{
public:
	virtual void setAnmFromIndex(int, u16 *) override;
	virtual void setAnm(const char *, u16 *) override;

	void loadAnmPtrArray(const char *, const char *);
};

class MActorAnmBlk : public MActorAnmEach<J3DAnmClusterKey>
{
public:
	virtual void updateIn() override;
	virtual void updateOut() override;
	virtual void setAnmFromIndex(int, u16 *) override;
	virtual void verifySuffix(const char *) override;
};

class MActorAnmBrk : public MActorAnmMatEach<J3DAnmClusterKey>
{
public:
	virtual void updateIn() override;
	virtual void updateOut() override;
	virtual void checkUseMaterialIDInit(u16 *) override;
	virtual void checkUseMaterialID(u16 *) override;
	virtual void verifySuffix(const char *) override;

	void setTevColorAnmKeyPtr();
	void setTevKColorAnmKeyPtr();
};

class MActorAnmBpk : public MActorAnmMatEach<J3DAnmClusterKey>
{
public:
	virtual void updateIn() override;
	virtual void updateOut() override;
	virtual void checkUseMaterialIDInit(u16 *) override;
	virtual void checkUseMaterialID(u16 *) override;
	virtual void verifySuffix(const char *) override;

	void setMatColorAnmKeyPtr();
};

class MActorAnmBtk : public MActorAnmMatEach<J3DAnmClusterKey>
{
public:
	virtual void updateIn() override;
	virtual void updateOut() override;
	virtual void checkUseMaterialIDInit(u16 *) override;
	virtual void checkUseMaterialID(u16 *) override;
	virtual void verifySuffix(const char *) override;

	void setTexMtxAnmKeyPtr();
};

class MActorAnmBtp : public MActorAnmMatEach<J3DAnmClusterKey>
{
public:
	virtual void updateIn() override;
	virtual void updateOut() override;
	virtual void checkUseMaterialIDInit(u16 *) override;
	virtual void checkUseMaterialID(u16 *) override;
	virtual void verifySuffix(const char *) override;

	void setTexNoAnmFullPtr();
};

class MActorAnmBck : public MActorAnmMatEach<J3DAnmClusterKey>
{
public:
	virtual void setModel(J3DModel *) override;
	virtual void updateIn() override;
	virtual void updateOut() override;
	virtual void setAnmFromIndex(int, u16 *) override;
	virtual void verifySuffix(const char *) override;

	f32 getMotionBlendRatio() const;
	void *getOldMotionBlendAnmPtr() const;
	f32 getOldMotionBlendFrame() const;
	void initNormalMotionBlend();
	void initSimpleMotionBlend(int);
	void setAnmFromIndex(int, u16 *);
	void setMotionBlendRatio(f32);
	void setOldMotionBlendAnmPtr(J3DAnmTransform *);
};

class MActorAnmData : public J3DModelLoader
{
public:
    enum LoopType
    {
        END,
        BEGIN,
        LOOP
    };

	MActorAnmData();

	void addFileTable(const char *);
	void createSampleModelData(J3DModelData *);
	void init(const char *, const char *);
	s32 partsNameToIdx(const char *);

    u32 mAnimationID; //0x0000
    void *vTable;     //0x0004
    u8 mLoopType;     //0x0008
    u8 _09;           //0x0009
    u16 _01;          //0x000A
    u16 mNumFrames;   //0x000C
    u16 _02;          //0x000E
    f32 mFrameRate; //0x0010
    f32 mCurFrame;  //0x0014
	u32 _18;
	u8 _1C;
	u32 _20;
	u32 _24;
	u32 _28;
	u32 _2C;
	u32 _30;
	u32 _34;
	u32 _38;
	u32 _3C;
	u32 _40;
	u32 _44;
	u32 _48;
};

class MActor
{

public:
    u32 _00[0xC / 4];       //0x0000
    MActorAnmBck *mBckInfo; //0x000C
    u32 _10;
	u32 _14;
	u32 _18;
	u32 _1C;
	u32 _20;
	u32 _24;
	u32* _28;
	u32 _2C;
	u32 _30;
	u32 _34;
	u8 _38;
	u8 _39;
	u8 _3A; // padding?
	u8 _3B; // ^^
	u32 mLightID; // _3C
	u8 _40;
	u8 _41; // padding?
	u8 _42; // ^^
	u8 _43; // ^^
	u32 mLightType; // _44
};