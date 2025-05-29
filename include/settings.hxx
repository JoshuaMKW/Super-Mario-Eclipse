#pragma once

#include <Dolphin/CARD.h>
#include <Dolphin/types.h>

#include <BetterSMS/memory.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/settings.hxx>

constexpr size_t MaxShineCount = 240;

using namespace BetterSMS;

extern const u8 gSaveBnr[];
extern const u8 gSaveIcon[];

class BugsExploitsSetting final : public Settings::SwitchSetting {
public:
    BugsExploitsSetting() : SwitchSetting("Bugs/Exploits", &mBugsExploitsValue) {
        mValueChangedCB = valueChanged;
    }
    ~BugsExploitsSetting() override {}

    bool isUnlocked() const override { return sIsUnlocked; }

    void load(JSUMemoryInputStream &in) override {
        in.read(&sIsUnlocked, 1);
        {
            bool b;
            in.read(&b, 1);
            setBool(b);
        }
    }

    void save(JSUMemoryOutputStream &out) override {
        out.write(&sIsUnlocked, 1);
        out.write(&mBugsExploitsValue, 1);
    }

    void getValueName(char *dst) const override {
        getBool() ? strncpy(dst, "ON", 4) : strncpy(dst, "OFF", 5);
    }

    inline void lock() { sIsUnlocked = false; }
    inline void unlock() { sIsUnlocked = true; }

    // clang-format off
    static void valueChanged(void* old, void* cur, ValueKind kind) {
        ModuleInfo engine_module = BetterSMS::getModuleInfo("Better Sunshine Engine").value();

        SingleSetting *exploits_setting = engine_module.mSettings->getSetting("Exploit Fixes");
        SingleSetting *collision_setting = engine_module.mSettings->getSetting("Collision Fixes");

        const bool is_unlocked = *reinterpret_cast<bool *>(cur);
        
        if (exploits_setting)
            exploits_setting->setBool(!is_unlocked);
        
        if (collision_setting)
            collision_setting->setBool(!is_unlocked);
    }
    // clang-format on

private:
    bool mBugsExploitsValue = false;
    static bool sIsUnlocked;
};

// Invisible flag that doesn't show in settings menu
class MirrorModeFlag final : public Settings::SwitchSetting {
public:
    MirrorModeFlag() : SwitchSetting("__mirror_mode_unlocked", &mMirrorModeFlag) {}
    ~MirrorModeFlag() override {}

    void load(JSUMemoryInputStream &in) override {
        SwitchSetting::load(in);
        updateSetting();
    }

    bool isUnlocked() const override { return false; }

    bool isUnlocked_() const { return mMirrorModeFlag; }

    void updateSetting() const {
        optional<ModuleInfo> mirror_module = BetterSMS::getModuleInfo("Mirror Mode");
        if (!mirror_module)
            return;

        auto *active_setting = mirror_module->mSettings->getSetting("Is Enabled");
        if (active_setting) {
            active_setting->setUserEditable(mMirrorModeFlag, BetterSMS::Settings::Priority::GAME);
            if (!mMirrorModeFlag)
                active_setting->setBool(false);
        }
    }

private:
    bool mMirrorModeFlag = false;
};

class TutorialSetting final : public Settings::SwitchSetting {
public:
    TutorialSetting() : SwitchSetting("__tutorial_flag", &mTutorialValue) {}
    ~TutorialSetting() override {}

    bool isUnlocked() const override { return false; }

    void load(JSUMemoryInputStream &in) override {
        bool b;
        in.read(&b, 1);
        setBool(b);
    }

    void save(JSUMemoryOutputStream &out) override { out.write(&mTutorialValue, 1); }

private:
    bool mTutorialValue = false;
};

class DarknessSetting final : public Settings::IntSetting {
public:
    enum Kind { REGULAR, ECLIPSED, VANILLA };

    DarknessSetting() : IntSetting("Darkness", &mDarknessValue) { mValueRange = {0, 2, 1};
    }
    ~DarknessSetting() override {}

    void getValueName(char *dst) const override {
        switch (getInt()) {
        case Kind::REGULAR:
            strncpy(dst, "REGULAR", 9);
            break;
        case Kind::ECLIPSED:
            strncpy(dst, "ECLIPSED", 10);
            break;
        case Kind::VANILLA:
            strncpy(dst, "VANILLA", 8);
            break;
        }
    }

    void load(JSUMemoryInputStream &in) override {
        int x;
        in.read(&x, 4);
        if (x < REGULAR || x > VANILLA)
            x = REGULAR;  // Reset value if corrupt
        setInt(x);
    }

    void save(JSUMemoryOutputStream &out) override { out.write(&mDarknessValue, 4); }

private:
    int mDarknessValue = ECLIPSED;
};

class SpeedrunSetting final : public Settings::IntSetting {
public:
    enum Kind { NONE, ANY, SHINE_121, SHINE_197, SHINE_240, SHINE_240_EX, ILS };

    SpeedrunSetting() : IntSetting("Speedrun Mode", &mSpeedrunSetting), mSpeedrunSetting(NONE) {
        mValueRange = {0, 6, 1};
    }
    ~SpeedrunSetting() override {}

    void getValueName(char *dst) const override {
        switch (getInt()) {
        default:
        case Kind::NONE:
            strncpy(dst, "OFF", 4);
            break;
        case Kind::ANY:
            strncpy(dst, "ANY PERCENT", 12);
            break;
        case Kind::SHINE_121:
            strncpy(dst, "121 SHINES", 11);
            break;
        case Kind::SHINE_197:
            strncpy(dst, "197 SHINES", 11);
            break;
        case Kind::SHINE_240:
            strncpy(dst, "240 SHINES", 11);
            break;
        case Kind::SHINE_240_EX:
            strncpy(dst, "240ex PERCENT", 14);
            break;
        case Kind::ILS:
            strncpy(dst, "IL TIMER", 9);
            break;
        }
    }

private:
    int mSpeedrunSetting;
};
