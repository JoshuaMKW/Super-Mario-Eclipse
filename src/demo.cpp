#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>
#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>

#include <BetterSMS/module.hxx>

#include "settings.hxx"

extern BugsExploitsSetting gBugsSetting;
extern MirrorModeFlag gMirrorModeSetting;

static const char *sDirectors[] = {"JoshuaMK", "Tempo", "UncleMeat", "epicwade", "ddumpy"};

class DirectorCreditSetting final : public Settings::IntSetting {
public:
    DirectorCreditSetting() : IntSetting("Directors", &mIndex) {
        mValueRange = {0, (sizeof(sDirectors) / sizeof(const char *)) - 1, 1};
    }
    ~DirectorCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sDirectors[mIndex]); }

private:
    int mIndex;
};

static const char *sProgrammers[] = {"JoshuaMK", "CyrusLoS", "Halleester"};

class ProgrammerCreditSetting final : public Settings::IntSetting {
public:

    ProgrammerCreditSetting() : IntSetting("Programmers", &mIndex) {
        mValueRange = {0, (sizeof(sProgrammers) / sizeof(const char *)) - 1, 1};
    }
    ~ProgrammerCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sProgrammers[mIndex]); }

private:
    int mIndex;
};

static const char *sStageDesigners[] = {"UncleMeat", "epicwade", "Tempo"};

class StageCreditSetting final : public Settings::IntSetting {
public:
    StageCreditSetting() : IntSetting("Stages", &mIndex) {
        mValueRange = {0, (sizeof(sStageDesigners) / sizeof(const char *)) - 1, 1};
    }
    ~StageCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sStageDesigners[mIndex]); }

private:
    int mIndex;
};

static const char *sComposers[] = {"ddumpy", "Sherman Haynes"};

class ComposerCreditSetting final : public Settings::IntSetting {
public:
    ComposerCreditSetting() : IntSetting("Composers", &mIndex) {
        mValueRange = {0, (sizeof(sComposers) / sizeof(const char *)) - 1, 1};
    }
    ~ComposerCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sComposers[mIndex]); }

private:
    int mIndex;
};

static const char *sSoundDesigners[] = {"ddumpy", "Purple Twirler"};

class SoundDesignCreditSetting final : public Settings::IntSetting {
public:
    SoundDesignCreditSetting() : IntSetting("Sound Design", &mIndex) {
        mValueRange = {0, (sizeof(sSoundDesigners) / sizeof(const char *)) - 1, 1};
    }
    ~SoundDesignCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sSoundDesigners[mIndex]); }

private:
    int mIndex;
};

static const char *sSequencers[] = {"Alex Benito", "ddumpy", "Purple Twirler"};

class SequencingCreditSetting final : public Settings::IntSetting {
public:
    SequencingCreditSetting() : IntSetting("Sequencing", &mIndex) {
        mValueRange = {0, (sizeof(sSequencers) / sizeof(const char *)) - 1, 1};
    }
    ~SequencingCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sSequencers[mIndex]); }

private:
    int mIndex;
};

static const char *sTextures[] = {"Purple Twirler", "Fecal-Matter-Photoshops", "UncleMeat"};

class TextureCreditSetting final : public Settings::IntSetting {
public:
    TextureCreditSetting() : IntSetting("Textures", &mIndex) {
        mValueRange = {0, (sizeof(sTextures) / sizeof(const char *)) - 1, 1};
    }
    ~TextureCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sTextures[mIndex]); }

private:
    int mIndex;
};

static const char *sCharacters[] = {"GamerSubZero", "UncleMeat"};

class CharacterCreditSetting final : public Settings::IntSetting {
public:
    CharacterCreditSetting() : IntSetting("Characters", &mIndex) {
        mValueRange = {0, (sizeof(sCharacters) / sizeof(const char *)) - 1, 1};
    }
    ~CharacterCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sCharacters[mIndex]); }

private:
    int mIndex;
};

