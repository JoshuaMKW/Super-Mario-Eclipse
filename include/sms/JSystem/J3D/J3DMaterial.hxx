#pragma once

#include "J3DColor.hxx"
#include "J3DPacket.hxx"
#include "J3DShape.hxx"
#include "J3DTexture.hxx"
#include "JSU/JSUInputStream.hxx"
#include "MTX.h"
#include "types.h"

class J3DPEBlock {
public:
  virtual void reset(J3DPEBlock *);
  // virtual void setFog(J3DFog *);

  virtual ~J3DPEBlock();
};

struct J3DTevOrder {
	u8 mTexCoordId;
	u8 mTexMap;
	u8 mChannelId;
};

struct J3DTevStage {
  J3DTevStage();

  void setTevAlphaOp(u8, u8, u8, u8, u8);
  void setTevColorOp(u8, u8, u8, u8, u8);

  u8 _00;
  u8 mColorOp;
  u8 _02;
  u8 _03;
  u8 _04;
  u8 mAlphaOp;
  u8 _06;
  u8 _07;
};

struct J3DTevSwapModeInfo {};

struct J3DTevSwapModeTable {
  u8 mID;
};

struct J3DIndTevStage {
  J3DIndTevStage();

  u8 _00;
  u8 _01;
  u8 _02;
  u8 _03;
  u8 _04;
  u8 _05;
  u8 _06;
  u8 _07;
  u8 _08;
};

class J3DTevBlock {
public:
  virtual void reset(J3DTevBlock *);
  virtual u32 getType(s32 idx) = 0;
  virtual size_t countDLSize(s32 idx) = 0;
  virtual void setTexNo(s32 idx, u16 no) = 0;
  virtual void setTexNo(s32 idx, const u16 *no) = 0;
  virtual s32 getTexNo(s32 idx) = 0;
  virtual void setTevOrder(s32 idx, J3DTevOrder order) = 0;
  virtual void setTevOrder(s32 idx, const J3DTevOrder *order) = 0;
  virtual s32 getTevOrder(s32 idx) = 0;
  virtual void setTevColor(s32 idx, J3DGXColorS10 color) = 0;
  virtual void setTevColor(s32 idx, const J3DGXColorS10 *color) = 0;
  virtual s32 getTevColor(s32 idx) = 0;
  virtual void setTevKColor(s32 idx, J3DGXColor color) = 0;
  virtual void setTevKColor(s32 idx, const J3DGXColor *color) = 0;
  virtual s32 getTevKColor(s32 idx) = 0;
  virtual void setTevKColorSel(s32 idx, u8 sel) = 0;
  virtual void setTevKColorSel(s32 idx, const u8 *sel) = 0;
  virtual s32 getTevKColorSel(s32 idx) = 0;
  virtual void setTevKAlphaSel(s32 idx, u8 sel) = 0;
  virtual void setTevKAlphaSel(s32 idx, const u8 *sel) = 0;
  virtual s32 getTevKAlphaSel(s32 idx) = 0;
  virtual void setTevStageNum(u8 num) = 0;
  virtual void setTevStageNum(const u8 *num) = 0;
  virtual s32 getTevStageNum(s32 idx) = 0;
  virtual void setTevStage(s32 idx, J3DTevStage stage) = 0;
  virtual void setTevStage(s32 idx, const J3DTevStage *stage) = 0;
  virtual s32 getTevStage(s32 idx) = 0;
  virtual void setTevSwapModeInfo(s32 idx, J3DTevSwapModeInfo info) = 0;
  virtual void setTevSwapModeInfo(s32 idx, const J3DTevSwapModeInfo *info) = 0;
  virtual void setTevSwapModeTable(s32 idx, J3DTevSwapModeTable info) = 0;
  virtual void setTevSwapModeTable(s32 idx, const J3DTevSwapModeTable *info) = 0;
  virtual s32 getTevSwapModeTable(s32 idx) = 0;
  virtual void setIndTevStage(s32 idx, J3DIndTevStage stage) = 0;
  virtual void setIndTevStage(s32 idx, const J3DIndTevStage *stage) = 0;
  virtual s32 getIndTevStage(s32 idx) = 0;
  virtual ~J3DTevBlock();
  virtual void load() = 0;
};


