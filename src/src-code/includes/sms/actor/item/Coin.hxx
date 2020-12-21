#ifndef COIN_H
#define COIN_H

#include "types.h"
#include "sms/JDrama.hxx"
#include "sms/actor/BaseActor.hxx"
#include "sms/actor/Item.hxx"

class TCoin : public TItem
{
public:
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

#endif