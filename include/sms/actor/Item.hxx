#pragma once

#include "types.h"
#include "sms/mapobj/MapObjGeneral.hxx"

class TItem : public TMapObjGeneral
{
public:
    TItem(const char *);
	virtual ~TItem();
	
	virtual void load(JSUMemoryInputStream &) override;
	virtual void perform(u32, JDrama::TGraphics *) override;
	virtual bool receiveMessage(THitActor *, u32) override;
	virtual void calcRootMatrix() override;
	virtual void appear() override;
	virtual void initMapObj() override;
	virtual void calc() override;
	virtual void touchPlayer(THitActor *) override;
	virtual void appearing() override;
	virtual void appeared() override;
	virtual void killByTimer(int);
	virtual void taken(THitActor *);

    u32 *_148; // THideObjBase *
    s32 mKillTimer; // _14C
    u32 mStateTimerSet; // ?

    static f32 mAppearedScaleSpeed;
};