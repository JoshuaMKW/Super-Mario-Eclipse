#pragma once

#include "sms/manager/ObjManager.hxx"
#include "sms/spc/SpcBinary.hxx"

class TLiveManager : public TObjManager
{
public:
	TLiveManager(char const *);
	virtual ~TLiveManager();

	virtual void load(JSUMemoryInputStream &);
	virtual void perform(u32, JDrama::TGraphics *);
	virtual void clipActors(JDrama::TGraphics *);
	virtual void setFlagOutOfCube();
	virtual void createSpcBinary();
	virtual bool hasMapCollsion() const;
	
	TSpcBinary* mSpcBinary; // _34 
};