#pragma once

#include <Dolphin/types.h>
#include <Dolphin/CARD.h>

#include <BetterSMS/settings.hxx>
#include <BetterSMS/memory.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/icons.hxx>

constexpr size_t MaxShineCount = 300;

using namespace BetterSMS;

extern const u8 gSaveBnr[];
extern const u8 gSaveIcon[];

class HUDSetting final : public Settings::IntSetting {
public:
    enum HUD { SMS, E3, SPACEWORLD };

    HUDSetting() : IntSetting("HUD Style", &mHUDValue) {
        mValueRange = { 0, 2, 1 };
        mValueChangedCB = valueChanged;
    }
    ~HUDSetting() override {}

    void getValueStr(char* dst) const override {
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

    void load(JSUMemoryInputStream& in) override {
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
    BugsExploitsSetting() : SwitchSetting("Enable Bugs/Exploits", &mBugsExploitsValue) {
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

    // clang-format off
    static void valueChanged(void* old, void* cur, ValueKind kind) {
        auto *engine_module = BetterSMS::getModuleInfo("Better Sunshine Engine");
        if (!engine_module) {
            OSReport("Could not find engine module!!\n");
            return;
        }

        auto *bugs_setting = engine_module->mSettings->getSetting("Bug Fixes");
        auto *exploits_setting = engine_module->mSettings->getSetting("Exploit Fixes");
        auto *collision_setting = engine_module->mSettings->getSetting("Collision Fixes");

        const bool is_unlocked = *reinterpret_cast<bool *>(cur);

        if (bugs_setting)
            bugs_setting->setBool(is_unlocked);
        
        if (exploits_setting)
            exploits_setting->setBool(is_unlocked);
        
        if (collision_setting)
            collision_setting->setBool(is_unlocked);
    }
    // clang-format on

private:
    bool mBugsExploitsValue;
    static bool sIsUnlocked;
};

HUDSetting::HUD getHUDKind();