#pragma once

#include "types.h"

class JSUPtrList;

class JSUPtrLink
{
public:
    JSUPtrLink();
    ~JSUPtrLink();

	void *mItemPtr;
	JSUPtrList *mParentList; // _4
	JSUPtrLink *mPrevLink;
	JSUPtrLink *mNextLink;
};

class JSUPtrList
{
public:
    JSUPtrList();
    JSUPtrList(bool);
    ~JSUPtrList();

    void append(JSUPtrLink *link);
    JSUPtrLink *getFirstLink() const;
    void initiate();
    void insert(JSUPtrLink *atLink, JSUPtrLink *link);
    void prepend(JSUPtrLink *link);
    void remove(JSUPtrLink *link);

	JSUPtrLink *mFirst;
	JSUPtrLink *mLast;
	size_t mSize;
};

template <typename T>
class JSUList : public JSUPtrList
{
public:
    JSUList();
    ~JSUList();
};