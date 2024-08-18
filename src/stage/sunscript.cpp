#include <Dolphin/string.h>
#include <Dolphin/stdlib.h>
#include <Dolphin/printf.h>

#include <JSystem/JDrama/JDRNameRef.hxx>

#include <SMS/MoveBG/Shine.hxx>
#include <SMS/System/MarNameRefGen.hxx>
#include <SMS/Enemy/Conductor.hxx>
#include <SMS/Manager/ItemManager.hxx>
#include <SMS/NPC/NpcBase.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/sunscript.hxx>

#include "message.hxx"

using namespace BetterSMS;

void evGetAddressFromGraphWebName(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(1, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::STRING) {
        SpcTrace("Expected string argument for evGetAddressFromGraphWebName!\n");
        Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
        return;
    }

    const char *name = (const char *)arg_slice.mValue;
    TGraphWeb *graph = gpConductor->getGraphByName(name);
    if (graph == nullptr) {
        SpcTrace("GraphWeb %s not found!\n", name);
    }

    Spc::Stack::pushItem(spc, (u32)graph, Spc::ValueType::INT);
}

void evNpcSetGraphWeb(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(2, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 1 for evNpcSetGraphWeb!\n");
        return;
    }

    TGraphWeb *graph = (TGraphWeb *)arg_slice.mValue;

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 2 for evNpcSetGraphWeb!\n");
        return;
    }

    TBaseNPC *npc             = (TBaseNPC *)arg_slice.mValue;
    npc->mGraphTracer->mGraph = graph;

    Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
}

void evNpcStepOn(TSpcInterp *spc, u32 argc) {
    TGraphWeb *graph = nullptr;
    TBaseNPC *npc    = nullptr;

    if (argc == 2) {
        TSpcSlice arg_slice = Spc::Stack::popItem(spc);
        if (arg_slice.mType != Spc::ValueType::INT) {
            SpcTrace("Expected integer argument 2 for evNpcStepOn!\n");
            return;
        }

        graph = (TGraphWeb *)arg_slice.mValue;
    }

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument for evNpcStepOn!\n");
        return;
    }

    npc = (TBaseNPC *)arg_slice.mValue;

    while (npc->mSpineBase->mNerveStack.depth() > 0) {
        npc->mSpineBase->mNerveStack.pop();
    }

    if (graph != nullptr) {
        npc->mGraphTracer->mGraph = graph;
        npc->mSpineBase->setNerve(
            (const TNerveBase<TLiveActor> *)0x8040DFD4);  // TNerveNPCGraphWander
    } else {
        npc->mGraphTracer->mGraph = (TGraphWeb *)0x803AFB48;  // (null)
        npc->mSpineBase->setNerve(
            (const TNerveBase<TLiveActor> *)0x8040DFE4);  // TNerveNPCGraphWait
    }

    npc->mSpineBase->setNerve((const TNerveBase<TLiveActor> *)0x8040DFD4);  // TNerveNPCGraphWander
    npcStepIn__8TBaseNPCFv(npc);

    Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
}

void evNpcWaitOn(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(1, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument for evNpcWaitOn!\n");
        return;
    }

    TBaseNPC *npc = (TBaseNPC *)arg_slice.mValue;

    while (npc->mSpineBase->mNerveStack.depth() > 0) {
        npc->mSpineBase->mNerveStack.pop();
    }

    npc->mGraphTracer->mGraph = (TGraphWeb *)0x803AFB48;  // (null)
    npc->mSpineBase->setNerve(
        (const TNerveBase<TLiveActor> *)0x8040DFF4);  // TNerveNPCWaitMarioApproach
    npcWaitIn__8TBaseNPCFv(npc);

    Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
}

void evNpcMadOn(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(1, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument for evNpcWaitOn!\n");
        return;
    }

    TBaseNPC *npc = (TBaseNPC *)arg_slice.mValue;

    const TNerveBase<TLiveActor> *nerve = npc->mSpineBase->getLatestNerve();

    while (npc->mSpineBase->mNerveStack.depth() > 0) {
        npc->mSpineBase->mNerveStack.pop();
    }

    // npc->mGraphTracer->mGraph = (TGraphWeb *)0x803AFB48;                    // (null)
    npc->mSpineBase->setNerve((const TNerveBase<TLiveActor> *)0x8040E03C);  // TNerveNPCMad
    npc->mSpineBase->pushNerve(nerve);                                      // Old current
    npcMadIn__8TBaseNPCFv(npc);

    Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
}

