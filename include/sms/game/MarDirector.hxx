#pragma once

#include "types.h"

#include "JDrama/JDRActor.hxx"
#include "JDrama/JDRDirector.hxx"
#include "JGeometry.hxx"

#include "sms/GC2D/Guide.hxx"

#include "sms/actor/Yoshi.hxx"
#include "sms/actor/item/Coin.hxx"
#include "sms/actor/item/Shine.hxx"

#include "sms/game/GCConsole2.hxx"
#include "sms/npc/BaseNPC.hxx"
#include "sms/object/DemoCannon.hxx"
#include "sms/screen/ShineFader.hxx"

#include "GCConsole2.hxx"
#include "PerformList.hxx"

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
  void currentStateFinalize(u8);
  void changeState();
  void fireStreamingMovie(s8);
  void fireEndDemoCamera();
  void fireStartDemoCamera(const char *, TVec3f const *, s32,
                           f32, bool, s32 (*)(u32, u32), u32, JDrama::TActor *,
                           JDrama::TFlagT<u16>);
  void setNextStage(u16 stageID, JDrama::TActor *);
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

  TPerformList *mPerformListGXPost;     // 0x0024
  TPerformList *mPerformListMovement;   // 0x0028
  TPerformList *mPerformListCalcAnim;   // 0x002C
  TPerformList *mPerformListUIElements; // 0x0030
  TPerformList *mPerformListPreDraw;    // 0x0034 ?
  TPerformList *mPerformListUnk1;       // 0x0038
  TPerformList *mPerformListUnk2;       // 0x003C
  TPerformList *mPerformListPostDraw;   // 0x0040 ?
  TPerformList *mPerformListShineMove;  // 0x0044
  TPerformList *mPerformListShineAnim;  // 0x0048
  u16 mGameState;                       // 0x004C
  u16 _02;                              // 0x004E
  u32 _03[0x14 / 4];                    // 0x0050
  u8 mCurState;                         // 0x0064
  u8 _04;                               // 0x0065
  u16 _05;                              // 0x0066
  u32 _06[0xC / 4];                     // 0x0068
  TGCConsole2 *mGCConsole;              // 0x0074
  TGuide *mGuide;                       // 0x0078
  u8 mAreaID;                           // 0x007C
  u8 mEpisodeID;                        // 0x007D
  u16 _09;                              // 0x007E
  u32 _10;                              // 0x0080
  void *mTalkCursor;                    // TTalkCursor
  u32 _88[0x18 / 4];
  TBaseNPC *mTalkingNPC; // 0x00A0
  u32 _A4[2];
  u32 *mpNextState;         // 0x00AC
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