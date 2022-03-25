#pragma once

#include "types.h"
#include "defines.h"
#include "JGeometry.hxx"
#include "JKR/JKRHeap.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/option/CardManager.hxx"
#include "sms/SMS.hxx"

#include "LightContext.hxx"
#include "collision/WarpCollision.hxx"
#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

namespace SME {

struct TGlobals {
  static u8 getActivePlayers() { return sActivePlayers; }
  static u8 getMaxPlayers() { return sMaxPlayers; }
  static SME_NO_INLINE TMario *getPlayerByIndex(u8 index);
  static SME_NO_INLINE SME::Class::TPlayerData *getPlayerData(u8 id);
  static SME_NO_INLINE SME::Class::TPlayerData *getPlayerData(TMario *player);
  static u8 getIDFromPlayer(TMario *player);

  static SME_NO_INLINE void setPlayerByIndex(u8 index, TMario *player);

  static SME_NO_INLINE void registerPlayerParams(SME::Class::TPlayerData *params);
  static SME_NO_INLINE void deregisterPlayerParams(SME::Class::TPlayerData *params);
  static SME_NO_INLINE void clearAllPlayerParams();

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

  static bool isDebugMode() { return sIsDebugMode; }
  static bool isWideScreen() { return sScreenWidth > 640.0f; }
  static bool isVariableFrameRate() { return sIsVariableFrameRate; }
  static bool isMirrorMode() { return sIsMirrorMode; }
  static bool isFreePlayMode() { return sIsFreePlay; }
  static bool isMultiplayerActive() { return sActivePlayers > 1; }
  static bool isMusicBeingStreamed() { return sIsAudioStreaming; }
  static bool isMusicStreamingAllowed() { return sIsAudioStreamAllowed; }
  static bool areCodesPresent() { return sPlayerHasGeckoCodes; }

  static f32 getScreenWidth() { return sScreenWidth; }
  static f32 getFrameRate() { return sFrameRate; }
  static f32 getScreenToFullScreenRatio() { return sScreenWidth / 600.0f; }
  static u8 getMinDarkness() { return sMinDarkness; }
  static Enum::UIKind getUIKind() { return sUIKind; }

  static void setDebugMode(bool active) { sIsDebugMode = active; }
  static void setScreenWidth(f32 width) { sScreenWidth = width; }
  static void setFrameRate(f32 framerate) { sFrameRate = framerate; }
  static void setVariableFrameRate(bool active) { sIsVariableFrameRate = active; }
  static void setMirrorMode(bool mirror) { sIsMirrorMode = mirror; }
  static void setMinDarkness(u8 val) { sMinDarkness = val; }
  static void setUIKind(Enum::UIKind kind) { sUIKind = kind; }

  static Class::TLightContext sLightData;

  static void *sPRMFile;
  static Class::TWarpCollisionList *sWarpColArray;
  static Class::TWarpCollisionList *sWarpColPreserveArray;

  #ifdef SME_DETACHED_HEAPS
  static JKRExpHeap sCharacterHeap;
  static JKRExpHeap sGlobalHeap;
  #else
  static JKRExpHeap *sCharacterHeap;
  static JKRExpHeap *sGlobalHeap;
  #endif

  static Class::TPlayerData *sPlayerCfgArray[SME_MAX_PLAYERS];
  static TMario *sPlayers[SME_MAX_PLAYERS];
  static bool sPlayerHasGeckoCodes;
  static bool sIsAudioStreaming;
  static bool sIsAudioStreamAllowed;
  static bool sIsFreePlay;
  static u8 sActivePlayers;
  static u8 sMaxPlayers;
  static Enum::Player sCharacterIDList[SME_MAX_PLAYERS];
  static Enum::UIKind sUIKind;
  static f32 sScreenWidth;
  static f32 sFrameRate;
  static bool sIsVariableFrameRate;
  static bool sIsMirrorMode;
  static bool sIsDebugMode;
  static u8 sMinDarkness;
};

} // namespace SME