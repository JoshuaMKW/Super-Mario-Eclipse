#include "includes/eclipse.hxx"

//0x80294EF4
void TFlagManager::resetStage()
{
    size_t mainResetSize = (MAX_SHINES - 120) < 0 ? 0x180 : 0x100; /*0x180 - ((MAX_SHINES - 120) >> 3);*/
    OSReport("TFlagManager::resetStage() - Resetting field at %X, size = %X\n", ((u8*)&this->Type1Flag) + 0xF4, mainResetSize);
    memset(((u8*)&this->Type1Flag) + 0xE4, 0, 0xD);
    memset(((u8*)&this->Type1Flag) + 0xF4, 0, mainResetSize); //OG = 0x18C
    memset(((u8*)&this->Type1Flag) + 0x11F, 0, 1);
}

void resetStageFlags(TFlagManager *gpFlagManager)
{
    gpFlagManager->resetStage();
}
kmBranch(0x80294EF4, &resetStageFlags);

void TFlagManager::resetGame()
{
    memset(this + 0xE4, 0, 0xD);
}