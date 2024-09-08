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

static const char *sMarioBros[] = {"JoshuaMK", "Tempo", "Leif Erikson (FATHER)"};

class MarioBrosCreditSetting final : public Settings::IntSetting {
public:
    MarioBrosCreditSetting() : IntSetting("The Mario Bros", &mIndex) {
        mValueRange = {0, (sizeof(sMarioBros) / sizeof(const char *)) - 1, 1};
    }
    ~MarioBrosCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sMarioBros[mIndex]); }

private:
    int mIndex;
};

static const char *sDirectors[] = {"JoshuaMK", "Tempo", "UncleMeat", "Epicwade"};

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

static const char *sStageDesigners[] = {"UncleMeat", "Tempo", "Epicwade", "Portable Productions",
                                        "Brace"};

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

static const char *sComposers[] = {"Trevor Scott", "StevenShockley", "Portable Productions"};

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

static const char *sTextures[] = {"UncleMeat", "Some-Crappy-Edits", "ShaneMGD", "Purple Twirler"};

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

static const char *sAnimators[] = {"Fludd-Stop"};

class AnimatorsCreditSetting final : public Settings::IntSetting {
public:
    AnimatorsCreditSetting() : IntSetting("Animators", &mIndex) {
        mValueRange = {0, (sizeof(sAnimators) / sizeof(const char *)) - 1, 1};
    }
    ~AnimatorsCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sAnimators[mIndex]); }

private:
    int mIndex;
};

static const char *sBetaTesters[] = {"MasterMario777",
                                     "Flash",
                                     "Hailbot",
                                     "syrupyy",
                                     "timenoe",
                                     "Multi",
                                     "Ghostly",
                                     "Epicwade",
                                     "Leif Erikson (FATHER)",
                                     "Portable Productions",
                                     "Tempo",
                                     "Baris Eren Kaplan",
                                     "Bacon.24",
                                     "MondoKRUEL!",
                                     "StevenShockley"};

class BetaTestersCreditSetting final : public Settings::IntSetting {
public:
    BetaTestersCreditSetting() : IntSetting("Beta Testers", &mIndex) {
        mValueRange = {0, (sizeof(sBetaTesters) / sizeof(const char *)) - 1, 1};
    }
    ~BetaTestersCreditSetting() override {}

    void load(JSUMemoryInputStream &in) override {}
    void save(JSUMemoryOutputStream &out) override {}
    void getValueName(char *dst) const override { strcpy(dst, sBetaTesters[mIndex]); }

private:
    int mIndex;
};

static const char *sSpecialThanks[] = {
    "Xayr", "Halleester",  "Portable Productions", "Gamma", "Brace", "Henk_Wasmachine",
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

static Settings::SettingsGroup sCreditsGroup(1, 0, Settings::Priority::GAME);
static BetterSMS::ModuleInfo sModuleInfo("Super Mario Eclipse Credits", 1, 0, &sCreditsGroup);

static MarioBrosCreditSetting sMarioBrosCreditSetting;
static DirectorCreditSetting sDirectorCreditSetting;
static ProgrammerCreditSetting sProgrammerCreditSetting;
static StageCreditSetting sStageCreditSetting;
static ComposerCreditSetting sComposerCreditSetting;
static TextureCreditSetting sTextureCreditSetting;
static CharacterCreditSetting sCharacterCreditSetting;
static AnimatorsCreditSetting sAnimatorsCreditSetting;
static BetaTestersCreditSetting sBetaTestersCreditSetting;
static SpecialThanksCreditSetting sSpecialThanksCreditSetting;

void initDemoCredits() {
    sCreditsGroup.addSetting(&sMarioBrosCreditSetting);
    sCreditsGroup.addSetting(&sDirectorCreditSetting);
    sCreditsGroup.addSetting(&sProgrammerCreditSetting);
    sCreditsGroup.addSetting(&sStageCreditSetting);
    sCreditsGroup.addSetting(&sComposerCreditSetting);
    sCreditsGroup.addSetting(&sTextureCreditSetting);
    sCreditsGroup.addSetting(&sCharacterCreditSetting);
    sCreditsGroup.addSetting(&sAnimatorsCreditSetting);
    sCreditsGroup.addSetting(&sBetaTestersCreditSetting);
    sCreditsGroup.addSetting(&sSpecialThanksCreditSetting);
    sCreditsGroup.setIOValid(false);
    BetterSMS::registerModule(sModuleInfo);
}

bool isGameCompletedPrior() { return gBugsSetting.isUnlocked(); }

void lockModuleSettings(TApplication *app) {
    optional<ModuleInfo> engine_module   = BetterSMS::getModuleInfo("Better Sunshine Engine");
    optional<ModuleInfo> movement_module = BetterSMS::getModuleInfo("Better Sunshine Moveset");

    SMS_ASSERT(movement_module,
               "Super Mario Eclipse requires the Better Sunshine Movement module to be present and "
               "loaded. Please restore \"BetterSunshineMovement.kxe\" to \"./Kuribo!/Mods/\"!");

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
                setting->emit();
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Back Flip");
                setting->setBool(true);
                setting->emit();
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Hover Burst");
                setting->setBool(true);
                setting->emit();
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Hover Slide");
                setting->setBool(true);
                setting->emit();
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Rocket Dive");
                setting->setBool(true);
                setting->emit();
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Fast Turbo");
                setting->setBool(true);
                setting->emit();
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Fast Dive/Rollout");
                setting->setBool(true);
                setting->emit();
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }

            {
                auto *setting = movement_settings->getSetting("Fall Damage");
                setting->setInt(1);
                setting->emit();
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
                setting->emit();
                setting->setUserEditable(false, BetterSMS::Settings::Priority::GAME);
            }
        }
    }
}

void unlockSettings(TMarDirector *director) {
    size_t shine_count = TFlagManager::smInstance->getFlag(0x40000);

    bool update_save                   = false;

    optional<ModuleInfo> engine_module       = BetterSMS::getModuleInfo("Better Sunshine Engine");
    Settings::SettingsGroup *engine_settings = engine_module->mSettings;
    {
        {
            auto *setting =
                reinterpret_cast<BugsSetting *>(engine_settings->getSetting("Bug Fixes"));
            setting->setBool(true);
            setting->lock();
        }
    }

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