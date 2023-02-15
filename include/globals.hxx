#pragma once

#include <Dolphin/types.h>
#include <JSystem/JGeometry/JGMVec.hxx>
#include <JSystem/JKernel/JKRHeap.hxx>
#include <SMS/macros.h>
#include <SMS/Player/Mario.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "darkness.hxx"
#include "player.hxx"

namespace SME {

struct TGlobals {
	static CharacterID getCharacterIDFromPlayer(TMario *player) { return sCharacterIDList[0]; }

	static bool isFreePlayMode() { return sIsFreePlay; }

	static u8 getMinDarkness() { return sMinDarkness; }
	static void setMinDarkness(u8 val) { sMinDarkness = val; }

	static void *sPRMFile;

	static CharacterID sCharacterIDList[4];
	static u8 sMinDarkness;

	static bool sIsFreePlay;
};

} // namespace SME