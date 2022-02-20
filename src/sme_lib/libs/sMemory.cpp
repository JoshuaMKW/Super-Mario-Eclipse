#include "libs/sMemory.hxx"

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
