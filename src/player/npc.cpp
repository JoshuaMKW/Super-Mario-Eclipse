#include <Dolphin/MTX.h>
#include <Dolphin/math.h>
#include <Dolphin/types.h>

#include <SMS/Enemy/EnemyMario.hxx>
#include <SMS/NPC/NpcBase.hxx>
#include <SMS/Player/Mario.hxx>

#include <BetterSMS/module.hxx>

using namespace BetterSMS;

#if SME_GRAB_NPCS

/* NPC CARRY CODE */

// 0x8029A87C
static u32 carryOrTalkNPC(TBaseNPC *npc) {
    const Class::TPlayerData *playerData = TGlobals::getPlayerData(gpMarioAddress);

    if (!playerData->isMario())
        return isNowCanTaken__8TBaseNPCCFv(npc);

    if ((*(u32 *)(&npc->mStateFlags.asFlags) & 0x840007) != 0)
        return 0;

    if (gpMarioAddress->mState == static_cast<u32>(TMario::STATE_IDLE))
        return 0;

    bool oldTake                         = npc->mStateFlags.asFlags.mCanBeTaken;
    npc->mStateFlags.asFlags.mCanBeTaken = playerData->getParams()->mCanHoldNPCs.get();

    u32 ret = isNowCanTaken__8TBaseNPCCFv(npc);

    npc->mStateFlags.asFlags.mCanBeTaken = oldTake;
    return ret;
}
SME_PATCH_BL(SME_PORT_REGION(0x8029A87C, 0x80292758, 0, 0), carryOrTalkNPC);

// 0x802815F0
static bool canGrabAtNPC() {
    TBaseNPC *npc;
    SME_FROM_GPR(30, npc);

    const Class::TPlayerData *playerData = TGlobals::getPlayerData(gpMarioAddress);

    if (!playerData->isMario())
        return npc->mStateFlags.asFlags.mCanBeTaken;

    return (playerData->getParams()->mCanHoldNPCs.get() &&
            gpMarioAddress->mState != static_cast<u32>(TMario::STATE_IDLE)) ||
           npc->mStateFlags.asFlags.mCanBeTaken;
}
SME_PATCH_BL(SME_PORT_REGION(0x802815F0, 0x8027937C, 0, 0), canGrabAtNPC);
SME_WRITE_32(SME_PORT_REGION(0x802815F4, 0x80279380, 0, 0), 0x2C030000);

// 0x80207430
static bool canCarryNPC() {
    TBaseNPC *npc;
    SME_FROM_GPR(29, npc);

    const Class::TPlayerData *playerData = TGlobals::getPlayerData(gpMarioAddress);

    if (!playerData->isMario())
        return npc->mStateFlags.asFlags.mCanBeTaken;

    return (playerData->getParams()->mCanHoldNPCs.get() &&
            gpMarioAddress->mState != static_cast<u32>(TMario::STATE_IDLE)) ||
           npc->mStateFlags.asFlags.mCanBeTaken;
}
SME_PATCH_BL(SME_PORT_REGION(0x80207430, 0x801FF314, 0, 0), canCarryNPC);
SME_WRITE_32(SME_PORT_REGION(0x80207434, 0x801FF318, 0, 0), 0x2C030000);

// // extern -> SME.cpp
// // 0x802145F0
// static TMario *scaleNPCThrowLength(TMario *player, float *params) {
//   f32 _f11;
//   SME_FROM_FPR(11, _f11);

//   _f11 = params[0x1D0 / 4];
//   Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

//   if (playerData->isMario())
//     _f11 *= playerData->getParams()->mThrowPowerMultiplier.get() *
//             Util::Math::scaleLinearAtAnchor<f32>(
//                 playerData->getParams()->mSizeMultiplier.get(), 0.5f, 1.0f);

//   if (player->mState == static_cast<u32>(TMario::STATE_NPC_THROW) ||
//       player->mState == static_cast<u32>(TMario::STATE_NPC_JUMPTHROW)) {
//     _f11 *= 4.0f;
//   }

//   SME_TO_FPR(11, _f11);
//   return player;
// }
// SME_PATCH_BL(SME_PORT_REGION(0x802145F0, 0x8020C4D4, 0, 0), scaleNPCThrowLength);
// SME_WRITE_32(SME_PORT_REGION(0x802145F4, 0x8020C4D8, 0, 0), 0xC002E5E0);
// SME_WRITE_32(SME_PORT_REGION(0x802145F8, 0x8020C4DC, 0, 0), 0xC0230034);

// // extern -> SME.cpp
// // 0x8021463C
// static u32 scaleNPCThrowHeight(u32 _r3, f32 z, f32 y) {
//   TBaseNPC *npc;
//   SME_FROM_GPR(31, npc);

//   TMario *player = (TMario *)npc->mPrevHolder;
//   Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

//   if (playerData->isMario())
//     y *= playerData->getParams()->mThrowPowerMultiplier.get() *
//          Util::Math::scaleLinearAtAnchor<f32>(
//              playerData->getParams()->mSizeMultiplier.get(), 0.5f, 1.0f);

//   if (player->mState == static_cast<u32>(TMario::STATE_NPC_THROW) ||
//       player->mState == static_cast<u32>(TMario::STATE_NPC_JUMPTHROW))
//     y *= 4.0f;

//   npc->mSpeed.y = y;
//   npc->mSpeed.z = z;

//   return _r3;
// }
// SME_WRITE_32(SME_PORT_REGION(0x8021462C, 0x8020C510, 0, 0), 0xEC0B0032);
// SME_WRITE_32(SME_PORT_REGION(0x80214634, 0x8020C518, 0, 0), 0xEC2B0072);
// SME_PATCH_BL(SME_PORT_REGION(0x8021463C, 0x8020C520, 0, 0), scaleNPCThrowHeight);

#endif

// 0x80213314
static SMS_ASM_FUNC void scaleNPCTalkRadius() {
    SMS_ASM_BLOCK("lis 3, gpMarioAddress@ha                \n\t"
                  "lwz 3, gpMarioAddress@l (3)             \n\t"
                  "lfs 0, 0x2C (3)                         \n\t"
                  "fmuls 30, 30, 0                         \n\t"
                  "lis 3, mPtrSaveNormal__8TBaseNPC@ha     \n\t"
                  "lwz 3, mPtrSaveNormal__8TBaseNPC@l (3)  \n\t"
                  "blr                                     \n\t");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80213314, 0x8020B1FC, 0, 0), scaleNPCTalkRadius);