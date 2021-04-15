#pragma once

#include "types.h"

class JSUPtrList;

class JSUPtrLink
{
public:
    JSUPtrLink();
    ~JSUPtrLink();

	void *_0;
	JSUPtrList *mPtrList; // _4
	u32 _8;
	u32 _C;
};

class JSUPtrList
{
public:
    JSUPtrList();
    JSUPtrList(bool);
    ~JSUPtrList();

    void append(JSUPtrLink *);
    JSUPtrLink *getFirstLink() const;
    void initiate();
    void insert(JSUPtrLink *, JSUPtrLink *);
    void prepend(JSUPtrLink *);
    void remove(JSUPtrLink *);

	u32 _0;
	u32 _4;
	s32 mNumPtrs; // _8
};

template <typename T>
class JSUList : public JSUPtrList
{
public:
    JSUList();
    ~JSUList();
};