static const char *sSpecialThanks[] = {"Xayr", "ShaneMGD", "Arie"};

class SpecialThanksCreditSetting final : public Settings::IntSetting {
public:
    SpecialThanksCreditSetting() : IntSetting("Special Thanks", &mIndex) {
        mValueRange = {0, (sizeof(sSpecialThanks) / sizeof(const char *)) - 1, 1};
    }
    ~SpecialThanksCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueStr(char *dst) const override { strcpy(dst, sSpecialThanks[mIndex]); }

private:
    int mIndex;
};

DirectorCreditSetting sDirectorCreditSetting;
ProgrammerCreditSetting sProgrammerCreditSetting;
StageCreditSetting sStageCreditSetting;
ComposerCreditSetting sComposerCreditSetting;
SoundDesignCreditSetting sSoundDesignCreditSetting;
SequencingCreditSetting sSequencingCreditSetting;
TextureCreditSetting sTextureCreditSetting;
CharacterCreditSetting sCharacterCreditSetting;
SpecialThanksCreditSetting sSpecialThanksCreditSetting;

void initDemoCredits(Settings::SettingsGroup &group) {
    group.addSetting(&sDirectorCreditSetting);
    group.addSetting(&sProgrammerCreditSetting);
    group.addSetting(&sStageCreditSetting);
    group.addSetting(&sComposerCreditSetting);
    group.addSetting(&sSoundDesignCreditSetting);
    group.addSetting(&sSequencingCreditSetting);
    group.addSetting(&sTextureCreditSetting);
    group.addSetting(&sCharacterCreditSetting);
    group.addSetting(&sSpecialThanksCreditSetting);
}

bool isDemoComplete() { return gBugsSetting.isUnlocked(); }

void lockModuleSettings(TApplication *app) {
    SMS_ASSERT(BetterSMS::isModuleRegistered("Better Sunshine Moveset"),
               "Super Mario Eclipse requires the Better Sunshine Movement module to be present and "
               "loaded. Please restore \"BetterSunshineMovement.kxe\" to \"./Kuribo!/Mods/\"!");

    SMS_ASSERT(BetterSMS::isModuleRegistered("Mirror Mode"),
               "Super Mario Eclipse requires the Mirror Mode module to be present and "
                              "loaded. Please restore \"MirrorMode.kxe\" to \"./Kuribo!/Mods/\"!");

    auto *engine_module   = BetterSMS::getModuleInfo("Better Sunshine Engine");
    auto *movement_module = BetterSMS::getModuleInfo("Better Sunshine Moveset");
    auto *mirror_module = BetterSMS::getModuleInfo("Mirror Mode");

    auto *engine_settings = engine_module->mSettings;
    {
        auto *setting = engine_settings->getSetting("Bug Fixes");
        setting->setBool(true);
    }

    if (!isDemoComplete()) {
        {
            auto *setting = engine_settings->getSetting("Exploit Fixes");
            setting->setBool(true);
        }

        {
            auto *setting = engine_settings->getSetting("Collision Fixes");
            setting->setBool(true);
        }

        {
            auto *movement_settings = movement_module->mSettings;

            {
                auto *setting = movement_settings->getSetting("Long Jump");
                setting->setBool(true);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Back Flip");
                setting->setBool(true);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Hover Burst");
                setting->setBool(true);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Hover Slide");
                setting->setBool(true);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Rocket Dive");
                setting->setBool(true);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Fast Turbo");
                setting->setBool(true);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Fast Dive/Rollout");
                setting->setBool(true);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }
        }
    }

    if (!gMirrorModeSetting.getBool()) {
        {
            auto *mirror_settings = mirror_module->mSettings;

            {
                auto *setting = mirror_settings->getSetting("Is Enabled");
                setting->setBool(false);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }
        }
    }
}

