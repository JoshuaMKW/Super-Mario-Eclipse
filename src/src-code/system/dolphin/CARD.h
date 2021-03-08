#ifndef CARD_H
#define CARD_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CARD_SLOTA 0 /*!< memory card slot A */
#define CARD_SLOTB 1 /*!< memory card slot B */

#define CARD_WORKAREA (5 * 8 * 1024) /*!< minimum size of the workarea passed to Mount[Async]() */
#define CARD_READSIZE 512            /*!< minimum size of block to read from memory card */
#define CARD_FILENAMELEN 32          /*!< maximum filename length */
#define CARD_MAXFILES 128            /*!< maximum number of files on the memory card */

#define CARD_ERROR_UNLOCKED 1       /*!< card being unlocked or already unlocked. */
#define CARD_ERROR_READY 0          /*!< card is ready. */
#define CARD_ERROR_BUSY -1          /*!< card is busy. */
#define CARD_ERROR_WRONGDEVICE -2   /*!< wrong device connected in slot */
#define CARD_ERROR_NOCARD -3        /*!< no memory card in slot */
#define CARD_ERROR_NOFILE -4        /*!< specified file not found */
#define CARD_ERROR_IOERROR -5       /*!< internal EXI I/O error */
#define CARD_ERROR_BROKEN -6        /*!< directory structure or file entry broken */
#define CARD_ERROR_EXIST -7         /*!< file allready exists with the specified parameters */
#define CARD_ERROR_NOENT -8         /*!< found no empty block to create the file */
#define CARD_ERROR_INSSPACE -9      /*!< not enough space to write file to memory card */
#define CARD_ERROR_NOPERM -10       /*!< not enough permissions to operate on the file */
#define CARD_ERROR_LIMIT -11        /*!< card size limit reached */
#define CARD_ERROR_NAMETOOLONG -12  /*!< filename too long */
#define CARD_ERROR_ENCODING -13     /*!< font encoding PAL/SJIS mismatch*/
#define CARD_ERROR_CANCELED -14     /*!< card operation canceled */
#define CARD_ERROR_FATAL_ERROR -128 /*!< fatal error, non recoverable */

/* File attribute defines */
#define CARD_ATTRIB_PUBLIC 0x04
#define CARD_ATTRIB_NOCOPY 0x08
#define CARD_ATTRIB_NOMOVE 0x10

/* Banner & Icon Attributes */
#define CARD_BANNER_W 96
#define CARD_BANNER_H 32

#define CARD_BANNER_NONE 0x00
#define CARD_BANNER_CI 0x01
#define CARD_BANNER_RGB 0x02
#define CARD_BANNER_MASK 0x03

#define CARD_MAXICONS 8
#define CARD_ICON_W 32
#define CARD_ICON_H 32

#define CARD_ICON_NONE 0x00
#define CARD_ICON_CI 0x01
#define CARD_ICON_RGB 0x02
#define CARD_ICON_MASK 0x03

#define CARD_ANIM_LOOP 0x00
#define CARD_ANIM_BOUNCE 0x04
#define CARD_ANIM_MASK 0x04

#define CARD_SPEED_END 0x00
#define CARD_SPEED_FAST 0x01
#define CARD_SPEED_MIDDLE 0x02
#define CARD_SPEED_SLOW 0x03
#define CARD_SPEED_MASK 0x03

