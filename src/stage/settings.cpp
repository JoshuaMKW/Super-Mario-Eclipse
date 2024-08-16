#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>

#include "settings.hxx"

extern Settings::SettingsGroup gSettingsGroup;
extern BugsExploitsSetting gBugsSetting;
extern MirrorModeFlag gMirrorModeSetting;

static const char *sDirectors[] = {"JoshuaMK", "Tempo", "UncleMeat", "epicwade"};

class DirectorCreditSetting final : public Settings::IntSetting {
public:
    DirectorCreditSetting() : IntSetting("Directors", &mIndex) {
        mValueRange = {0, (sizeof(sDirectors) / sizeof(const char *)) - 1, 1};
    }
    ~DirectorCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sDirectors[mIndex]); }

private:
    int mIndex;
};

static const char *sProgrammers[] = {"JoshuaMK", "CyrusLoS", "Adam Tollios"};

class ProgrammerCreditSetting final : public Settings::IntSetting {
public:
    ProgrammerCreditSetting() : IntSetting("Programmers", &mIndex) {
        mValueRange = {0, (sizeof(sProgrammers) / sizeof(const char *)) - 1, 1};
    }
    ~ProgrammerCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sProgrammers[mIndex]); }

private:
    int mIndex;
};

static const char *sStageDesigners[] = {"UncleMeat", "Tempo", "epicwade"};

class StageCreditSetting final : public Settings::IntSetting {
public:
    StageCreditSetting() : IntSetting("Stages", &mIndex) {
        mValueRange = {0, (sizeof(sStageDesigners) / sizeof(const char *)) - 1, 1};
    }
    ~StageCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sStageDesigners[mIndex]); }

private:
    int mIndex;
};

static const char *sComposers[] = {"Trevor Scott", "StevenShockley"};

class ComposerCreditSetting final : public Settings::IntSetting {
public:
    ComposerCreditSetting() : IntSetting("Composers", &mIndex) {
        mValueRange = {0, (sizeof(sComposers) / sizeof(const char *)) - 1, 1};
    }
    ~ComposerCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sComposers[mIndex]); }

private:
    int mIndex;
};

static const char *sSoundDesigners[] = {"Trevor Scott", "StevenShockley"};

class SoundDesignCreditSetting final : public Settings::IntSetting {
public:
    SoundDesignCreditSetting() : IntSetting("Sound Design", &mIndex) {
        mValueRange = {0, (sizeof(sSoundDesigners) / sizeof(const char *)) - 1, 1};
    }
    ~SoundDesignCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sSoundDesigners[mIndex]); }

private:
    int mIndex;
};

static const char *sTextures[] = {"UncleMeat", "Fecal-Matter-Photoshops", "ShaneMGD"};

class TextureCreditSetting final : public Settings::IntSetting {
public:
    TextureCreditSetting() : IntSetting("Textures", &mIndex) {
        mValueRange = {0, (sizeof(sTextures) / sizeof(const char *)) - 1, 1};
    }
    ~TextureCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sTextures[mIndex]); }

private:
    int mIndex;
};

static const char *sCharacters[] = {"UncleMeat", "MasterMario777", "StevenShockley"};

class CharacterCreditSetting final : public Settings::IntSetting {
public:
    CharacterCreditSetting() : IntSetting("Characters", &mIndex) {
        mValueRange = {0, (sizeof(sCharacters) / sizeof(const char *)) - 1, 1};
    }
    ~CharacterCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sCharacters[mIndex]); }

private:
    int mIndex;
};

static const char *sSpecialThanks[] = {
    "Xayr", "Halleester",  "Portable Productions", "Henk_Wasmachine",
    "Gonz", "Truegamer02", "zachthepillow",        "MissB"};

class SpecialThanksCreditSetting final : public Settings::IntSetting {
public:
    SpecialThanksCreditSetting() : IntSetting("Special Thanks", &mIndex) {
        mValueRange = {0, (sizeof(sSpecialThanks) / sizeof(const char *)) - 1, 1};
    }
    ~SpecialThanksCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sSpecialThanks[mIndex]); }

private:
    int mIndex;
};

