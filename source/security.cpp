#include "includes/eclipse.hxx"

void setUserCodes(bool enable) {
    const u8 enabledmagic[] = { 0x00, 0xD0, 0xC0, 0xDE, 0x00, 0xD0, 0xC0, 0xDE };
    const u8 disabledmagic[] = { 0x00, 0xD0, 0xDE, 0xAD, 0x00, 0xD0, 0xDE, 0xAD };
    u8* searchcontext = (u8*)0x80001F00;
    u32 searchlength = 0x1100;
    u32 searchindex = 0;
    u8 matchcount;
    u8 matchmax = sizeof(enabledmagic);

    for (u32 i = 0; i < searchlength; ++i) {
        if (enable) {
            if (searchcontext[i] == disabledmagic[matchcount]) matchcount++;
            else matchcount = 0;
        } else {
            if (searchcontext[i] == enabledmagic[matchcount]) matchcount++;
            else matchcount = 0;
        }

        if (matchcount >= matchmax) break;
        searchindex++;
    }

    if (enable) {
        for (u32 i = 0; i < sizeof(enabledmagic); ++i) {
            searchcontext[(searchindex - 8) + i] = disabledmagic[i];
        }
    } else {
        for (u32 i = 0; i < sizeof(disabledmagic); ++i) {
            searchcontext[(searchindex - 8) + i] = enabledmagic[i];
        }
    }
}

void setUserCodes() {
    const u8 enabledmagic[]{ 0x00, 0xD0, 0xC0, 0xDE };
    const u8 disabledmagic[]{ 0x00, 0xD0, 0xDE, 0xAD };
    u8* searchcontext = (u8*)0x80001F00;
    u32 searchlength = 0x1100;
    u32 searchindex = 0;
    u8 matchcount;
    u8 matchmax = sizeof(enabledmagic);

    for (u32 i = 0; i < searchlength; ++i) {
        if (searchcontext[i] == enabledmagic[matchcount % 4]) matchcount++;
        else matchcount = 0;

        if (matchcount >= matchmax) break;
        searchindex++;
    }

    for (u32 i = 0; i < sizeof(enabledmagic) << 1; ++i) {
        searchcontext[(searchindex - 8) + i] = disabledmagic[i % 4];
    }
}