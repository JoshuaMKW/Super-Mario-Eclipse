#pragma once

#include "types.h"
#include "OS.h"
#include "JKRDisposer.hxx"
#include "JSU/JSUList.hxx"

class JKRHeap : public JKRDisposer
{
public:
	class TState {};

	JKRHeap(void *, size_t, JKRHeap *, bool);
	virtual ~JKRHeap();
	
	virtual void *alloc(size_t, int) = 0;
	virtual void free(void *) = 0;
	virtual void freeAll();
	virtual void freeTail() = 0;
	virtual u32 resize(void *, u32) = 0;
	virtual u32 getSize(void *) = 0;
	virtual u32 getFreeSize() = 0;
	virtual u32 getTotalFreeSize() = 0;
	virtual u32 getHeapType() = 0;
	virtual bool check() = 0;
	virtual u32 dump_sort();
	virtual bool dump() = 0;
	virtual u8 changeGroupID(u8);
	virtual u8 getCurrentGroupId();
	virtual u32 state_register(JKRHeap::TState *, size_t);
	virtual u32 state_compare(const JKRHeap::TState &, const JKRHeap::TState &);
	virtual void state_dump(const JKRHeap::TState &);

	JKRHeap *becomeSystemHeap();
	JKRHeap *becomeCurrentHeap();
	bool initArena(u8 **, size_t, int);
	JKRHeap *find(void *) const;
	void dispose_subroutine(size_t, size_t);
	void dispose(void *, size_t);
	void dispose(void *, void *);
	void dispose();

	static void *alloc(size_t, int, JKRHeap *);
	static void copyMemory(void *, void *, size_t);
	static JKRHeap* findFromRoot(void *);
	static void free(void *, JKRHeap *);

	u32 _0C[3];
	OSMutex mMutex;
	u32 _2C;
	void *mStart;
	void *mEnd;
	size_t mSize;
	JSUPtrList mPtrList; // _3C
	u32 _48[4];
	JSUPtrList mPtrList2; // _58
	bool _64;

	static JKRHeap *sSystemHeap;
	static JKRHeap *sCurrentHeap;
	static JKRHeap *sRootHeap;
	static void *mErrorHandler;
	static u8 *mCodeStart;
	static u8 *mCodeEnd;
	static u8 *mUserRamStart;
	static u8 *mUserRamEnd;
	static size_t mMemorySize;
};

class JKRExpHeap : public JKRHeap
{
public:
	class CMemBlock
	{
	public:
		void *allocBack(size_t, u8, u8, u8, u8);
		void *allocFore(size_t, u8, u8, u8, u8);
		void free(JKRExpHeap *);
		void initiate(CMemBlock *, CMemBlock *, size_t, u8, u8);

		static CMemBlock *getHeapBlock(void *);
	};

	JKRExpHeap(void *, size_t, JKRHeap *, bool);
	virtual ~JKRExpHeap();

	virtual void *alloc(size_t, int) override;
	virtual void free(void *) override;
	virtual void freeAll() override;
	virtual void freeTail() override;
	virtual u32 resize(void *, size_t) override;
	virtual u32 getSize(void *) override;
	virtual u32 getFreeSize() override;
	virtual u32 getTotalFreeSize() override;
	virtual u32 getHeapType() override;
	virtual bool check() override;
	virtual u32 dump_sort() override;
	virtual bool dump() override;
	virtual u8 changeGroupID(u8) override;
	virtual u8 getCurrentGroupId() override;
	virtual u32 state_register(JKRHeap::TState *, size_t) override;
	virtual u32 state_compare(const JKRHeap::TState &, const JKRHeap::TState &) override;
	virtual void state_dump(const JKRHeap::TState &) override;

	void destroy();
	u32 getUsedSize(u8) const;
	void joinTwoBlocks(CMemBlock *);
	void recycleFreeBlock(CMemBlock *);
	void removeFreeBlock(CMemBlock *);
	void setFreeBlock(CMemBlock *, CMemBlock *, CMemBlock *);

	static JKRExpHeap *create(void *, size_t, JKRHeap *, bool);
	static JKRExpHeap *create(size_t, JKRHeap *, bool);
	static JKRExpHeap *createRoot(int, bool);

	u8 _68;
	u8 mGroupID; //_69
	u16 _6A;
	u32 _6C;
	u32 _70;
	u32 _74;
	u32 _78;
	u32 _7C;
	u32 _80;
};

class JKRStdHeap : public JKRHeap
{
public:
	JKRStdHeap(void *, size_t, JKRHeap *, bool);
	virtual ~JKRStdHeap();

	virtual void *alloc(size_t, int) override;
	virtual void free(void *) override;
	virtual void freeAll() override;
	virtual void freeTail() override;
	virtual u32 resize(void *, size_t) override;
	virtual u32 getSize(void *) override;
	virtual u32 getFreeSize() override;
	virtual u32 getTotalFreeSize() override;
	virtual u32 getHeapType() override;
	virtual bool check() override;
	virtual bool dump() override;
	virtual u32 state_register(JKRHeap::TState *, size_t) override;
	virtual u32 state_compare(const JKRHeap::TState &, const JKRHeap::TState &) override;

	static JKRStdHeap *create(size_t, JKRHeap *, bool);

	u32 mHeapID;
};

class JKRSolidHeap : public JKRHeap
{
public:
	JKRSolidHeap(void *, size_t, JKRHeap *, bool);
	virtual ~JKRSolidHeap();

	virtual void *alloc(size_t, int) override;
	virtual void free(void *) override;
	virtual void freeAll() override;
	virtual void freeTail() override;
	virtual u32 resize(void *, size_t) override;
	virtual u32 getSize(void *) override;
	virtual u32 getFreeSize() override;
	virtual u32 getTotalFreeSize() override;
	virtual u32 getHeapType() override;
	virtual bool check() override;
	virtual u32 dump_sort() override;
	virtual bool dump() override;
	virtual u32 state_register(JKRHeap::TState *, size_t) override;
	virtual u32 state_compare(const JKRHeap::TState &, const JKRHeap::TState &) override;

	static JKRSolidHeap *create(size_t, JKRHeap *, bool);

	u32 _68;
	u32 _6C;
	u32 _70;
	u32 _74;
};

void JKRDefaultMemoryErrorRoutine(void *, u32, u32);

void *operator new(size_t blocksize);
void *operator new(size_t blocksize, int align);
void *operator new(size_t blocksize, JKRHeap *heap, int align);
void *operator new[](size_t blocksize);
void *operator new[](size_t blocksize, int align);
void *operator new[](size_t blocksize, JKRHeap *heap, int align);
void operator delete(void *block);
void operator delete[](void *block);