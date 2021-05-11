#pragma once

#include "sms/JSystem/JDrama.hxx"
#include "sms/camera/CameraInbetween.hxx"
#include "sms/camera/CameraMapTool.hxx"
#include "sms/npc/BaseNPC.hxx"
#include "types.h"


class CPolarSubCamera : public JDrama::TLookAtCamera {
public:
  enum EnumNoticeOnOffMode {
      UNK_MODE_0,
      UNK_MODE_1,
      UNK_MODE_2
  };

  CPolarSubCamera(const char *);
  virtual ~CPolarSubCamera();
  virtual void loadAfter() override;
  virtual void perform(u32, JDrama::TGraphics *) override;

  void addMoveCameraAndMario(const Vec &pos);
  void addMultiPlayer(const JGeometry::TVec3<f32> *, f32, f32);
  s32 calcAngleXFromXRotRatio() const;
  s32 calcDistFromXRotRatio() const;
  void calcFinalPosAndAt();
  void calcInHouseNo(bool);
  void calcNoticeTargetYrot(const Vec &);
  void calcNowTargetFromPosAndAt(const Vec &, const Vec &);
  void calcPosAndAt();
  void calcSecureViewTarget(s16, f32 *, f32 *);
  void calcSlopeAngleX(s16 *);
  void calcTowerCenterPos(Vec *);
  void changeCamModeSpecifyCamMapTool(const TCameraMapTool *);
  void changeCamModeSpecifyCamMapToolAndFrame(const TCameraMapTool *, int);
  void changeCamModeSpecifyFrame(int, int);
  void changeCamModeSub(int, int, bool);
  void controlByCameraCode(int *);
  void createMultiplayer(u8);
  void ctrlGameCamera();
  void ctrlJetCoasterCamera();
  void ctrlLButtonCamera();
  void ctrlMultiPlayerCamera();
  void ctrlNormalDeadDemo();
  void ctrlNormalOrTowerCamera();
  void ctrlOptionCamera();
  void ctrlTalkCamera();
  void doLButtonCameraOff(bool);
  void endDemoCamera();
  void execCameraModeChangeProc(int);
  void execDeadDemoProc();
  void execFrontRotate();
  void execGroundCheck(Vec);
  void execHeightPan();
  void exeInvalidAutoChase();
  void execNoticeOnOffProc(EnumNoticeOnOffMode);
  void execRoofCheck(Vec);
  void execSecureView(s16, Vec *);
  void execWallCheck(Vec *);
  s16 getCameraInbetweenFrame(int);
  s16 getFinalAngleZ() const;
  void *getNoticeActor();
  JGeometry::TVec3<f32> *getNozzleTopPos(JGeometry::TVec3<f32> *) const;
  s16 getOffsetAngleX() const;
  s16 getOffsetAngleY() const;
  s32 getRestDemoFrames() const;
  Mtx *getToroccoMtx() const;
  JGeometry::TVec3<f32> getUsualLookat() const;
  bool isChangeToBossGesoCamera() const;
  bool isChangeToCancanCamera() const;
  bool isChangeToParallelCameraByMoveBG() const;
  bool isChangeToParallelCameraCByMoveBG() const;
  bool isDefiniteCameraSpecifyMode(int) const;
  bool isFixCameraSpecifyMode(int) const;
  bool isFollowCameraSpecifyMode(int) const;
  bool isHellDeadDemo() const;
  bool isJetCoaster1stCamera() const;
  bool isLButtonCameraInbetween();
  bool isLButtonCameraSpecifyMode(int);
  bool isMarioAimWithGun() const;
  bool isMarioCrabWalk() const;
  bool isMomentDefinite() const;
  bool isNeedGroundCheck();
  bool isNeedRoofCheck() const;
  bool isNeedWallCheck() const;
  bool isNormalCameraCompletely() const;
  bool isNormalCameraSpecifyMode(int) const;
  bool isNormalDeadDemo() const;
  bool isNotHeightPanCamMode() const;
  bool isNowInbetween() const;
  bool isOverHipAttackSpecifyMode(int) const;
  bool isRailCameraSpecifyMode(int) const;
  bool isSimpleDemoCamera() const;
  bool isSlopeCameraMode() const;
  bool isTalkCameraInbetween() const;
  bool isTalkCameraSpecifyMode(int) const;
  bool isTowerCameraSpecifyMode(int) const;
  void killHeightPan();
  void killHeightPanWhenChangeCamMode();
  void makeMtxForPrevTalk();
  void makeMtxForTalk(const TBaseNPC *);
  void offMoveApproach();
  void onMoveApproach();
  void removeMultiPlayer(const JGeometry::TVec3<f32> *);
  void rotateX_ByStickY(f32);
  void rotateY_ByStickX(f32);
  void setNoticeInfo();
  void setUpFromLButtonCamera();
  void setUpToLButtonCamera(int);
  void startDemoCamera(const char *, const JGeometry::TVec3<f32> *, s32, f32, bool);
  void startGateDemoCamera(const JDrama::TActor *);
  void updateDemoCamera(bool);
  void updateGateDemoCamera();
  void warpPosAndAt(f32, s16);
  void warpPosAndAt(const Vec &, const Vec &);

  u32 _00[0x1C / 4];            // 0x0050
  TCameraInbetween *mInbetween; // 0x006C
  u32 _01[0x34 / 4];            // 0x0070
  u16 _02;                      // 0x00A4
  u16 mHorizontalAngle;         // 0x00A6
  f32 mInterpolateDistance;     // 0x00A8
  u32 _AC[0x350 / 4];

  static char *mCamKindNameSaveFile[73];
};