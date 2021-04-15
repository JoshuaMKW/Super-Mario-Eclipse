#pragma once

#include "types.h"
#include "CARD.h"
#include "OS.h"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"

class TCardBookmarkInfo
{
public:
    struct
    {
        u32 _00[0x1C / 4]; //0x0000
        u16 mShineCount;   //0x001C
        u16 _01;           //0x001E
    } FileData[3];
};

class TCardManager
{
public:
    enum Commands
    {
        NOP,
        FORMAT,
        CREATE,
        GETBOOKMARKS,
        NOP4,
        LOADBLOCK,
        SAVEBLOCK,
        LOADFILE,
        SAVEFILE,
        EXIT
    };

    enum Blocks
    {
        A,
        B,
        C
    };

    class TCriteria
    {
    public:
        enum TEBlockStat
        {
            SETTING = 1,
            COPYING = 3,
        };

        TCriteria();

        void set(TEBlockStat, size_t, const void *);

        TEBlockStat mBlockStat;
        u32 _04; //?
        u8 mBuffer[32];
    };

    TCardManager(void *, void *, s32, s32, void *, size_t);
    ~TCardManager();

    void cmdLoop();
    void copyTo(TCriteria *, TCardBookmarkInfo *);
    s32 createFile_();
    void createFile();
    s32 decideUseSector(TCriteria *);
    void filledInitData(CARDFileInfo *);
    void format();
    TCardBookmarkInfo *getBookmarkInfos_();
    void getBookmarkInfos();
    s32 getLastStatus();
    JSUMemoryInputStream *getOptionReadStream(JSUMemoryInputStream *);
    JSUMemoryOutputStream *getOptionWriteStream(JSUMemoryOutputStream *);
    JSUMemoryInputStream *getReadStream(JSUMemoryInputStream *);
    JSUMemoryOutputStream *getWriteStream(JSUMemoryOutputStream *);
    s32 mount_(bool);
    s32 open_(CARDFileInfo *);
    s32 probe();
    s32 readBlock_(u32);
    void readBlock(u32);
    void readOptionBlock(u32);
    void setCardStat(CARDFileInfo *);
    void unmount();
    s32 writeBlock_(u32);
    void writeBlock(u32);
    s32 writeOptionBlock_();
    void writeOptionBlock();

    s32 mChannel;
    u32 _04;
    u32 _08;
    TCriteria mCriteriaArray[7];
    u8 _124;
    bool mMounted;              //0x0125
    bool mRefuseOverwriteMount; //0x0126
    s32 mLastStatus;               //0x0128
    void *mCardWorkArea;           //0x012C
    void *mCARDBlock;              //0x0130
    u32 _134;
    OSThread mThread;
    Commands mCommand;             //0x0448
    OSMutex mMutex;
    OSCond mCond;
    TCardBookmarkInfo *mBookMarks; //0x046C
    Blocks mSaveBlock;             //0x0474
};

void cardmain(void *);