#include <BetterSMS/sunscript.hxx>
#include <SMS/NPC/NpcBase.hxx>
#include <SMS/raw_fn.hxx>

using namespace BetterSMS;

void evNpcWaitOn(TSpcInterp *spc, u32 argc) {
    u32 expected_args = 1;
    spc->verifyArgNum(argc, &expected_args);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument for evNpcWaitOn!\n");
        return;
    }

    TBaseNPC *npc = (TBaseNPC *)arg_slice.mValue;
    npc->mGraphTracer->mGraph = (TGraphWeb *)0x803AFB48;
    npc->mSpineBase->setNerve((const TNerveBase<TLiveActor> *)0x8040DFF4);
    npcWaitIn__8TBaseNPCFv(npc);
}