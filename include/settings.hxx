#pragma once

#include <Dolphin/CARD.h>
#include <Dolphin/types.h>

#include <BetterSMS/memory.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/settings.hxx>

constexpr size_t MaxShineCount = 300;

using namespace BetterSMS;

extern const u8 gSaveBnr[];
extern const u8 gSaveIcon[];

class HUDSetting final : public Settings::IntSetting {
public:
    enum HUD { SMS, E3, SPACEWORLD };

    HUDSetting() : IntSetting("HUD Style", &mHUDValue) {
        mValueRange     = {0, 2, 1};
        mValueChangedCB = valueChanged;
    }
    ~HUDSetting() override {}

    void getValueName(char *dst) const override {
        switch (mHUDValue) {
        case SMS:
            strncpy(dst, "DEFAULT", 8);
            break;
        case E3:
            strncpy(dst, "E3 DEMO", 8);
            break;
        case SPACEWORLD:
            strncpy(dst, "SPACEWORLD", 11);
            break;
        }
    }

    void load(JSUMemoryInputStream &in) override {
        IntSetting::load(in);
        mValueChangedCB(&mHUDValue, &mHUDValue,
                        getKind());  // We manually update here to set instructions on load
    }

    // clang-format off
    static void valueChanged(void* old, void* cur, ValueKind kind) {
        //if (*reinterpret_cast<int*>(cur) == LongJumpMappingSetting::BUTTON_Z) {
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249670, 0, 0, 0)), 0x540004A5);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249730, 0, 0, 0)), 0x540004A5);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249C34, 0, 0, 0)), 0x540004A5);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024BF30, 0, 0, 0)), 0x540004A5);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C248, 0, 0, 0)), 0x540004A5);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C36C, 0, 0, 0)), 0x540004A5);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80252124, 0, 0, 0)), 0x540004A5);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8834, 0, 0, 0)), 0x540006F7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8840, 0, 0, 0)), 0x540006F7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A884C, 0, 0, 0)), 0x41820010);  // Allow L button meaning updates
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8860, 0, 0, 0)), 0x54000673);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A886C, 0, 0, 0)), 0x54000673);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A88C8, 0, 0, 0)), 0x54000673);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A88D4, 0, 0, 0)), 0x54000673);
        //}
        //else {
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249670, 0, 0, 0)), 0x540004E7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249730, 0, 0, 0)), 0x540004E7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249C34, 0, 0, 0)), 0x540004E7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024BF30, 0, 0, 0)), 0x540004E7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C248, 0, 0, 0)), 0x540004E7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C36C, 0, 0, 0)), 0x54000427);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80252124, 0, 0, 0)), 0x540004E7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8834, 0, 0, 0)), 0x54000673);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8840, 0, 0, 0)), 0x54000673);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A884C, 0, 0, 0)), 0x60000000);  // Allow L button meaning updates
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8860, 0, 0, 0)), 0x540006F7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A886C, 0, 0, 0)), 0x540006F7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A88C8, 0, 0, 0)), 0x540006F7);
        //    PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A88D4, 0, 0, 0)), 0x540006F7);
        //}
    }
    // clang-format on

private:
    int mHUDValue;
};

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
        auto *engine_module = BetterSMS::getModuleInfo("Better Sunshine Engine");

        auto *exploits_setting = engine_module->mSettings->getSetting("Exploit Fixes");
        auto *collision_setting = engine_module->mSettings->getSetting("Collision Fixes");

        const bool is_unlocked = *reinterpret_cast<bool *>(cur);
        
        if (exploits_setting)
            exploits_setting->setBool(!is_unlocked);
        
        if (collision_setting)
            collision_setting->setBool(!is_unlocked);
    }
    // clang-format on

private:
    bool mBugsExploitsValue;
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
        if (!BetterSMS::isModuleRegistered("Mirror Mode"))
            return;
        auto *mirror_module = BetterSMS::getModuleInfo("Mirror Mode");

        auto *active_setting = mirror_module->mSettings->getSetting("Is Enabled");
        if (active_setting) {
            active_setting->setUserEditable(mMirrorModeFlag, BetterSMS::Settings::Priority::GAME);
            if (!mMirrorModeFlag)
                active_setting->setBool(false);
        }
    }

private:
    bool mMirrorModeFlag;
};

class TutorialSetting final : public Settings::SwitchSetting {
public:
    TutorialSetting() : SwitchSetting("Bugs/Exploits", &mTutorialValue) {}
    ~TutorialSetting() override {}

    bool isUnlocked() const override { return false; }

    void load(JSUMemoryInputStream &in) override {
        bool b;
        in.read(&b, 1);
        setBool(b);
    }

    void save(JSUMemoryOutputStream &out) override { out.write(&mTutorialValue, 1); }

private:
    bool mTutorialValue;
};

HUDSetting::HUD getHUDKind();