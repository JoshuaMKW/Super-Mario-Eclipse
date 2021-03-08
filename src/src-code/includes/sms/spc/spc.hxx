#pragma once

#include "sms/JDrama.hxx"
#include "sms/event/EventWatcher.hxx"
#include "sms/spc/SpcTypedInterp.hxx"

void SpcTrace(char const *, ...);

void evStartMontemanFanfare(TSpcTypedInterp<TEventWatcher> *, u32);
void evStartMontemanBGM(TSpcTypedInterp<TEventWatcher> *, u32);
void evIsWaterMelonIsReached(TSpcTypedInterp<TEventWatcher> *, u32);
void evInvalidatePad(TSpcTypedInterp<TEventWatcher> *, u32);
void evOnNeutralMarioKey(TSpcTypedInterp<TEventWatcher> *, u32);
void evWarpFrontToMario(TSpcTypedInterp<TEventWatcher> *, u32);
void evAppear8RedCoinsAndTimer(TSpcTypedInterp<TEventWatcher> *, u32);
void evAppearReadyGo(TSpcTypedInterp<TEventWatcher> *, u32);
void evSetEventForWaterMelon(TSpcTypedInterp<TEventWatcher> *, u32);