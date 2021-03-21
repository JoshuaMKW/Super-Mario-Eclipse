#pragma once

#include "types.h"
#include "JSUInputStream.hxx"
#include "../JKR/JKRFile.hxx"
#include "../JKR/JKRDvdFile.hxx"

class JSUFileInputStream : public JSURandomInputStream
{
public:
    JSUFileInputStream(JKRFile *);
    virtual ~JSUFileInputStream();

    virtual void readData(void *, s32) override;
	virtual s32 getLength() const override;
	virtual s32 getPosition() const override;
	virtual s32 seekPos(s32, JSUStreamSeekFrom) override;

    JKRDvdFile *mJKRDvdFile;
	u32 mPosition;
};