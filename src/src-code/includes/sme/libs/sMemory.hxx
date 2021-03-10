#include "mem.h"

#pragma once

#include "types.h"
#include "OS.h"
#include "sms/JKR.hxx"

void *operator new(size_t size, JKRHeap *heap) noexcept
{
    if (heap)
        return heap->alloc(size, 4);
    
    if (JKRHeap::sCurrentHeap)
        return JKRHeap::sCurrentHeap->alloc(size, 4);
    
    return nullptr;
}

namespace Cache
{
    inline void flush(void *addr, size_t size)
    {
        DCFlushRange(addr, size);
        ICInvalidateRange(addr, size);
    }

    inline void flash()
    {
        ICFlashInvalidate();
    }

    inline void store(void *addr, size_t size)
    {
        DCStoreRange(addr, size);
        ICInvalidateRange(addr, size);
    }

    inline void zero(void *addr, size_t size)
    {
        DCZeroRange(addr, size);
    }

    inline void enable()
    {
        DCEnable();
        ICEnable();
    }

    inline void disable()
    {
        DCDisable();
        ICDisable();
    }
} // namespace Cache

namespace Memory
{
    inline void *malloc(const size_t size, const size_t alignment) { return new (alignment) u8[size]; }
    inline void *hmalloc(JKRHeap *heap, const size_t size, const size_t alignment) { return new (heap, alignment) u8[size]; }
    inline void *calloc(const size_t size, const size_t alignment)
    {
        void *obj = malloc(size, alignment);
        memset(obj, 0, size);
        return obj;
    }
    inline void *hcalloc(JKRHeap *heap, const size_t size, const size_t alignment)
    {
        void *obj = hmalloc(heap, size, alignment);
        memset(obj, 0, size);
        return obj;
    }
    inline void free(const void *ptr) { delete (u8 *)ptr; }

    namespace PPC
    {
        inline u32 *getBranchDest(u32 *bAddr)
        {
            s32 offset;
            if (*bAddr & 0x2000000)
                offset = (*bAddr & 0x3FFFFFD) - 0x4000000;
            else
                offset = *bAddr & 0x3FFFFFD;
            return (u32 *)((u32)bAddr + offset);
        }

        inline void write(void *ptr, u32 value)
        {
            *(u32 *)ptr = value;
            Cache::flush(ptr, 4);
        }
    } // namespace PPC

    namespace Processor
    {
    }

    namespace Protection
    {
        enum ACCESS
        {
            DENIED,
            READ,
            WRITE,
            READWRITE
        };

        class MemoryMap
        {
            u16 _mStart;
            u16 _mEnd;
            ACCESS mPermission;
            u8 mIndex;

        public:
            MemoryMap() {}
            MemoryMap(u8 index);
            ~MemoryMap();

            void *end() const { return (void *)((this->_mEnd << 10) | 0x80000000); }
            u8 index() const { return this->mIndex; }
            ACCESS permission() const { return this->mPermission; }
            s32 size() const { return (this->_mEnd - this->_mStart) << 10; }
            void *start() const { return (void *)((this->_mStart << 10) | 0x80000000); }
            void setStart(const u32 addr) { this->_mStart = (addr & 0x1FFFFFF) >> 10; }
            void setEnd(const u32 addr) { this->_mEnd = (addr & 0x1FFFFFF) >> 10; }
            void setPermission(ACCESS perm) { this->mPermission = perm; }
            void setIndex(u8 index) { this->mIndex = index; }

            void activate();
            void deactivate();

            static const u32 MARR0 = 0xCC004000;
            static const u32 MARR1 = 0xCC004004;
            static const u32 MARR2 = 0xCC004008;
            static const u32 MARR3 = 0xCC00400C;
            static const u32 MARRCTRL = 0xCC004010;
            static const u32 MARRENBL = 0xCC00401C;
        };
    } // namespace Protection

    class ArenaBlock
    {
        ArenaBlock *mPrev;
        ArenaBlock *mNext;
        size_t mSize;
        size_t mAlignment;
        bool mGarbageCollect;

        constexpr size_t tailRealSize() const;

    public:
        bool isVolatile() const { return this->mGarbageCollect; };

        void *end() const { return (void *)((u8 *)this + sizeof(ArenaBlock) + this->size()); };
        ArenaBlock *next() const { return this->mNext; };
        ArenaBlock *prev() const { return this->mPrev; };
        constexpr size_t size() const { return this->mSize; };
        void *start() const { return (void *)((u8 *)this + sizeof(ArenaBlock)); };

        void setVolatile(bool v) { this->mGarbageCollect = v; };

        void free();

        friend class ArenaHeap;
    };

    class ArenaHeap
    {
        ArenaBlock *mHeadBlock;
        ArenaBlock *mTailBlock;
        void *mCallback;
        size_t mHeapSize;
        bool mAutoDefragment;

        void defragment();

    public:
        ArenaHeap(const size_t size);
        ArenaHeap(const size_t size, void *cb);
        ~ArenaHeap();

        bool defragmentEnabled() const { return this->mAutoDefragment; };
        void setDefragment(bool autoDefrag) { this->mAutoDefragment = autoDefrag; };
        void setFailCallback(void *cb) { this->mCallback = cb; };

        size_t maxSize() const { return this->mHeapSize; };
        size_t size() const;

        bool canAlloc(const size_t size, const size_t alignment = 4) const;

        void *alloc(const size_t size, const size_t alignment, bool isVolatile = false);
        void free(const void *block);
        void freeAll();
    };
} // namespace Memory
