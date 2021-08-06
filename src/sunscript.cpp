#include "sms/spc/SpcBinary.hxx"

#include "sme/sunscript/common.hxx"
#include "SME.hxx"

using namespace SME;

#define BIND_SYMBOL(binary, symbol, func) (binary)->bindSystemDataToSymbol((symbol), reinterpret_cast<u32>(&(func)))
void Patch::Spc::initCustomFunctions(TSpcBinary *spcBinary, const char *symbol, u32 address) {
    spcBinary->bindSystemDataToSymbol(symbol, address);
    BIND_SYMBOL(spcBinary, "vectorTranslate", Util::Spc::vectorTranslate);
    BIND_SYMBOL(spcBinary, "vectorTranslatef", Util::Spc::vectorTranslatef);
    BIND_SYMBOL(spcBinary, "vectorScalef", Util::Spc::vectorScalef);
    BIND_SYMBOL(spcBinary, "vectorMagnitude", Util::Spc::vectorMagnitude);
    BIND_SYMBOL(spcBinary, "vectorNormalize", Util::Spc::vectorNormalize);
    BIND_SYMBOL(spcBinary, "setActorPosToOther", Util::Spc::setActorPosToOther);
    BIND_SYMBOL(spcBinary, "setActorRotToOther", Util::Spc::setActorRotToOther);
    BIND_SYMBOL(spcBinary, "getActorPos", Util::Spc::getActorPos);
    BIND_SYMBOL(spcBinary, "setActorPos", Util::Spc::setActorPos);
    BIND_SYMBOL(spcBinary, "setActorPosf", Util::Spc::setActorPosf);
    BIND_SYMBOL(spcBinary, "getActorRot", Util::Spc::getActorRot);
    BIND_SYMBOL(spcBinary, "setActorRot", Util::Spc::setActorRot);
    BIND_SYMBOL(spcBinary, "setActorRotf", Util::Spc::setActorRotf);
    BIND_SYMBOL(spcBinary, "spawnObjByID", Util::Spc::spawnObjByID);
    BIND_SYMBOL(spcBinary, "isMultiplayerActive", Util::Spc::isMultiplayerActive);
    BIND_SYMBOL(spcBinary, "isFreePlayActive", Util::Spc::isFreePlayActive);
    BIND_SYMBOL(spcBinary, "isCompletionRewardActive", Util::Spc::isCompletionRewardActive);
    BIND_SYMBOL(spcBinary, "getActivePlayers", Util::Spc::getActivePlayers);
    BIND_SYMBOL(spcBinary, "getMaxPlayers", Util::Spc::getMaxPlayers);
    BIND_SYMBOL(spcBinary, "getPlayerByIndex", Util::Spc::getPlayerByIndex);
    BIND_SYMBOL(spcBinary, "getDateAsStr", Util::Spc::getDateAsStr);
    BIND_SYMBOL(spcBinary, "getTimeAsStr", Util::Spc::getTimeAsStr);
    BIND_SYMBOL(spcBinary, "getPlayerInputByIndex", Util::Spc::getPlayerInputByIndex);
    BIND_SYMBOL(spcBinary, "read8", Util::Spc::read8);
    BIND_SYMBOL(spcBinary, "read16", Util::Spc::read16);
    BIND_SYMBOL(spcBinary, "read32", Util::Spc::read32);
    BIND_SYMBOL(spcBinary, "write8", Util::Spc::write8);
    BIND_SYMBOL(spcBinary, "write16", Util::Spc::write16);
    BIND_SYMBOL(spcBinary, "write32", Util::Spc::write32);
    BIND_SYMBOL(spcBinary, "memcpy", Util::Spc::memcpy_);
    BIND_SYMBOL(spcBinary, "memmove", Util::Spc::memmove_);
    BIND_SYMBOL(spcBinary, "memcmp", Util::Spc::memcmp_);
    BIND_SYMBOL(spcBinary, "memset", Util::Spc::memset_);
    BIND_SYMBOL(spcBinary, "sprintf", Util::Spc::sprintf);
    BIND_SYMBOL(spcBinary, "formatStrBySpec", Util::Spc::formatStrBySpec);
    BIND_SYMBOL(spcBinary, "getStageBGM", Util::Spc::getStageBGM);
    BIND_SYMBOL(spcBinary, "queueStream", Util::Spc::queueStream);
    BIND_SYMBOL(spcBinary, "playStream", Util::Spc::playStream);
    BIND_SYMBOL(spcBinary, "pauseStream", Util::Spc::pauseStream);
    BIND_SYMBOL(spcBinary, "stopStream", Util::Spc::stopStream);
    BIND_SYMBOL(spcBinary, "seekStream", Util::Spc::seekStream);
    BIND_SYMBOL(spcBinary, "nextStream", Util::Spc::nextStream);
    BIND_SYMBOL(spcBinary, "skipStream", Util::Spc::skipStream);
    BIND_SYMBOL(spcBinary, "getStreamVolume", Util::Spc::getStreamVolume);
    BIND_SYMBOL(spcBinary, "setStreamVolume", Util::Spc::setStreamVolume);
    BIND_SYMBOL(spcBinary, "getStreamLooping", Util::Spc::getStreamLooping);
    BIND_SYMBOL(spcBinary, "setStreamLooping", Util::Spc::setStreamLooping);
}
#undef BIND_SYMBOL