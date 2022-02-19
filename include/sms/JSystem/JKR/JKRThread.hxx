#pragma once

#include "types.h"
#include "JKRDisposer.hxx"
#include "JKRHeap.hxx"
#include "JSU/JSUList.hxx"

class JKRThread : public JKRDisposer
{
public:
	JKRThread(u32 stackSize, int messageCount, int);
	virtual ~JKRThread();

	virtual void run();

	static void* start(void *src);

	JSUPtrLink mThreadPtrs;		  // _18
	JKRHeap *mHeap;				  // _28
	OSThread *mThread;			  // _2C
	OSMessageQueue mMessageQueue; // _30
	OSMessage *mMessage;		  // _50
	int mMessageCount;			  // _54
	void *mStackPtr;			  // _58
	u32 mStackSize;				  // _5C
};