class J3DTevBlock1 : public J3DTevBlock {
public:
  virtual void reset(J3DTevBlock *) override;
  virtual u32 getType(s32 idx) override;
  virtual size_t countDLSize(s32 idx) override;
  virtual void setTexNo(s32 idx, u16 no) override;
  virtual void setTexNo(s32 idx, const u16 *no) override;
  virtual s32 getTexNo(s32 idx) override;
  virtual void setTevOrder(s32 idx, J3DTevOrder order) override;
  virtual void setTevOrder(s32 idx, const J3DTevOrder *order) override;
  virtual s32 getTevOrder(s32 idx) override;
  virtual void setTevColor(s32 idx, J3DGXColorS10 color) override;
  virtual void setTevColor(s32 idx, const J3DGXColorS10 *color) override;
  virtual s32 getTevColor(s32 idx) override;
  virtual void setTevKColor(s32 idx, J3DGXColor color) override;
  virtual void setTevKColor(s32 idx, const J3DGXColor *color) override;
  virtual s32 getTevKColor(s32 idx) override;
  virtual void setTevKColorSel(s32 idx, u8 sel) override;
  virtual void setTevKColorSel(s32 idx, const u8 *sel) override;
  virtual s32 getTevKColorSel(s32 idx) override;
  virtual void setTevKAlphaSel(s32 idx, u8 sel) override;
  virtual void setTevKAlphaSel(s32 idx, const u8 *sel) override;
  virtual s32 getTevKAlphaSel(s32 idx) override;
  virtual void setTevStageNum(u8 num) override;
  virtual void setTevStageNum(const u8 *num) override;
  virtual s32 getTevStageNum(s32 idx) override;
  virtual void setTevStage(s32 idx, J3DTevStage stage) override;
  virtual void setTevStage(s32 idx, const J3DTevStage *stage) override;
  virtual s32 getTevStage(s32 idx) override;
  virtual void setTevSwapModeInfo(s32 idx, J3DTevSwapModeInfo info) override;
  virtual void setTevSwapModeInfo(s32 idx, const J3DTevSwapModeInfo *info) override;
  virtual void setTevSwapModeTable(s32 idx, J3DTevSwapModeTable info) override;
  virtual void setTevSwapModeTable(s32 idx, const J3DTevSwapModeTable *info) override;
  virtual s32 getTevSwapModeTable(s32 idx) override;
  virtual void setIndTevStage(s32 idx, J3DIndTevStage stage) override;
  virtual void setIndTevStage(s32 idx, const J3DIndTevStage *stage) override;
  virtual s32 getIndTevStage(s32 idx) override;
  virtual ~J3DTevBlock1();
  virtual void load() override;
};


class J3DTevBlock2 : public J3DTevBlock {
public:
  virtual void reset(J3DTevBlock *) override;
  virtual u32 getType(s32 idx) override;
  virtual size_t countDLSize(s32 idx) override;
  virtual void setTexNo(s32 idx, u16 no) override;
  virtual void setTexNo(s32 idx, const u16 *no) override;
  virtual s32 getTexNo(s32 idx) override;
  virtual void setTevOrder(s32 idx, J3DTevOrder order) override;
  virtual void setTevOrder(s32 idx, const J3DTevOrder *order) override;
  virtual s32 getTevOrder(s32 idx) override;
  virtual void setTevColor(s32 idx, J3DGXColorS10 color) override;
  virtual void setTevColor(s32 idx, const J3DGXColorS10 *color) override;
  virtual s32 getTevColor(s32 idx) override;
  virtual void setTevKColor(s32 idx, J3DGXColor color) override;
  virtual void setTevKColor(s32 idx, const J3DGXColor *color) override;
  virtual s32 getTevKColor(s32 idx) override;
  virtual void setTevKColorSel(s32 idx, u8 sel) override;
  virtual void setTevKColorSel(s32 idx, const u8 *sel) override;
  virtual s32 getTevKColorSel(s32 idx) override;
  virtual void setTevKAlphaSel(s32 idx, u8 sel) override;
  virtual void setTevKAlphaSel(s32 idx, const u8 *sel) override;
  virtual s32 getTevKAlphaSel(s32 idx) override;
  virtual void setTevStageNum(u8 num) override;
  virtual void setTevStageNum(const u8 *num) override;
  virtual s32 getTevStageNum(s32 idx) override;
  virtual void setTevStage(s32 idx, J3DTevStage stage) override;
  virtual void setTevStage(s32 idx, const J3DTevStage *stage) override;
  virtual s32 getTevStage(s32 idx) override;
  virtual void setTevSwapModeInfo(s32 idx, J3DTevSwapModeInfo info) override;
  virtual void setTevSwapModeInfo(s32 idx, const J3DTevSwapModeInfo *info) override;
  virtual void setTevSwapModeTable(s32 idx, J3DTevSwapModeTable info) override;
  virtual void setTevSwapModeTable(s32 idx, const J3DTevSwapModeTable *info) override;
  virtual s32 getTevSwapModeTable(s32 idx) override;
  virtual void setIndTevStage(s32 idx, J3DIndTevStage stage) override;
  virtual void setIndTevStage(s32 idx, const J3DIndTevStage *stage) override;
  virtual s32 getIndTevStage(s32 idx) override;
  virtual ~J3DTevBlock2();
  virtual void load() override;
};