#define CARDGetBannerFmt(stat) (((stat)->mBannerFmt) & CARD_BANNER_MASK)
#define CARDSetBannerFmt(stat, fmt) ((stat)->mBannerFmt = (u8)(((stat)->mBannerFmt & ~CARD_BANNER_MASK) | (fmt)))
#define CARDGetIconFmt(stat, n) (((stat)->mIconFmt >> (2 * (n))) & CARD_ICON_MASK)
#define CARDSetIconFmt(stat, n, fmt) ((stat)->mIconFmt = (u16)(((stat)->mIconFmt & ~(CARD_ICON_MASK << (2 * (n)))) | ((fmt) << (2 * (n)))))
#define CARDGetIconSpeed(stat, n) (((stat)->mIconSpeed >> (2 * (n))) & ~CARD_SPEED_MASK);
#define CARDSetIconSpeed(stat, n, speed) ((stat)->mIconSpeed = (u16)(((stat)->mIconFmt & ~(CARD_SPEED_MASK << (2 * (n)))) | ((speed) << (2 * (n)))))
#define CARDSetIconAddr(stat, addr) ((stat)->mIconAddr = (u32)(addr))
#define CARDSetCommentAddr(stat, addr) ((stat)->mCommentAddr = (u32)(addr))

    // this seems to be what is referenced in a lot of functions
    typedef void (*CARDCallback)(s32 chan, s32 res);

    /*! \typedef struct card_dir
    \brief structure to hold the information of a directory entry
    \param mChan CARD slot.
    \param mFileno file index in the card directory structure.
    \param mFilelen length of file.
    \param mFilename[CARD_FILENAMELEN] name of the file on card.
    \param mGamecode[4] string identifier <=4.
    \param mCompany[2] string identifier <=2.
    \param mShowall boolean flag whether to showall entries or ony those identified by card_gamecode and card_company, previously set within the call to CARD_Init()
    */
    typedef struct CARDDir
    {
        s32 mChan;
        u32 mFileno;
        u32 mFilelen;
        u8 mPermissions;
        u8 mFilename[CARD_FILENAMELEN];
        u8 mGamecode[4];
        u8 mCompany[2];
        bool mShowall;
    } CARDDir;

    /*! \typedef struct CARDStat
    \brief structure to hold the additional statistical informations.
    \param mFilename[CARD_FILENAMELEN] name of the file on card.
    \param mLength length of file.
    \param mGamecode[4] string identifier <=4.
    \param mCompany[2] string identifier <=2.
    \param mBannerFmt format of banner.
    \param mIconAddr icon image address in file.
    \param mIconSpeed speed of an animated icon.
    \param mCommentAddr address in file of the comment block.
    \param mOffsetBanner offset in file to the banner's image data.
    \param mOffsetBannerTlut offset in file to the banner's texture lookup table.
    \param mOffsetIcon[CARD_MAXICONS] array of offsets in file to the icon's image data <CARD_MAXICONS.
    \param mOffsetIconTlut offset in file to the icons's texture lookup table.
    \param mOffsetData offset to additional data.
    */
    typedef struct CARDStat
    {
        char mFilename[CARD_FILENAMELEN];
        u32 mLength;
        u32 mTime; //time since 1970 in seconds
        char mGamecode[4];
        char mCompany[2];
        u8 mBannerFmt;
        u32 mIconAddr;
        u16 mIconFmt;
        u16 mIconFmtList[CARD_MAXICONS];
        u16 mIconSpeed;
        u16 mIconSpeedList[CARD_MAXICONS];
        u32 mCommentAddr;
        u32 mOffsetBanner;
        u32 mOffsetBannerTlut;
        u32 mOffsetIcon[CARD_MAXICONS];
        u32 mOffsetIconTlut[CARD_MAXICONS];
        u32 mOffsetData;
    } CARDStat;

    /*! \typedef struct CARDInfo
    \brief structure to hold the fileinformations upon open and for later use.
    \param mChannel CARD slot.
    \param mFilenum file index in the card directory structure.
    \param mOffset offset into the file.
    \param mLength length of file.
    \param mBlock block index on memory card.
    */
    typedef struct CARDInfo
    {
        s32 mChannel;
        s32 mFileNo;
        s32 mOffset;
        s32 mLength;
        u16 mBlock;
    } CARDInfo;

    void CARDInit();

    void CARDCheck(s32 chan);
    s32 CARDProbeEx(s32 chan, s32 *memSize, s32 *sectSize);
    s32 CARDGetStatus(s32 chan, s32 fileNo, CARDInfo *stats);

    s32 CARDSetStatus(s32 chan, s32 fileNo, CARDInfo *stats);
    s32 CARDSetStatusAsync(s32 chan, s32 fileNo, CARDInfo *stats, CARDCallback cb);

    s32 CARDCreate(s32 chan, const char *fileName, s32 size, CARDInfo *file);
    s32 CARDCreateAsync(s32 chan, const char *fileName, s32 size, CARDInfo *file, CARDCallback cb);

    s32 CARDFormat(s32 chan);
    s32 CARDFormatAsync(s32 chan, CARDCallback cb);

    void CARDMount(s32 chan, void *workArea, CARDCallback detachCB);
    void CARDMountAsync(s32 chan, void *workArea, CARDCallback detachCB, CARDCallback attachCB);
    s32 CARDUnmount(s32 chan);

    s32 CARDRead(CARDInfo *file, void *buffer, u32 len, u32 ofs);
    s32 CARDReadAsync(CARDInfo *file, void *buffer, u32 len, u32 ofs, CARDCallback cb);

    s32 CARDWrite(CARDInfo *file, void *buffer, u32 len, u32 ofs);
    s32 CARDWriteAsync(CARDInfo *file, void *buffer, u32 len, u32 ofs, CARDCallback cb);

    s32 CARDOpen(s32 chan, const char *filename, CARDInfo *file);
    s32 CARDClose(CARDInfo *file);

    s32 CARDFreeBlocks(s32 chan, u16 *freeBlocks, void *unk);

#ifdef __cplusplus
}
#endif

#endif