void evIsInSightOfActor(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(4, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 4 for evIsInSightOfActor!\n");
        Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
        return;
    }

    JDrama::TActor *actor = (JDrama::TActor *)arg_slice.mValue;

    f32 max_dist = Spc::Stack::popItem(spc).getDataFloat();
    f32 fovy     = Spc::Stack::popItem(spc).getDataFloat();

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 1 for evIsInSightOfActor!\n");
        Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
        return;
    }

    JDrama::TActor *src_actor = (JDrama::TActor *)arg_slice.mValue;

    if (fabsf(src_actor->mTranslation.y - actor->mTranslation.y) > 450.0f) {
        Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
        return;
    }

    if (PSVECDistance(src_actor->mTranslation, actor->mTranslation) > max_dist) {
        Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
        return;
    }

    TVec3f v = src_actor->mTranslation - actor->mTranslation;
#if 0
    f32 B    = atan2f(v.y, v.x);
    f32 &a    = fovy;
    f32 &y    = actor->mRotation.y;

    bool inSight = (a + y - (M_PI / 2.0f) <= B && B <= a + y + (M_PI / 2.0f));
#else
    v.normalize();
    TVec3f forward = TVec3f(sinf(DEGREES_TO_RADIANS(actor->mRotation.y)), 0.0f,
                            cosf(DEGREES_TO_RADIANS(actor->mRotation.y)));
    forward.normalize();
    f32 dot = v.dot(forward);

    f32 angleToPlayer = RADIANS_TO_DEGREES(acosf(dot));
    bool inSight      = fabsf(angleToPlayer) <= (fovy / 2.0f);
#endif
    Spc::Stack::pushItem(spc, inSight, Spc::ValueType::INT);
    return;
}

void evSetLiveActorFlag(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(3, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 2 for evIsInSightOfActor!\n");
        return;
    }

    bool set = arg_slice.mValue;

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 1 for evIsInSightOfActor!\n");
        return;
    }

    u32 flags = arg_slice.mValue;

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 3 for evIsInSightOfActor!\n");
        return;
    }

    TLiveActor *src_actor        = (TLiveActor *)arg_slice.mValue;
    src_actor->mStateFlags.asU32 = set ? (src_actor->mStateFlags.asU32 | flags)
                                       : (src_actor->mStateFlags.asU32 & ~flags);

    Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
}

void evCheckBrokenWatermelon(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(3, &argc);

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 2 for evCheckBrokenWatermelon!\n");
        return;
    }

    u32 endInt = arg_slice.mValue;

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::INT) {
        SpcTrace("Expected integer argument 1 for evCheckBrokenWatermelon!\n");
        return;
    }

    u32 startInt = arg_slice.mValue;

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::STRING) {
        SpcTrace("Expected string argument 0 for evCheckBrokenWatermelon!\n");
        return;
    }

    const char *name = (const char *)arg_slice.mValue;

    char formatBuf[128] = {};
    snprintf(formatBuf, 64, "%s", name);
    strcat(formatBuf, "%d\0");

    char nameBuf[128] = {};
    auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();

    size_t total_broken = 0;

    for (size_t i = startInt; i < endInt; ++i) {
        snprintf(nameBuf, 128, formatBuf, i);
        u16 keyCode = JDrama::TNameRef::calcKeyCode(nameBuf);
        if (JDrama::TNameRef *p = nameRef->searchF(keyCode, nameBuf)) {
            TLiveActor *actor = reinterpret_cast<TLiveActor *>(p);
            if ((actor->mStateFlags.asU32 & 3) != 0) {
                total_broken++;
            }
        }
    }

    Spc::Stack::pushItem(spc, total_broken, Spc::ValueType::INT);
}

