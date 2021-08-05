#ifndef DVD_H
#define DVD_H

#include "types.h"

#define DEVICE_TYPE_GAMECUBE_DVD (('G' << 24) | ('D' << 16) | ('V' << 8) | 'D')
#define DVD_ERROR_CANCELED -3
#define DVD_ERROR_COVER_CLOSED -4
#define DVD_ERROR_FATAL -1
#define DVD_ERROR_IGNORED -2
#define DVD_ERROR_OK 0
#define DVD_RESETHARD 0
#define DVD_RESETNONE 2
#define DVD_RESETSOFT 1
#define DVD_SPINMOTOR_ACCEPT 0x00004000
#define DVD_SPINMOTOR_CHECKDISK 0x00008000
#define DVD_SPINMOTOR_DOWN 0x00000000
#define DVD_SPINMOTOR_UP 0x00000100
#define DVD_STATE_BUSY 1
#define DVD_STATE_CANCELED 10
#define DVD_STATE_COVER_CLOSED 3
#define DVD_STATE_COVER_OPEN 5
#define DVD_STATE_END 0
#define DVD_STATE_FATAL_ERROR -1
#define DVD_STATE_IGNORED 8
#define DVD_STATE_MOTOR_STOPPED 7
#define DVD_STATE_NO_DISK 4
#define DVD_STATE_RETRY 11
#define DVD_STATE_WAITING 2
#define DVD_STATE_WRONG_DISK 6

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct DVDFileInfo DVDFileInfo;
    typedef struct DVDCommandBlock DVDCommandBlock;

    // our callbacks
    typedef void (*DVDCallback)(s32 result, DVDFileInfo *info);
    typedef void (*DVDCBCallback)(s32 result, DVDCommandBlock *cmdBlock);

    typedef struct DVDDiskID
    {
        char mName[0x04];     // _0
        char mCompany[0x02];  // _4
        u8 mDiskNum;          // _6
        u8 mVersion;          // _7
        u8 mStreaming;        // _8
        u8 mStreamBufferSize; // _9
        u8 mPad[0x16];        // _A
    } DVDDiskID;

    typedef struct DVDCommandBlock
    {
        DVDCommandBlock *mNextBlock; // _0
        DVDCommandBlock *mPrevBlock; // _4
        u32 mCurCommand;             // _8
        s32 mCurState;               // _C
        u32 mOffset;                 // _10
        u32 mLength;                 // _14
        void *mAddress;              // _18
        u32 mCurTransferSize;        // _1C
        u32 mCommandResult;        // _20
        DVDDiskID *mDiskID;          // _24
        DVDCBCallback mCB;           // _28
        void *mData;                 // _2C
    } DVDCommandBlock;

    typedef struct DVDFileInfo
    {
        DVDCommandBlock mCmdBlock; // _0
        u32 mStart;                // _30
        u32 mLen;                  // _34
        DVDCallback mCB;           // _38
    } DVDFileInfo;

    // credit to arookas
    typedef struct DVDDriveInfo
    {
        u16 mRevisionLevel; // _0
        u16 mDeviceCode;    // _2
        u16 mReleaseDate;   // _4
        u8 mPad[0x18];
    } DVDDriveInfo;

    void DVDInit();
    void DVDPause();
    void DVDResume();

    s32 DVDConvertPathToEntrynum(const char *path);
    bool DVDOpen(const char *name, DVDFileInfo *info);
    bool DVDFastOpen(s32 entryNum, DVDFileInfo *info);
    bool DVDClose(DVDFileInfo *info);

    bool DVDGetCurrentDir(const char *path, u32 maxLength);
    bool DVDChangeDir(const char *directoryName);

    bool DVDReadAsyncPrio(DVDFileInfo *info, void *address, s32 length, s32 offset, DVDCallback cb, s32 priority);
    s32 DVDReadPrio(DVDFileInfo *info, void *address, s32 length, s32 offset, s32 priority);

    bool DVDPrepareStreamAsync(DVDFileInfo *info, u32 length, u32 offset, DVDCallback cb);
    bool DVDCancelStreamAsync(DVDCommandBlock *cmdBlock, DVDCBCallback cb);
    bool DVDStopStreamAtEndAsync(DVDCommandBlock *cmdBlock, DVDCBCallback cb);
    u32 DVDStopStreamAtEnd(DVDCommandBlock *cmdblock);
    bool DVDGetStreamErrorStatusAsync(DVDCommandBlock *cmdblock, DVDCBCallback cb);
    u32 DVDGetStreamErrorStatus(DVDCommandBlock *cmdblock);
    bool DVDGetStreamLengthAsync(DVDCommandBlock *cmdblock, DVDCBCallback cb);
    u32 DVDGetStreamLength(DVDCommandBlock *cmdblock);
    bool DVDGetStreamPlayAddrAsync(DVDCommandBlock *cmdblock, DVDCBCallback cb);
    u32 DVDGetStreamPlayAddr(DVDCommandBlock *cmdblock);
    bool DVDGetStreamStartAddrAsync(DVDCommandBlock *cmdblock, DVDCBCallback cb);
    u32 DVDGetStreamStartAddr(DVDCommandBlock *cmdblock);
    u32 DVDGetTransferredSize(DVDCommandBlock *cmdblock);

    s32 DVDGetCommandBlockStatus(DVDCommandBlock *cmdBlock);
    s32 DVDGetDriveStatus();

    bool DVDInquiryAsync(DVDCommandBlock *cmdBlock, DVDDriveInfo *info, void *cb);

    bool DVDSetAutoInvalidation(bool isAutoInvalidated);

    bool DVDCancelAsync(DVDCommandBlock *cmdBlock, DVDCBCallback cb);
    s32 DVDCancel(DVDCommandBlock *cmdBlock);
    DVDDiskID *DVDGetCurrentDiskID();
    bool DVDCheckDisk();

#ifdef __cplusplus
}
#endif

#endif