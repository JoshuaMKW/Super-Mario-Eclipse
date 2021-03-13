#pragma once

#include "types.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/actor/HitActor.hxx"
#include "sms/actor/Item.hxx"

class TCoin : public TItem
{
public:
	TCoin(const char *);
	virtual ~TCoin();

	virtual void loadAfter();
	virtual void perform(u32, JDrama::TGraphics *);
	virtual void appear();
	virtual void makeObjAppeared();
	virtual void makeObjDead();
	virtual void initMapObj();
	virtual void taken(THitActor *);

	u32 _154;
};