void unlockSettings(TMarDirector *director) {
    size_t shine_count = TFlagManager::smInstance->getFlag(0x40000);

    if (shine_count >= 43 && !gBugsSetting.isUnlocked()) {
        gBugsSetting.unlock();

        auto *movement_module = BetterSMS::getModuleInfo("Better Sunshine Moveset");
        {
            auto *movement_settings = movement_module->mSettings;

            {
                auto *setting = movement_settings->getSetting("Long Jump");
                setting->setUserEditable(true, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Back Flip");
                setting->setUserEditable(true, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Hover Burst");
                setting->setUserEditable(true, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Hover Slide");
                setting->setUserEditable(true, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Rocket Dive");
                setting->setUserEditable(true, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Fast Turbo");
                setting->setUserEditable(true, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Fast Dive/Rollout");
                setting->setUserEditable(true, BetterSMS::Settings::Priority::GAME);
            }
        }
    }

    if (shine_count >= 30 && !gMirrorModeSetting.getBool()) {
        gMirrorModeSetting.setBool(true);
        gMirrorModeSetting.updateSetting();
    }
}

void forceYoshiUnlock(TMarDirector* director) { TFlagManager::smInstance->setBool(true, 0x1038F); }
SMS_WRITE_32(0x801bbfac, 0x38600001);

void setPlayerPosRotOnLoad(TMario* player) {
    auto &cur_scene = gpApplication.mCurrentScene;
    auto &prev_scene = gpApplication.mPrevScene;

    if (cur_scene.mAreaID == 6 && cur_scene.mEpisodeID == 0 ||  // Daisy Cruiser
        cur_scene.mAreaID == 32 && cur_scene.mEpisodeID == 0 || // Flip Panel
        cur_scene.mAreaID == 14 && cur_scene.mEpisodeID == 1) { // Casino Table
        TFlagManager::smInstance->setFlag(0x40002, 0);
    }

    // Daisy Cruiser
    if (cur_scene.mAreaID == 6 && cur_scene.mEpisodeID == 0) {
        if (prev_scene.mAreaID == 3) {
            // Mario's Dream
            player->mTranslation = {-17030, 6518, 3750};
            player->mAngle.y     = -180 * 182;
        } else if ((prev_scene.mAreaID == 14 && prev_scene.mEpisodeID == 1) ||
                   (prev_scene.mAreaID == 41 && prev_scene.mEpisodeID == 0)) {
            // Casino
            player->mTranslation = {5500, 1476, -1530};
            player->mAngle.y     = 60 * 182;
        } else if (prev_scene.mAreaID == 40 && prev_scene.mEpisodeID == 0) {
            // Planes and Trains
            player->mTranslation = {6060, 6612, -960};
            player->mAngle.y     = -60 * 182;
        } else if (prev_scene.mAreaID != 6 ||
                   (prev_scene.mEpisodeID != 1 && prev_scene.mEpisodeID != 2)) {
            player->mTranslation = {22400, 5400, 0};
            player->mAngle.y     = 90 * 182;
        }
    } else if (cur_scene.mAreaID == 6 && (cur_scene.mEpisodeID == 1 || cur_scene.mEpisodeID == 2)) {
        if (prev_scene.mAreaID == 6 && prev_scene.mEpisodeID == 0) {
            player->mTranslation = {6965, 1390, 12730};
            player->mAngle.y     = 180 * 182;
        }
    } else if (cur_scene.mAreaID == 26 && cur_scene.mEpisodeID == 0) {
        // Vaporwave
        if (prev_scene.mAreaID == 25 && prev_scene.mEpisodeID == 0) {
            player->mTranslation = {-4450, 7130, -100};
            player->mAngle.y     = 60 * 182;
        }
    }
}

static void assignExitAreaStages(TApplication *app) {
    u32 *tpausemenu;
    SMS_FROM_GPR(29, tpausemenu);
    tpausemenu[0x104 / 4] = app->mCurrentScene.mAreaID == 6 ? 2 : 3;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80156B70, 0, 0, 0), assignExitAreaStages);
SMS_WRITE_32(SMS_PORT_REGION(0x80156B80, 0, 0, 0), 0x60000000);

static void assignExitAreaDestination(JDrama::TFlagT<u16> flags, u16 flag) {
    flags.set(flag);
    if (gpMarDirector->mAreaID == 6 &&
        (gpMarDirector->mEpisodeID == 1 || gpMarDirector->mEpisodeID == 2)) {
        gpApplication.mNextScene.mAreaID = 6;
        gpApplication.mNextScene.mEpisodeID = gpMarDirector->mEpisodeID;
        return;
    }

    if (gpMarDirector->mAreaID == 25 && gpMarDirector->mEpisodeID == 0) {
        gpApplication.mNextScene.mAreaID = 26;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    }

    if ((gpMarDirector->mAreaID == 4 && gpMarDirector->mEpisodeID == 5) || // Starshine Beach
        (gpMarDirector->mAreaID == 32 && gpMarDirector->mEpisodeID == 0) || // Flip Panel
        (gpMarDirector->mAreaID == 26 && gpMarDirector->mEpisodeID == 0) ||
        ((gpMarDirector->mAreaID == 13 || gpMarDirector->mAreaID == 50) && gpMarDirector->mEpisodeID == 0)) {
        gpApplication.mNextScene.mAreaID    = 6;
        gpApplication.mNextScene.mEpisodeID = 3;
        return;
    }

}
SMS_PATCH_BL(SMS_PORT_REGION(0x80299808, 0, 0, 0), assignExitAreaDestination);

static void assignShineExitDestination(TApplication *app, u8 area, u8 episode,
                                       JDrama::TFlagT<u16> flag) {

    if (!(gpMarDirector->mCollectedShine->mType & 0x10)) {
        if (gpMarDirector->mAreaID == 6 &&
            (gpMarDirector->mEpisodeID == 1 || gpMarDirector->mEpisodeID == 2)) {
            area    = 6;
            episode = gpMarDirector->mEpisodeID;
        }
        if ((gpMarDirector->mAreaID == 4 && gpMarDirector->mEpisodeID == 5) ||   // Starshine Beach
            (gpMarDirector->mAreaID == 32 && gpMarDirector->mEpisodeID == 0) ||  // Flip Panel
            ((gpMarDirector->mAreaID == 25 || gpMarDirector->mAreaID == 26) &&
             gpMarDirector->mEpisodeID == 0) ||
            ((gpMarDirector->mAreaID == 13 || gpMarDirector->mAreaID == 50) &&
             gpMarDirector->mEpisodeID == 0)) {
            area    = 6;
            episode = 3;
        }
        app->mNextScene.set(area, episode, flag);
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80297C80, 0, 0, 0), assignShineExitDestination);
SMS_WRITE_32(SMS_PORT_REGION(0x80297C84, 0, 0, 0), 0x60000000);

static void assignGameOverDestination(TMarDirector* director) {
    if (gpApplication.mCurrentScene.mAreaID == 6 && gpApplication.mCurrentScene.mEpisodeID == 1)
        gpApplication.mNextScene = gpApplication.mCurrentScene;
    else
        gpApplication.mNextScene.set(6, 2, 0);
    director->moveStage();
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029933C, 0, 0, 0), assignGameOverDestination);


static void checkMariosDreamWarp(JDrama::TFlagT<u16> flags, u16 flag) {
    flags.set(flag);

    if (gpApplication.mNextScene.mAreaID != 3 || gpApplication.mNextScene.mEpisodeID != 255)
        return;

    if (TFlagManager::smInstance->getShineFlag(7)) {
        gpApplication.mNextScene.mEpisodeID = 1;
    } else {
        gpApplication.mNextScene.mEpisodeID = 0;
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029A3E0, 0, 0, 0), checkMariosDreamWarp);

static void checkIceStageSweat() {
    TMario *player;
    SMS_FROM_GPR(31, player);

    if (gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0)
        return;

    *reinterpret_cast<u32 *>(&player->mAttributes) |= 0x20;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8024FC58, 0, 0, 0), checkIceStageSweat);

static void loadIceStageWalkSandA(JPAResourceManager *manager, const char *old_res, u16 id) {
    if (gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0)
    manager->load("ms_m_walksnow_a.jpa", id);
    else manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B3ED0, loadIceStageWalkSandA);

static void loadIceStageWalkSandB(JPAResourceManager *manager, const char *old_res, u16 id) {
    if (gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0)
        manager->load("ms_m_walksnow_b.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B3EF8, loadIceStageWalkSandA);

static void loadIceStageSlideSandA(JPAResourceManager *manager, const char *old_res, u16 id) {
    if (gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0)
    manager->load("ms_m_slidesnow_a.jpa", id);
    else manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B4448, loadIceStageWalkSandA);

static void loadIceStageSlideSandB(JPAResourceManager *manager, const char *old_res, u16 id) {
    if (gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0)
    manager->load("ms_m_slidesnow_b.jpa", id);
    else manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B4470, loadIceStageWalkSandA);

// On Airstrip from file select
SMS_WRITE_32(SMS_PORT_REGION(0x80164E30, 0, 0, 0), 0x38800701);
// On Delfino from file select
SMS_WRITE_32(SMS_PORT_REGION(0x80164E44, 0, 0, 0), 0x38800702);

// On Delfino Plaza from shine exit
SMS_WRITE_32(SMS_PORT_REGION(0x80297C2C, 0, 0, 0), 0x60000000);
SMS_WRITE_32(SMS_PORT_REGION(0x80297C4C, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x80297C54, 0, 0, 0), 0x38600000);

// On Delfino Plaza from death
SMS_WRITE_32(SMS_PORT_REGION(0x802997A4, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x802997AC, 0, 0, 0), 0x38000000);
SMS_WRITE_32(SMS_PORT_REGION(0x802997BC, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x802997C4, 0, 0, 0), 0x38600000);

// On Delfino Plaza from exit area
SMS_WRITE_32(SMS_PORT_REGION(0x802991E0, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x802991E8, 0, 0, 0), 0x38000000);
SMS_WRITE_32(SMS_PORT_REGION(0x802991F8, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x80299200, 0, 0, 0), 0x38600000);

static bool checkSpecialBlueCoinSound(MSound *msound, u32 soundID) {
    return msound->gateCheck(
        TFlagManager::smInstance->getFlag(0x40001) >= 10 ? MSound::SE_SHINE_TOUCH : soundID);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029A76C, 0, 0, 0), checkSpecialBlueCoinSound);

static bool sShineTradeReady = false;

static void makeSpecialBlueCoinSound(u32 soundID, const Vec *vec, u32 _0, JAISound **_1, u32 _2,
                                     u8 _3) {
    if (TFlagManager::smInstance->getFlag(0x40001) % 10 == 0) {
        MSoundSE::startSoundActor(MSound::SE_SHINE_TOUCH, vec, _0, _1, _2, _3);
        sShineTradeReady = true;
    } else {
        MSoundSE::startSoundActor(soundID, vec, _0, _1, _2, _3);
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029A76C, 0, 0, 0), makeSpecialBlueCoinSound);

void checkForBlueCoinTrade(TMarDirector *director) {
    if (sShineTradeReady) {
        for (int id = 70; id < 80; ++id) {
            if (TFlagManager::smInstance->getShineFlag(id))
                continue;
            TFlagManager::smInstance->setShineFlag(id);
            break;
        }
        sShineTradeReady = false;
    }
}

// Disable guide menu
SMS_WRITE_32(SMS_PORT_REGION(0x80297A64, 0, 0, 0), 0x4800000C);

// Disable fludd balloons
SMS_WRITE_32(0x8014A1EC, 0x4E800020);