class J3DTevBlock4 : public J3DTevBlock {
public:
  virtual void reset(J3DTevBlock *) override;
  virtual u32 getType(s32 idx) override;
  virtual size_t countDLSize(s32 idx) override;
  virtual void setTexNo(s32 idx, u16 no) override;
  virtual void setTexNo(s32 idx, const u16 *no) override;
  virtual s32 getTexNo(s32 idx) override;
  virtual void setTevOrder(s32 idx, J3DTevOrder order) override;
  virtual void setTevOrder(s32 idx, const J3DTevOrder *order) override;
  virtual s32 getTevOrder(s32 idx) override;
  virtual void setTevColor(s32 idx, J3DGXColorS10 color) override;
  virtual void setTevColor(s32 idx, const J3DGXColorS10 *color) override;
  virtual s32 getTevColor(s32 idx) override;
  virtual void setTevKColor(s32 idx, J3DGXColor color) override;
  virtual void setTevKColor(s32 idx, const J3DGXColor *color) override;
  virtual s32 getTevKColor(s32 idx) override;
  virtual void setTevKColorSel(s32 idx, u8 sel) override;
  virtual void setTevKColorSel(s32 idx, const u8 *sel) override;
  virtual s32 getTevKColorSel(s32 idx) override;
  virtual void setTevKAlphaSel(s32 idx, u8 sel) override;
  virtual void setTevKAlphaSel(s32 idx, const u8 *sel) override;
  virtual s32 getTevKAlphaSel(s32 idx) override;
  virtual void setTevStageNum(u8 num) override;
  virtual void setTevStageNum(const u8 *num) override;
  virtual s32 getTevStageNum(s32 idx) override;
  virtual void setTevStage(s32 idx, J3DTevStage stage) override;
  virtual void setTevStage(s32 idx, const J3DTevStage *stage) override;
  virtual s32 getTevStage(s32 idx) override;
  virtual void setTevSwapModeInfo(s32 idx, J3DTevSwapModeInfo info) override;
  virtual void setTevSwapModeInfo(s32 idx, const J3DTevSwapModeInfo *info) override;
  virtual void setTevSwapModeTable(s32 idx, J3DTevSwapModeTable info) override;
  virtual void setTevSwapModeTable(s32 idx, const J3DTevSwapModeTable *info) override;
  virtual s32 getTevSwapModeTable(s32 idx) override;
  virtual void setIndTevStage(s32 idx, J3DIndTevStage stage) override;
  virtual void setIndTevStage(s32 idx, const J3DIndTevStage *stage) override;
  virtual s32 getIndTevStage(s32 idx) override;
  virtual ~J3DTevBlock4();
  virtual void load() override;
};


