#pragma once

#include "types.h"
#include "MTX.h"
#include "sms/JKR.hxx"
#include "sms/sound/JAIBasic.hxx"

enum MS_SCENE_WAVE
{
};

class MSound : public JAIBasic
{
public:
    enum System
    {
        SE_FLUDD_SPRAY_1 = 0,
        SE_FLUDD_SPRAY_2 = 1,
        SE_FLUDD_EMPTY = 2,
        SE_LIFE_METER_DANGER = 0x4800,
        SE_LIFE_REPLENISH = 0x4801,
        SE_MAP_OPEN = 0x4802,
        SE_MAP_CLOSE = 0x4803,
        SE_TALK_OPEN = 0x4804,
        SE_TALK_CLOSE = 0x4805,
        SE_ROCKETNOZZLE_u8GE = 0x4806,

        SE_FRUIT_IN_BASKET = 0x480A,

        SE_WATER_METER_LOSE = 0x480C,

        SE_MENU_CURSOR_MOVE = 0x480F,
        SE_OPEN_SAVE_FILE = 0x4810,
        SE_GET_COIN = 0x4811,
        SE_DEBBS_ALERT_APPEAR = 0x4812,
        SE_COIN_APPEAR = 0x4813,
        SE_SHINE_TOUCH = 0x4814,

        SE_NOTICE_OPEN = 0x4819,
        SE_NOTICE_CLOSE = 0x481A,

        SE_SHINE_APPEAR = 0x4821,
        SE_FIRST_PERSON_LOOK = 0x4824,
        SE_FIRST_PERSON_STOP = 0x4825,
        SE_CAMERA_ADJUST = 0x4826,
        SE_TEXT_APPEAR = 0x4827,

        SE_TIME_SLOW = 0x4835,
        SE_TIME_FAST = 0x4836,
        SE_TIME_FASTER = 0x4837,
        SE_TIME_FASTEST = 0x4838,
        SE_TIME_FASTESTEST = 0x4839,
        SE_TIME_FASTESTESTEST = 0x483A,

        SE_DENY = 0x483D,
        SE_YOSHI_DENY_FRUIT = 0x483E,
        SE_ACCEPT = 0x483F,
        SE_YOSHI_ACCEPT_FRUIT = 0x4840,
        SE_LIFE_UP = 0x4841,
        SE_COIN_DROP = 0x4842,
        SE_BLUE_COIN_APPEAR = 0x4843,
        SE_NPC_CLEAN = 0x4844,
        SE_BLUE_COIN_GET = 0x4845,
        SE_ONE_UP_GET = 0x4846,
        SE_POLLUTION_CLEAN = 0x4847,

        SE_YOSHI_FIRST_RIDE = 0x4849,

        SE_NINTENDO_CHAO = 0x7914,
        SE_NINTENDO_COIN = 0x7915
    };

    MSound(JKRHeap *, JKRHeap *, u32, u8 *, u8 *, u32);

    bool cameraLooksAtMario();
    u32 checkMarioVoicePlaying(u8);
    void checkWaveOnAram(MS_SCENE_WAVE);
    void demoModeIn(u16, bool);
    void demoModeOut(bool);
    void enterStage(MS_SCENE_WAVE, u8, u8);
    void exitStage();
    void fadeOutAllSound(u32);
    bool gateCheck(u32);
    void *getBstSwitch(u32);
    f32 getDistFromCamera(Vec *);
    f32 getDistPowFromCamera(const Vec &);
    bool getMapInfoGround(u32);
    u32 getMarioVoiceID(u8);
    u32 getSwitch(u32, u32, u32);
    u32 getWallSound(u32, f32);
    void initSound();
    void loadGroupWave(s64);
    void loadWave(MS_SCENE_WAVE);
    void mainLoop();
    void makeSound(u32);
    void pauseOff(u8);
    void pauseOn(bool);
    void playTimer(u32);
    bool resetAudioAll(u16);
    void setCameraInfo(Vec *, Vec *, float (*) [4], u32);
    void setCategoryVOLs(u16, f32);
    void setPlayerInfo(Vec *, Vec *, float (*) [4], bool);
    bool setRegisterTrackCallback();
    void startBeeSe(Vec *, u32);
    void startForceJumpSound(Vec *, u32, f32, u32);
    void startMarioVoice(u32, s16, u8);
    void startSoundActorSpecial(u32, const Vec *, f32, f32, u32, JAISound **, u32, u8);
    void startSoundSet(u32, const Vec *, u32, f32, u32, u32, u8);
    void startSoundGrp(u32, const Vec *, u32, f32, u32, u32, u8);
    void startSoundSystemSE(u32, u32, JAISound **, u32);
    void stopAllSound();
    void stopMarioVoice(u32, u8);
    void talkModeIn(bool);
    void talkModeOut();

    u8 _00[0xD2];
};