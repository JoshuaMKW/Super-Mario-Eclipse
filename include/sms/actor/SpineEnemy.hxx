#pragma once

#include "types.h"
#include "sms/actor/LiveActor.hxx"
#include "sms/graph/GraphTracer.hxx"

class TSpineEnemy : public TLiveActor
{

public:
    TSpineEnemy(const char *);
	virtual ~TSpineEnemy();
	
	virtual void load(JSUMemoryInputStream &);
	virtual void perform(u32, JDrama::TGraphics *);
	virtual bool receiveMessage(THitActor *, u32);
	virtual void init(TLiveManager *);
	virtual void calcRootMatrix();
	virtual void reset();
	virtual void resetToPosition(const JGeometry::TVec3<f32> &);
	virtual void resetSRTV(const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &);
	virtual u32 getSaveParam() const;
	virtual f32 getPhaseShift() const;
	virtual bool isReachedToGoal() const;

	void doShortCut();
	void walkToCurPathNode(f32, f32, f32);
	void turnToCurPathNode(f32);
	void zigzagToCurPathNode(f32, f32, f32, f32);

	f32 getCurAnmFrameNo(int) const;

    u32 _F4;
	f32 _F8;
	f32 _FC;
	f32 _100;
	u32 _104;
	f32 _108;
	f32 _10C;
	f32 _110;
	u32 _114;
	u32 _118;
	u32* _11C; // array
	u32 _120;
	TGraphTracer *mGraphTracer;
	u16 _128;
	u16 _12A; // padding?
	f32 _12C;
	u8 _130;
	u8 _131; // padding?
	u8 _132; // ^^
	u8 _133; // ^^
	f32 _134;
	u32* _138;
	u8 mHealth; // _13C
	u8 _13D; // padding?
	u8 _13E; // ^^
	u8 _13F; // ^^
	f32 _140;
	f32 _144;
	f32 _148;
	f32 _14C;
};