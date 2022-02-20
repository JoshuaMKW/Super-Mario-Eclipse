#pragma once

#include "types.h"
#include "JKR/JKRHeap.hxx"
#include "JSU/JSUMemoryStream.hxx"

class TFlagManager {

public:
  void correctFlag();
  void correctOptFlag();
  void decFlag(u32 flag, s32 num);
  void firstStart();
  bool getBlueCoinFlag(u8 stageID, u8) const;
  bool getBool(u32 flag) const;
  u32 getFlag(u32 flag) const;
  bool getShineFlag(u8 stageID) const;
  u8 getNozzleRight(u8, u8) const;
  void incFlag(u32 flag, s32 num);
  void incGoldCoinFlag(u8 stageID, s32 num);
  void incMario(s32 num);
  void load(JSUMemoryInputStream &);
  void loadOption(JSUMemoryInputStream &);
  void resetCard();
  void resetGame();
  void resetOpt();
  void resetStage();
  void restore();
  void save(JSUMemoryOutputStream &);
  void saveFail();
  void saveOption(JSUMemoryOutputStream &);
  void saveSuccess();
  void setBlueCoinFlag(u8 stageID, u8);
  void setBool(bool, u32 flag);
  void setFlag(u32 flag, s32 value);
  void setShineFlag(u8 stageID);
  void setNozzleRight(u8, u8);
  void start(JKRHeap *);

  struct {
    u8 m1Type[0x76]; // 0x0000
    u16 _00;         // 0x0076
  } Type1Flag;

  struct {
    s32 mSaveCount;         // 0x0078
    s32 mLifeCount;         // 0x007C
    s32 mGelato5Record;     // 0x0080
    s32 mPianta2Record;     // 0x0084
    s32 _01[0x8 / 4];       // 0x0088
    s32 mDelfinoCoinRecord; // 0x0090
    s32 mBiancoCoinRecord;  // 0x0094
    s32 mRiccoCoinRecord;   // 0x0098
    s32 mGelatoCoinRecord;  // 0x009C
    s32 mPinnaCoinRecord;   // 0x00A0
    s32 mSirenaCoinRecord;  // 0x00A4
    s32 mPiantaCoinRecord;  // 0x00A8
    s32 mNokiCoinRecord;    // 0x00AC
    s32 _02[0x18 / 4];      // 0x00B0
    s32 mNoki5Record;       // 0x00C8
  } Type2Flag;

  struct {
    u8 _00 : 2; // 0x00CC
    bool mLostLifePrev : 1;
    bool mPlazaDemoWatched : 1;
    bool mWatchPeachKidnappedPrev : 1;
    bool mPpdBJRBalloonsPrev : 1;
    bool mShineGetPrev : 1;
    u8 _01 : 2;
    bool mPlaneCrashWatched : 1; // 0x00CD
    u8 _02 : 1;
    bool mCourtWatched : 1;
    bool mPeachKNAPFMVWatched : 1;
    bool mFluddTheftViewed : 1;
    u8 _03 : 1;
    u8 _04[2]; // 0x00CE
  } Type3Flag;

  struct {
    s32 mShineCount;    // 0x00D0
    s32 mBlueCoinCount; // 0x00D4
    s32 mGoldCoinCount; // 0x00D8
    s32 mEpisodeNumber; // 0x00DC
    s32 mSecondNozzle;  // 0x00E0
  } Type4Flag;

  struct {
    bool mShineSpawned : 1; // 0x00E4
    bool mRiccoUnlockable : 1;
    bool mGelatoUnlockable : 1;
    bool mSunflowersRescue : 1;
    bool mNokiAvailable : 1;
    bool mPiantismoRaceComplete : 1;
    bool _00 : 1;
    bool mMantaSpawned : 1;
    bool mHotelRising : 1; // 0x00E5
    bool mRedCoinSwitchPressed : 1;
    bool mMechaBowserDefeated : 1;
    bool mWigglerFalling : 1;
    bool mMoleDefeated : 1;
    u8 _01 : 3;
    u8 _02[0xE]; // 0x00E6
  } Type5Flag;

  struct {
    s32 mRedCoinCount;        // 0x00F4
    s32 mBJRBalloonCount;     // 0x00F8
    s32 _00;                  // 0x00FC
    s32 mShadowMarioEvent;    // 0x0100
    u32 _01[0xF0 / 4];        // 0x0104
    u8 mCustomShineIDs[0x80]; // 0x01F4
    // TODO: RESIZE BASED ON `SME_MAX_SHINES'
    struct {
      bool mIsGamePlus : 1; // 0x0274
      u8 mHUDElement : 4;
      u32 _00 : 27;
      u32 _01 : 32;
      u32 _02 : 32;
    } CustomFlags;
  } Type6Flag;

  struct {
    bool mRumbleDisabled : 1; // 0x0284
    bool mSurroundSound : 1;
    bool mSubtitlesDisabled : 1;
    u8 _00 : 5;
  } Type7Flag;

  struct {
    s32 mLanguage; // 0x0288
  } Type8Flag;

  struct {
    bool mRumbleEnabled : 1; // 0x028C
    bool mSubtitlesEnabled : 1;
  } Type9Flag;

  struct {
    s32 mSoundMode; // 0x0290
    s32 mLanguage;  // 0x0294
  } TypeAFlag;

  s64 mTimeOnLastSave;      // 0x0298
  s64 mTimeOnLastSaveCopy;  // 0x02A0
  u8 mType1FlagSave[120];   // 0x02A8
  u8 mType2FlagSave[84];    // 0x0320
  s64 mSavedTimeOnLastSave; // 0x0378

  static TFlagManager *smInstance;
};

/*
void TFlagManager::incGoldCoinFlag(u8 stageID, s32 num) {
    constexpr u32 coin_id = 0x40002;

    incFlag(coin_id, num);

    u32 stageflag = 0x20000 + stageID;

    const int coinRecord = getFlag(stageflag);
    const int currentcoin = getFlag(coin_id);

    if (currentcoin > coinRecord) {
        setFlag(stageflag, currentcoin);
    }
}
*/