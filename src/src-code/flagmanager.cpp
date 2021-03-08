#include "SME.hxx"

//0x80294EF4
void resetStage(TFlagManager *flagManager)
{
    size_t mainResetSize = (SME_MAX_SHINES - 120) < 0 ? 0x180 : 0x100; /*0x180 - ((SME_MAX_SHINES - 120) >> 3);*/
    OSReport("TFlagManager::resetStage() - Resetting field at %p, size = 0x%X\n", (u8*)&flagManager->Type1Flag + 0xF4, mainResetSize);
    memset(((u8*)&flagManager->Type1Flag) + 0xE4, 0, 0xD);
    memset(((u8*)&flagManager->Type1Flag) + 0xF4, 0, mainResetSize); //OG = 0x18C
    memset(((u8*)&flagManager->Type1Flag) + 0x11F, 0, 1);
}
kmBranch(0x80294EF4, &resetStage);

void resetStageFlags(TFlagManager *flagManager)
{
    TFlagManager::smInstance->resetStage();
}
//kmBranch(0x80294EF4, &resetStageFlags);

void TFlagManager::resetGame()
{
    memset(this + 0xE4, 0, 0xD);
}