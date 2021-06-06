#pragma once

#include "GCConsole2.hxx"
#include "types.h"

#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/actor/Yoshi.hxx"
#include "sms/actor/item/Coin.hxx"
#include "sms/actor/item/Shine.hxx"
#include "sms/game/GCConsole2.hxx"
#include "sms/object/DemoCannon.hxx"
#include "sms/screen/ShineFader.hxx"

class TMarDirector : public JDrama::TDirector {
public:
  enum Status {
    INTRO_INIT = 0,
    INTRO_PLAYING = 1,
    NORMAL = 4,
    PAUSE_MENU = 5,
    SAVE_CARD = 11
  };

  enum State { WARP_OUT = 2 };

  TMarDirector();
  ~TMarDirector();

  virtual s32 direct();
  virtual JStage::TObject *JSGFindObject(const char *, JStage::TEObject) const;

  void moveStage();
  void updateGameMode();
  void nextStateInitialize(s8);
  void setMario();
  void currentStateFinalize(s8);
  void changeState();
  void fireStreamingMovie(s8);
  void fireEndDemoCamera();
  void fireStartDemoCamera(char const *, JGeometry::TVec3<f32> const *, u32,
                           f32, bool, u32 (*)(u32, u32), u32, JDrama::TActor *,
                           JDrama::TFlagT<s16>);
  void setNextStage(s16 stageID, JDrama::TActor *);
  void movement();
  void fireRideYoshi(TYoshi *);
  void fireGetStar(TShine *);
  void fireGetNozzle(u32 *); // TItemNozzle
  void fireGetBlueCoin(TCoin *);
  void movement_game();
  u32 *findNearestTalkNPC();
  u32 createObjects(); // returns 0
  void setupObjects();
  bool thpInit();
  void loadParticleMario();
  void loadParticle();
  void initLoadParticle();
  void loadResource();

  u32 _00[0x28 / 4];        // 0x0024
  u16 mGameState;           // 0x004C
  u16 _02;                  // 0x004E
  u32 _03[0x14 / 4];        // 0x0050
  u8 mLastState;            // 0x0064
  u8 _04;                   // 0x0065
  u16 _05;                  // 0x0066
  u32 _06[0xC / 4];         // 0x0068
  TGCConsole2 *mGCConsole;  // 0x0074
  u32 _07;                  // 0x0078
  u8 mAreaID;               // 0x007C
  u8 mEpisodeID;            // 0x007D
  u16 _09;                  // 0x007E
  u32 _10[0x2C / 4];        // 0x0080
  u32 *sNextState;          // 0x00AC
  u32 _11;                  // 0x00B0
  u8 mNextState;            // 0x00B4
  u32 _12[0x1C / 4];        // 0x00B8
  u32 *mGame6Data;          // 0x00D4
  u32 *mAramArchive;        // 0x00D8
  TShineFader *mShineFader; // 0x00DC
  u32 _13[0x44 / 4];        // 0x00E0
  u8 mNextStateA;           // 0x0124 ?
  u32 _14[0x12C / 4];       // 0x0128
  TDemoCannon *mCannonObj;  // 0x0254
  u32 _15;                  // 0x0258
  TShine *mCollectedShine;  // 0x025C
};