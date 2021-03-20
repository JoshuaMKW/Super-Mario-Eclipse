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

	virtual void loadAfter() override;
	virtual void perform(u32, JDrama::TGraphics *) override;
	virtual void appear() override;
	virtual void makeObjAppeared() override;
	virtual void makeObjDead() override;
	virtual void initMapObj() override;
	virtual void taken(THitActor *) override;

	u32 _154;
};