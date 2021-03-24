#pragma once

#include "sms/sound/MSBGM.hxx"

namespace SME::Util::Music {

bool isValidBGM(MSStageInfo musicID);
bool streamAudio(MSStageInfo ID, u8 type, bool loopAudio);

} // namespace SME::Util::Music