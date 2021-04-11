#include "sMemory.hxx"

using namespace SME;

struct MemCtrl
{
    u8 _reserved;
    u8 mCh3 : 2;
    u8 mCh2 : 2;
    u8 mCh1 : 2;
    u8 mCh0 : 2;

    void setAccess(u8 index, u8 access)
    {
        switch (index)
        {
        case 0:
            this->mCh0 = access;
            break;
        case 1:
            this->mCh1 = access;
            break;
        case 2:
            this->mCh2 = access;
            break;
        case 3:
            this->mCh3 = access;
            break;
        }
    }
};

struct MemEnbl
{
    u16 _reserved : 11;
    bool mChAll : 1;
    bool mCh3 : 1;
    bool mCh2 : 1;
    bool mCh1 : 1;
    bool mCh0 : 1;

    void setActive(u8 index, bool enable)
    {
        switch (index)
        {
        case 0:
            this->mCh0 = enable;
            break;
        case 1:
            this->mCh1 = enable;
            break;
        case 2:
            this->mCh2 = enable;
            break;
        case 3:
            this->mCh3 = enable;
            break;
        default:
            this->mChAll = enable;
            break;
        }
    }
};

static MemCtrl *gpMapCtrl = (MemCtrl *)SME::Util::Memory::Protection::MemoryMap::MARRCTRL;
static MemEnbl *gpMapEnbl = (MemEnbl *)SME::Util::Memory::Protection::MemoryMap::MARRENBL;

Util::Memory::Protection::MemoryMap::MemoryMap(u8 index)
{
    this->mIndex = index;
}

Util::Memory::Protection::MemoryMap::~MemoryMap()
{
    this->deactivate();
}

void Util::Memory::Protection::MemoryMap::activate()
{
    OSProtectRange(this->mIndex, this->start(), this->size(), this->permission());
}

void Util::Memory::Protection::MemoryMap::deactivate()
{
    u32 *mapAddrTable = (u32 *)SME::Util::Memory::Protection::MemoryMap::MARR0;

    mapAddrTable[this->mIndex] = 0;
    gpMapCtrl->setAccess(this->mIndex, READWRITE);
    gpMapEnbl->setActive(this->mIndex, false);
}

// -- ArenaBlock -- //

size_t Util::Memory::ArenaBlock::tailRealSize() const
{
    size_t size = this->size() + sizeof(ArenaBlock);

    ArenaBlock *block = this->next();
    while (block)
        size += block->size() + sizeof(ArenaBlock);

    return size;
}

void Util::Memory::ArenaBlock::free()
{
    ArenaBlock *prevBlock = this->prev();
    ArenaBlock *nextBlock = this->next();

    if (nextBlock)
        nextBlock->mPrev = prevBlock;
    if (prevBlock)
        prevBlock->mNext = nextBlock;
}

// -- ArenaHeap -- //

Util::Memory::ArenaHeap::ArenaHeap(size_t size)
{
    size = (size + 3) & -4;
    if (JKRHeap::mUserRamEnd)
        JKRHeap::mUserRamEnd -= size;
    else if (OSGetArenaHi())
        OSSetArenaHi((u8 *)OSGetArenaHi() - size);
    else
        *(u32 *)0x80000034 -= size;
}

Util::Memory::ArenaHeap::ArenaHeap(size_t size, void *cb)
{
    size = (size + 3) & -4;
    if (JKRHeap::mUserRamEnd)
        JKRHeap::mUserRamEnd -= size;
    else if (OSGetArenaHi())
        OSSetArenaHi((u8 *)OSGetArenaHi() - size);
    else
        *(u32 *)0x80000034 -= size;

    this->mCallback = cb;
}

Util::Memory::ArenaHeap::~ArenaHeap()
{
    if (JKRHeap::mUserRamEnd)
        JKRHeap::mUserRamEnd += this->maxSize();
    else if (OSGetArenaHi())
        OSSetArenaHi((u8 *)OSGetArenaHi() + this->maxSize());
    else
        *(u32 *)0x80000034 += this->maxSize();

    this->freeAll();
}

bool Util::Memory::ArenaHeap::canAlloc(const size_t size, const size_t alignment) const
{
    return this->size() > ((sizeof(ArenaBlock) + size + (alignment-1)) & -alignment);
}

void *Util::Memory::ArenaHeap::alloc(const size_t size, const size_t alignment, bool isVolatile)
{
    if (!this->canAlloc(size, alignment))
    {
        if (this->mCallback)
            ((void (*)(ArenaHeap *, size_t, size_t))this->mCallback)(this, this->size(), (sizeof(ArenaBlock) + size + (alignment-1)) & -alignment);
            
        return nullptr;
    }

    SME::Util::Memory::ArenaBlock *block;
    SME::Util::Memory::ArenaBlock *lastBlock = this->mTailBlock;
    if (lastBlock)
    {
        block = (ArenaBlock *)((u32)((u8 *)lastBlock + sizeof(ArenaBlock) + lastBlock->size() + (alignment-1)) & -alignment);
    }
    else
    {
        if (JKRHeap::mUserRamEnd)
            block = (ArenaBlock *)((u32)(JKRHeap::mUserRamEnd + (alignment-1)) & -alignment);
        else
            block = (ArenaBlock *)((u32)((u8 *)OSGetArenaHi() + (alignment-1)) & -alignment);
    }

    block->mSize = size;
    block->mAlignment = alignment;
    block->mGarbageCollect = isVolatile;

    lastBlock->mNext = block;
    block->mPrev = lastBlock;

    this->mTailBlock = block;
    return (void *)((u8 *)block + sizeof(ArenaBlock));
}

void Util::Memory::ArenaHeap::free(const void *p)
{
    ArenaBlock *block = (ArenaBlock *)((u8 *)p - sizeof(ArenaBlock));
    ArenaBlock *prevBlock = block->prev();
    ArenaBlock *nextBlock = block->next();

    if (this->defragmentEnabled())
        this->defragment();
        
    if (nextBlock)
    {
        nextBlock->mPrev = prevBlock;
        if (!prevBlock)
            this->mHeadBlock = nextBlock;
    }
    if (prevBlock)
    {
        prevBlock->mNext = nextBlock;
        if (!nextBlock)
            this->mTailBlock = prevBlock;
    }
}

void Util::Memory::ArenaHeap::freeAll()
{
    this->mHeadBlock = nullptr;
    this->mTailBlock = nullptr;
}

void Util::Memory::ArenaHeap::defragment()
{
    ArenaBlock *block = this->mHeadBlock;
    while (block)
    {
        ArenaBlock *nextBlock = block->next();
        if (nextBlock)
            memcpy((void *)((u32)((u8 *)block->end() + (block->mAlignment-1)) & -block->mAlignment), nextBlock, nextBlock->size() + sizeof(ArenaBlock));
    }
}

size_t Util::Memory::ArenaHeap::size() const
{
    size_t size = this->maxSize();

    ArenaBlock *block = this->mHeadBlock;
    if (block)
        size -= block->tailRealSize();

    return size;
}