void evAppearShineFromNPCLocal(TSpcInterp* spc, u32 argc) {
    spc->verifyArgNum(6, &argc);

    f32 z = Spc::Stack::popItem(spc).getDataFloat();
    f32 y = Spc::Stack::popItem(spc).getDataFloat();
    f32 x = Spc::Stack::popItem(spc).getDataFloat();

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::STRING) {
        SpcTrace("Expected integer argument 2 for evAppearShineFromNPCLocal!\n");
        return;
    }

    const char *camera_key = (const char *)arg_slice.mValue;

    arg_slice              = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::STRING) {
        SpcTrace("Expected integer argument 1 for evAppearShineFromNPCLocal!\n");
        return;
    }

    TBaseNPC *npc = nullptr;
    const char *npc_key = (const char *)arg_slice.mValue;
    {
        auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
        u16 keyCode   = JDrama::TNameRef::calcKeyCode(npc_key);
        if (JDrama::TNameRef *p = nameRef->searchF(keyCode, npc_key)) {
            npc           = reinterpret_cast<TBaseNPC *>(p);
        }
    }

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::STRING) {
        SpcTrace("Expected integer argument 0 for evAppearShineFromNPCLocal!\n");
        return;
    }

    TShine *shine = nullptr;
    const char *shine_key = (const char *)arg_slice.mValue;
    {
        auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
        u16 keyCode   = JDrama::TNameRef::calcKeyCode(shine_key);
        if (JDrama::TNameRef *p = nameRef->searchF(keyCode, shine_key)) {
            shine           = reinterpret_cast<TShine *>(p);
        }
    }

    if (strcmp(camera_key, "") == 0) {
        shine->mInitialPosition = npc->mTranslation + TVec3f(x, y, z);
        shine->mTranslation     = npc->mTranslation;
        shine->appearWithTime(0x4B0, -1, -1, -1);
    } else {
        shine->mInitialPosition = npc->mTranslation + TVec3f(x, y, z);
        gpItemManager->makeShineAppearWithDemo(shine_key, camera_key, npc->mTranslation.x,
                                               npc->mTranslation.y, npc->mTranslation.z);
    }

    Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
}

void evAppearShineFromNPCLocalWithoutDemo(TSpcInterp *spc, u32 argc) {
    spc->verifyArgNum(5, &argc);

    f32 z = Spc::Stack::popItem(spc).getDataFloat();
    f32 y = Spc::Stack::popItem(spc).getDataFloat();
    f32 x = Spc::Stack::popItem(spc).getDataFloat();

    TSpcSlice arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::STRING) {
        SpcTrace("Expected integer argument 1 for evAppearShineFromNPCLocalWithoutDemo!\n");
        return;
    }

    TBaseNPC *npc       = nullptr;
    const char *npc_key = (const char *)arg_slice.mValue;
    {
        auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
        u16 keyCode   = JDrama::TNameRef::calcKeyCode(npc_key);
        if (JDrama::TNameRef *p = nameRef->searchF(keyCode, npc_key)) {
            npc = reinterpret_cast<TBaseNPC *>(p);
        }
    }

    arg_slice = Spc::Stack::popItem(spc);
    if (arg_slice.mType != Spc::ValueType::STRING) {
        SpcTrace("Expected integer argument 0 for evAppearShineFromNPCLocalWithoutDemo!\n");
        return;
    }

    TShine *shine         = nullptr;
    const char *shine_key = (const char *)arg_slice.mValue;
    {
        auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
        u16 keyCode   = JDrama::TNameRef::calcKeyCode(shine_key);
        if (JDrama::TNameRef *p = nameRef->searchF(keyCode, shine_key)) {
            shine = reinterpret_cast<TShine *>(p);
        }
    }

    shine->mTranslation     = npc->mTranslation;
    shine->mInitialPosition = npc->mTranslation + TVec3f(x, y, z);
    shine->appearWithTime(0x3B0, -1, -1, -1);
    shine->mInitialPosition = npc->mTranslation + TVec3f(x, y, z);

    Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
}

static AtomBalloonMessageViewer s_msg_spc(-1, 0);

void evAppearFluddTip(TSpcInterp* spc, u32 argc) {
    spc->verifyArgNum(2, &argc);

    u32 message_duration = Spc::Stack::popItem(spc).getDataInt();
    u32 message_id = Spc::Stack::popItem(spc).getDataInt();

    s_msg_spc = AtomBalloonMessageViewer(message_id, message_duration);
    s_msg_spc.show();

    Spc::Stack::pushItem(spc, 0, Spc::ValueType::INT);
}