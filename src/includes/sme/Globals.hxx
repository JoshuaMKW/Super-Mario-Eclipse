#pragma once

#include "types.h"
#include "defines.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/option/CardManager.hxx"
#include "sms/SMS.hxx"

#include "LightContext.hxx"
#include "collision/WarpCollision.hxx"
#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

namespace SME {

struct TGlobals {
public:
  static u8 getActivePlayers() { return sActivePlayers; }
  static u8 getMaxPlayers() { return sMaxPlayers; }
  static TMario *getPlayerByIndex(u8 index);
  static SME::Class::TSMEFile *getStageConfig() { return sStageConfig; }
  static SME::Class::TPlayerParams *getPlayerParams(u8 id);
  static SME::Class::TPlayerParams *getPlayerParams(TMario *player);

  static void setPlayerByIndex(u8 index, TMario *player);

  static void registerPlayerParams(SME::Class::TPlayerParams *params);
  static void deregisterPlayerParams(SME::Class::TPlayerParams *params);
  static void clearAllPlayerParams();

  static bool isCompletionRewarded() {
    TCardBookmarkInfo *bookmarkInfo = gpCardManager->getBookmarkInfos_();
    if (!bookmarkInfo)
        return false;

    for (u32 i = 0; i < 3; ++i) {
      if (bookmarkInfo->FileData[i].mShineCount >= SME_MAX_SHINES)
        return true;
    }
    return false;
  }

  static bool isFreePlayMode() { return sIsFreePlay; }
  static bool isMultiplayerActive() { return sActivePlayers > 1; }
  static bool isMusicBeingStreamed() { return sIsAudioStreaming; }
  static bool isMusicStreamingAllowed() { return sIsAudioStreamAllowed; }
  static bool areCodesPresent() { return sPlayerHasGeckoCodes; }

  static TLightContext sLightData;

  static void *sPRMFile;
  static SME::Class::TWarpCollisionList *sWarpColArray;
  static SME::Class::TWarpCollisionList *sWarpColPreserveArray;

  #ifdef SME_DETACHED_HEAPS
  static JKRExpHeap sCharacterHeap;
  static JKRExpHeap sGlobalHeap;
  #else
  static JKRExpHeap *sCharacterHeap;
  static JKRExpHeap *sGlobalHeap;
  #endif

//private:
  static SME::Class::TSMEFile *sStageConfig;
  static SME::Class::TPlayerParams *sPlayerCfgArray[SME_MAX_PLAYERS];
  static TMario *sPlayers[SME_MAX_PLAYERS];
  static bool sPlayerHasGeckoCodes;
  static bool sIsAudioStreaming;
  static bool sIsAudioStreamAllowed;
  static bool sIsFreePlay;
  static u8 sActivePlayers;
  static u8 sMaxPlayers;
};

} // namespace SME