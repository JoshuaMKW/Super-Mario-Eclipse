#include "GX.h"
#include "sms/JSystem/J3D/J3DDrawBuffer.hxx"

#include "SME.hxx"

using namespace SME::Patch;

/*
NOTES
testperform bctrl to important rendering and update stuff 802fcce8
perform view on mario 8024d2a8

draw_3dframebuffer funny 802efab4
8029d7e4 where mario is birthed
8024de38 playercontrol mario
*/

/*
adresses

GXSetViewport = 0x803630c8;
GXSetViewportJitter = 0x80362fac;
draw_J3DDrawBuffer = 0x802efab4;
ptmf_scall = 0x80338204;
alloc = 0x802c3ba4;
ct_Mario = 0x80276cb4;
initValues_Mario = 0x802766b0;
performMario = 0x8024d2ac;
load_Mario = 0x80276bd4;
test = 0x8024d17c;
setMario = 0x802988dc;
*/

// the viewport values for player counts #1-4.
static constexpr float sViewPortData[] = {
    // X      Y      WIDTH  HEIGHT

    // 1 Player
    0.0f,   0.0f,   640.0f, 480.0f,

    // 2 Players
    0.0f,   0.0f,   640.0f, 240.0f,
    0.0f,   240.0f, 640.0f, 240.0f,
    
    // 3 Players
    0.0f,   0.0f,   640.0f, 240.0f,
    0.0f,   240.0f, 320.0f, 240.0f,
    320.0f, 240.0f, 320.0f, 240.0f,

    // 4 Players
    0.0f,   0.0f,   320.0f, 240.0f,
    320.0f, 0.0f,   320.0f, 240.0f,
    0.0f,   240.0f, 320.0f, 240.0f,
    320.0f, 240.0f, 320.0f, 240.0f
};


// When injecting at the mflr of a function prologue in a recursive implementation, this can be used
static void SME_NO_INLINE avoidRecurseCall(u32 addr, ...) {
	asm volatile ("mflr 0");
	asm volatile ("mtctr %0" : : "r"(addr));
    asm volatile ("bctr");
}

void Multiplayer::draw3DOverhaul(J3DDrawBuffer *drawBuffer) {
    const u8 activePlayers = SME::TGlobals::sGlobals.getActivePlayers();
    
	for (u32 i = 0; i < activePlayers; i++) {
		const u32 viewPortIndex = ((activePlayers-1) * 16) + (i * 4);
		GXSetViewport(sViewPortData[viewPortIndex], sViewPortData[viewPortIndex + 1],
                      sViewPortData[viewPortIndex + 2], sViewPortData[viewPortIndex + 3],
                      0.0f, 1.0f);
        drawBuffer->draw();
	}
}

//8029d7e8
TMario *Multiplayer::makeMarios() {
	for (int i = 0; i < SME_MAX_PLAYERS; i++) {
		TMario *player = new TMario();
        player->mController = &gpApplication.mGamePad1[i];
        SME::TGlobals::sGlobals.setPlayerByIndex(i, player);
	}
	return nullptr; //Skip default initialization
}

void Multiplayer::loadMarioTrickyOverhaul(TMario *player, JSUMemoryInputStream &stream) {
    // We preserve the stream's position for each iteration
    const s32 pos = stream.getPosition();
	for (u32 i = 0; i < SME_MAX_PLAYERS; i++) {
		gpMarioAddress = SME::TGlobals::sGlobals.getPlayerByIndex(i);
		SMS_SetMarioAccessParams__Fv();
		stream.seek(pos, JSUStreamSeekFrom::BEGIN);
		avoidRecurseCall(SME_PORT_REGION(0x8024D2AC, 0, 0, 0), gpMarioAddress, stream);
	}
	gpMarioAddress = player;
}

//I manually update each mario here
void Multiplayer::performMarioTrickyOverhaul(TMario *player, JSUMemoryInputStream &stream) {
	for (u32 i = 0; i < SME::TGlobals::sGlobals.getActivePlayers(); i++) {
        gpMarioAddress = SME::TGlobals::sGlobals.getPlayerByIndex(i);
		SMS_SetMarioAccessParams__Fv();
        avoidRecurseCall(SME_PORT_REGION(0x8024D2AC, 0, 0, 0), gpMarioAddress, stream);
	}
	gpMarioAddress = player;
}

// extern -> SME.cpp
// When the game changes the player's state (in)active, is required to make active players visible and alive
void Multiplayer::setMarioOverhaul(TMarDirector *director) {
	for (u32 i = 0; i < SME::TGlobals::sGlobals.getActivePlayers(); i++) {
		gpMarioAddress = SME::TGlobals::sGlobals.getPlayerByIndex(i);
        SMS_SetMarioAccessParams__Fv();
        director->setMario();
	}
	gpMarioAddress = SME::TGlobals::sGlobals.getPlayerByIndex(0);
}