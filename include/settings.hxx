#pragma once

#include <Dolphin/types.h>
#include <Dolphin/CARD.h>

#include <BetterSMS/settings.hxx>
#include <BetterSMS/memory.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/icons.hxx>

using namespace BetterSMS;

extern const u8 gSaveBnr[];
extern const u8 gSaveIcon[];

class LongJumpMappingSetting final : public Settings::IntSetting {
public:
    enum Mapping { BUTTON_Z, BUTTON_L };

    LongJumpMappingSetting() : IntSetting("Long Jump/Crouch Button", &sLongJumpValue) {
        mValueRange = { 0, 1, 1 };
        mValueChangedCB = valueChanged;
    }
    ~LongJumpMappingSetting() override {}

    void getValueStr(char* dst) const override {
        switch (sLongJumpValue) {
        case Mapping::BUTTON_Z:
            strncpy(dst, "Z", 2);
            break;
        case Mapping::BUTTON_L:
            strncpy(dst, "L", 2);
            break;
        }
    }

    void load(JSUMemoryInputStream& in) override {
        IntSetting::load(in);
        mValueChangedCB(mValuePtr, mValuePtr,
            getKind());  // We manually update here to set instructions on load
    }

    // clang-format off
    static void valueChanged(void* old, void* cur, ValueKind kind) {
        if (*reinterpret_cast<int*>(cur) == LongJumpMappingSetting::BUTTON_Z) {
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249670, 0, 0, 0)), 0x540004A5);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249730, 0, 0, 0)), 0x540004A5);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249C34, 0, 0, 0)), 0x540004A5);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024BF30, 0, 0, 0)), 0x540004A5);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C248, 0, 0, 0)), 0x540004A5);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C36C, 0, 0, 0)), 0x540004A5);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80252124, 0, 0, 0)), 0x540004A5);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8834, 0, 0, 0)), 0x540006F7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8840, 0, 0, 0)), 0x540006F7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A884C, 0, 0, 0)), 0x41820010);  // Allow L button meaning updates
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8860, 0, 0, 0)), 0x54000673);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A886C, 0, 0, 0)), 0x54000673);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A88C8, 0, 0, 0)), 0x54000673);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A88D4, 0, 0, 0)), 0x54000673);
        }
        else {
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249670, 0, 0, 0)), 0x540004E7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249730, 0, 0, 0)), 0x540004E7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249C34, 0, 0, 0)), 0x540004E7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024BF30, 0, 0, 0)), 0x540004E7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C248, 0, 0, 0)), 0x540004E7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C36C, 0, 0, 0)), 0x54000427);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80252124, 0, 0, 0)), 0x540004E7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8834, 0, 0, 0)), 0x54000673);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8840, 0, 0, 0)), 0x54000673);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A884C, 0, 0, 0)), 0x60000000);  // Allow L button meaning updates
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A8860, 0, 0, 0)), 0x540006F7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A886C, 0, 0, 0)), 0x540006F7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A88C8, 0, 0, 0)), 0x540006F7);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802A88D4, 0, 0, 0)), 0x540006F7);
        }
    }
    // clang-format on

private:
    static int sLongJumpValue;
};

class HoverSlideSetting final : public Settings::SwitchSetting {
public:
    HoverSlideSetting() : SwitchSetting("Hover Slide", &sHoverSlideActive) {
        mValueChangedCB = valueChanged;
    }
    ~HoverSlideSetting() override {}

    void load(JSUMemoryInputStream& in) override {
        SwitchSetting::load(in);
        mValueChangedCB(mValuePtr, mValuePtr,
            getKind());  // We manually update here to set instructions on load
    }

    // clang-format off
    static void valueChanged(void* old, void* cur, ValueKind kind) {
        if (*reinterpret_cast<bool*>(cur) == false) {
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x803DCA00, 0x803D41E0, 0, 0)), 0x00300000 | TMarioAnimeData::FLUDD_DISABLED);
        }
        else {
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x803DCA00, 0x803D41E0, 0, 0)), 0x00300000 | TMarioAnimeData::FLUDD_ENABLED);
        }
    }
    // clang-format on

private:
    static bool sHoverSlideActive;
};

class FastDiveSetting final : public Settings::SwitchSetting {
public:
    FastDiveSetting() : SwitchSetting("Fast Dive/Rollout", &sFastDiveActive) {
        mValueChangedCB = valueChanged;
    }
    ~FastDiveSetting() override {}

    void load(JSUMemoryInputStream& in) override {
        SwitchSetting::load(in);
        mValueChangedCB(mValuePtr, mValuePtr, getKind());  // We manually update here to set instructions on load
    }

    // clang-format off
    static void valueChanged(void* old, void* cur, ValueKind kind) {
        if (*reinterpret_cast<bool*>(cur) == false) {
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802496AC, 0, 0, 0)), 0x4800C089);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024976C, 0, 0, 0)), 0x4800BFC9);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024999C, 0, 0, 0)), 0x4800BD99);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249C70, 0, 0, 0)), 0x4800BAC5);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C3A8, 0, 0, 0)), 0x7FE3FB78);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C284, 0, 0, 0)), 0x480094B1);
        }
        else {
            // Remove arbitrary dive speed
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x802496AC, 0, 0, 0)), 0x60000000);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024976C, 0, 0, 0)), 0x60000000);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024999C, 0, 0, 0)), 0x60000000);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x80249C70, 0, 0, 0)), 0x60000000);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C3A8, 0, 0, 0)), 0x60000000);
            PowerPC::writeU32(reinterpret_cast<u32*>(SMS_PORT_REGION(0x8024C284, 0, 0, 0)), 0x60000000);
        }
    }
    // clang-format on

private:
    static bool sFastDiveActive;
};

extern Settings::SettingsGroup gSettingsGroup;

extern LongJumpMappingSetting gLongJumpMappingSetting;
extern Settings::SwitchSetting gLongJumpSetting;
extern Settings::SwitchSetting gBackFlipSetting;
extern Settings::SwitchSetting gHoverBurstSetting;
extern HoverSlideSetting gHoverSlideSetting;
extern Settings::SwitchSetting gRocketDiveSetting;
extern Settings::SwitchSetting gFastTurboSetting;
extern FastDiveSetting gFastDiveSetting;