DirectorCreditSetting sDirectorCreditSetting;
ProgrammerCreditSetting sProgrammerCreditSetting;
StageCreditSetting sStageCreditSetting;
ComposerCreditSetting sComposerCreditSetting;
SoundDesignCreditSetting sSoundDesignCreditSetting;
TextureCreditSetting sTextureCreditSetting;
CharacterCreditSetting sCharacterCreditSetting;
SpecialThanksCreditSetting sSpecialThanksCreditSetting;

void initDemoCredits(Settings::SettingsGroup &group) {
    group.addSetting(&sDirectorCreditSetting);
    group.addSetting(&sProgrammerCreditSetting);
    group.addSetting(&sStageCreditSetting);
    group.addSetting(&sComposerCreditSetting);
    group.addSetting(&sSoundDesignCreditSetting);
    group.addSetting(&sTextureCreditSetting);
    group.addSetting(&sCharacterCreditSetting);
    group.addSetting(&sSpecialThanksCreditSetting);
}

bool isGameCompletedPrior() { return gBugsSetting.isUnlocked(); }

void lockModuleSettings(TApplication *app) {
    optional<ModuleInfo> engine_module   = BetterSMS::getModuleInfo("Better Sunshine Engine");
    optional<ModuleInfo> movement_module = BetterSMS::getModuleInfo("Better Sunshine Moveset");

    /*SMS_ASSERT(movement_module,
               "Super Mario Eclipse requires the Better Sunshine Movement module to be present and "
               "loaded. Please restore \"BetterSunshineMovement.kxe\" to \"./Kuribo!/Mods/\"!");*/

    Settings::SettingsGroup *engine_settings = engine_module->mSettings;
    {
        {
            auto *setting =
                reinterpret_cast<BugsSetting *>(engine_settings->getSetting("Bug Fixes"));
            setting->setBool(true);
            setting->lock();
        }
        {
            auto *setting =
                reinterpret_cast<BugsSetting *>(engine_settings->getSetting("Exploit Fixes"));
            setting->lock();
        }

        {
            auto *setting = reinterpret_cast<CollisionFixesSetting *>(
                engine_settings->getSetting("Collision Fixes"));
            setting->setBool(true);
            setting->lock();
        }
    }

    if (!isGameCompletedPrior()) {
        {
            auto *setting = engine_settings->getSetting("Exploit Fixes");
            setting->setBool(true);
        }

        if (movement_module) {
            auto *movement_settings = movement_module->mSettings;

            {
                auto *setting = movement_settings->getSetting("Long Jump");
                setting->setInt(2);
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

            {
                auto *setting = movement_settings->getSetting("Fall Damage");
                setting->setInt(1);
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }
        }
    }

    if (BetterSMS::isModuleRegistered("Mirror Mode") && !gMirrorModeSetting.getBool()) {
        {
            optional<ModuleInfo> mirror_module       = BetterSMS::getModuleInfo("Mirror Mode");
            Settings::SettingsGroup *mirror_settings = mirror_module->mSettings;

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

    bool update_save = false;

    if (shine_count >= MaxShineCount && !gBugsSetting.isUnlocked()) {
        gBugsSetting.unlock();

        if (optional<ModuleInfo> movement_module =
                BetterSMS::getModuleInfo("Better Sunshine Moveset")) {
            Settings::SettingsGroup *movement_settings = movement_module->mSettings;

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

            {
                auto *setting = movement_settings->getSetting("Fall Damage");
                setting->setUserEditable(true, BetterSMS::Settings::Priority::GAME);
            }
        }

        update_save = true;
    }

    if (shine_count >= MaxShineCount && !gMirrorModeSetting.getBool()) {
        gMirrorModeSetting.setBool(true);
        gMirrorModeSetting.updateSetting();
        update_save = true;
    }

    if (update_save) {
        s32 cardStatus = Settings::mountCard();
        if (cardStatus >= CARD_ERROR_READY) {
            Settings::saveSettingsGroup(gSettingsGroup);
            Settings::unmountCard();
        }
    }
}

void forceYoshiUnlock(TMarDirector *director) { TFlagManager::smInstance->setBool(true, 0x1038F); }
// SMS_WRITE_32(0x801bbfac, 0x38600001);