class J3DTevBlock16 : public J3DTevBlock {
public:
  virtual void reset(J3DTevBlock *) override;
  virtual u32 getType(s32 idx) override;
  virtual size_t countDLSize(s32 idx) override;
  virtual void setTexNo(s32 idx, u16 no) override;
  virtual void setTexNo(s32 idx, const u16 *no) override;
  virtual s32 getTexNo(s32 idx) override;
  virtual void setTevOrder(s32 idx, J3DTevOrder order) override;
  virtual void setTevOrder(s32 idx, const J3DTevOrder *order) override;
  virtual s32 getTevOrder(s32 idx) override;
  virtual void setTevColor(s32 idx, J3DGXColorS10 color) override;
  virtual void setTevColor(s32 idx, const J3DGXColorS10 *color) override;
  virtual s32 getTevColor(s32 idx) override;
  virtual void setTevKColor(s32 idx, J3DGXColor color) override;
  virtual void setTevKColor(s32 idx, const J3DGXColor *color) override;
  virtual s32 getTevKColor(s32 idx) override;
  virtual void setTevKColorSel(s32 idx, u8 sel) override;
  virtual void setTevKColorSel(s32 idx, const u8 *sel) override;
  virtual s32 getTevKColorSel(s32 idx) override;
  virtual void setTevKAlphaSel(s32 idx, u8 sel) override;
  virtual void setTevKAlphaSel(s32 idx, const u8 *sel) override;
  virtual s32 getTevKAlphaSel(s32 idx) override;
  virtual void setTevStageNum(u8 num) override;
  virtual void setTevStageNum(const u8 *num) override;
  virtual s32 getTevStageNum(s32 idx) override;
  virtual void setTevStage(s32 idx, J3DTevStage stage) override;
  virtual void setTevStage(s32 idx, const J3DTevStage *stage) override;
  virtual s32 getTevStage(s32 idx) override;
  virtual void setTevSwapModeInfo(s32 idx, J3DTevSwapModeInfo info) override;
  virtual void setTevSwapModeInfo(s32 idx, const J3DTevSwapModeInfo *info) override;
  virtual void setTevSwapModeTable(s32 idx, J3DTevSwapModeTable info) override;
  virtual void setTevSwapModeTable(s32 idx, const J3DTevSwapModeTable *info) override;
  virtual s32 getTevSwapModeTable(s32 idx) override;
  virtual void setIndTevStage(s32 idx, J3DIndTevStage stage) override;
  virtual void setIndTevStage(s32 idx, const J3DIndTevStage *stage) override;
  virtual s32 getIndTevStage(s32 idx) override;
  virtual ~J3DTevBlock16();
  virtual void load() override;
};

class J3DIndBlock {
public:
  virtual void reset(J3DIndBlock *);

  virtual ~J3DIndBlock();
};

class J3DTexBlock {
public:
  virtual void reset(J3DTexBlock *);

  virtual ~J3DTexBlock();
};

class J3DTexGenBlock {
public:
  virtual void reset(J3DTexGenBlock *);

  virtual ~J3DTexGenBlock();
};

class J3DMaterial {
public:
  J3DMaterial();
  ~J3DMaterial();

  void addShape(J3DShape *);
  void calc(Mtx44 *);
  void change();
  void copy(J3DMaterial *);
  size_t countDLSize();
  J3DColorBlock *createColorBlock(int);
  J3DIndBlock *createIndBlock(int);
  J3DPEBlock *createPEBlock(int, u32);
  J3DTevBlock *createTevBlock(int);
  J3DTexGenBlock *createTexGenBlock(int);
  void initialize();
  void load();
  void makeDisplayList();
  J3DDisplayListObj *newSharedDisplayList(u32);
  void safeMakeDisplayList();
  void setCurrentMtx();

  u32 *_0;
  J3DShape *shape; // _4
  u32 _8;
  u16 _C;
  u16 _E; // padding?
  u32 _10;
  u32 _14; // padding?
  u32 *_18;
  u32 _1C;
  u32 *_20; // *TItemManager
  u32 *_24; // *TItemManager
  u32 *_28;
  u32 _2C; // *TItemManager
  u32 _30;
  u32 *_34;
  u32 _38;
  J3DDisplayListObj dispListObj; // _3C
};

class J3DMaterialBlock {
public:
  u32 _00;
  u32 _04;
  u16 _08;
  u32 _0C;
  u32 _10;
  u32 mMatOffset;
};

class J3DMaterialBlock_v21 : public J3DMaterialBlock {};

class J3DMaterialTable {
public:
  J3DMaterialTable();
  virtual ~J3DMaterialTable();

  void clear();

  s16 _04;
  u32 _08;
  J3DTexture *mTexture;
  u32 _10;
  u32 _14;
};

class J3DColorChan {
public:
  u16 colorChanID; // _0
};
