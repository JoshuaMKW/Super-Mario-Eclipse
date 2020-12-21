#ifndef J3D_H
#define J3D_H

#include "types.h"
#include "MTX.h"
#include "sms/JUT.hxx"
#include "sms/JGeometry.hxx"

// declaration for J3DSkinDeform to use J3DModel
class J3DModel;
class J3DShape;
class J3DDisplayListObj;

class J3DVertexData
{
public:
	u32 _0;
	u32 _4;
	u32 _8;
	u32 _C;
	u32 _10;
	u32 _14;
	u32 _18;
	u32 _1C;
	u32 _20;
	u32 _24;
	u32 _28;
	u32 _2C;
	u32 _30;
	u32 _34;
	u32 _38;
	u32 _3C;
	u32 _40;
};

class J3DVertexBuffer
{
public:
	J3DVertexData *vtxData; // _0
	u32 _4;
	u32 _8;
	u32 _C;
	u32 _10;
	u32 _14;
	u32 _18;
	u32 _1C;
	u32 _20;
	u32 _24;
	u32 _28;
	u32 _2C;
	u32 _30;
	u32 _34;
	u32 *vTable; // _38
};

class J3DDrawMtxData
{
public:
	u16 _0;
	u16 _2; // padding?
	u32 _4;
	u32 _8;
};

class J3DModelData
{
public:
	u32 *vTable; // _0
	u32 _4;
	u32 _8;
	u32 _C;
	u32 _10;
	u32 _14;
	u16 _18;
	u16 _1A;
	u16 _1C;
	u32 _20;
	u16 _24;
	u16 _26; // padding?
	u32 _28;
	u16 _2C;
	u16 _2E; // padding?
	u32 _30; // JUTNameTab (joints)
	u16 _34;
	u16 _36; // padding?
	u32 _38;
	J3DVertexData vertexData; // _3C
	u32 _80;
	u16 _84;
	u16 _86; // padding?
	u32 _88;
	u32 _8C;
	u32 _90;
	u32 _94;
	J3DDrawMtxData drawMtxData; // _98
	u32 _A4;
	u32 _A8;
	u32 _AC;
	u32 _B0;
	u32 _B4;
	u32 _B8;
};

class J3DSkinDeform
{
public:
	u32 *vTable; // _0
	u32 _4;
	u32 _8;
	u32 _C;
	u32 _10;
};

class J3DDisplayListObj
{
public:
	u32 _0;
	u32 _4;
	u32 _8;
	u32 _C;
};

class J3DMaterial
{
public:
	u32 *_0;
	J3DShape *shape; // _4
	u32 _8;
	u16 _C;
	u16 _E; //padding?
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

class J3DNode
{
public:
	u32 *vTable; // _0
	u32 _4;
	u32 _8;
	u32 _C; // padding?
	u32 *_10;
	J3DNode *childNode; // _14
};

class J3DJoint : public J3DNode
{
public:
	u16 _18;
	u8 _1A;
	u8 _1B;
	f32 _1C;
	f32 _20;
	f32 _24;
	u16 _28;
	u16 _2A;
	u16 _2C;
	u16 _2E; // padding?
	f32 _30;
	f32 _34;
	f32 _38;
	f32 _3C;
	u32 _40;
	u32 _44;
	u32 _48;
	u32 _50;
	u32 _54;
	u32 _58;
	u32 _5C;
	J3DMaterial *material; // _60
};

class J3DModel
{
public:
	u32 *vTable;		// _0
	J3DModelData *data; // _4
	u32 _8;
	u32 _C;
	JGeometry::TVec3<f32> mSizeMultiplier;
	Mtx _20;
	u32 _50;
	u32 _54;
	Mtx *mJointArray;
	u32 _5C;
	u32 _60;
	u32 _64;
	u32 _68;
	u32 _6C;
	u32 _70;
	u32 _74;
	u32 _7C;
	u32 _80;
	u32 _84;
	u32 _88;
	J3DSkinDeform *skinDeform; // _8C
	u32 _90;
	u32 _94;
	J3DVertexBuffer vtxBuffer; // _98
};

class J3DShape
{
public:
	u32 _0;
	u16 _4;
	u16 _6;
	u32 _8;
	f32 _C;
	f32 _10;
	f32 _14;
	f32 _18;
	f32 _1C;
	f32 _20;
	f32 _24;
	u32 _2C;
	bool _30;
	u8 _31; // padding?
	u8 _32; // ^^
	u8 _33; // ^^
	u32 _34;
	u32 _38; // might be a class instance
	u32 _3C; // padding?
	u32 _40; // padding?
	u32 *_44;
	u32 _48;
	u32 _4C;
	u32 _50;
	u32 _54;
	u32 _58;
};

class J3DShapeMtx
{
public:
	u32 *vTable;  // _0
	s16 mtxIndex; // _4
	u16 _6;		  // padding?
};

class J3DDrawBuffer
{
public:
	u32 *_0;
	u32 size; // _4
	u32 _8;
	u32 _C;
	f32 _10;
	f32 _14;
	f32 _18;
	u32 _1C;
	u32 _20;
};

class J3DColorChan
{
public:
	u16 colorChanID; // _0
};

class J3DGXColor
{
};

class J3DColorBlock
{
public:
	u32 *vTable; // _0
};

class J3DTexCoord
{
public:
	u8 _0;
	u8 _1;
	u8 _2;
	u8 _3; // padding?
};

class J3DTexture
{
public:
	u16 _0;
	u16 _2;
	u32 _4;
	u32 *vTable; // _8
};

class J3DMaterialTable
{
public:
	u32 *vTable; // _0
	u16 _4;
	u16 _6;
	u32 _8;
	J3DTexture *texture; // _C
	u32 _10;
	u32 _14;
};

class J3DAnmBase
{
public:
	u16 _0;
	u16 _2;
	f32 _4;
	u32 _8;		 // guessed
	u32 *vTable; // _C
};

class J3DAnmTexPattern : public J3DAnmBase
{
public:
	u32 _10;
	u32 _14;
	u16 _18;
	u16 _1A;
	u32 _1C;
	JUTNameTab *nameTab; // _20
};

class J3DTexNoAnm
{
public:
	u32 *vTable; // _0
	u16 _4;
	u16 _6; // padding?
};

class J3DPacket
{
public:
	u32 *vTable;
	J3DPacket *parentPacket; // _4
	J3DPacket *childPacket;	 // _8
};

class J3DDrawPacket : public J3DPacket
{
public:
	u8 _C[0x30 - 0xC];
	u32 *_30;
};

class J3DCallbackPacket : public J3DPacket
{
};

class J3DShapePacket : public J3DCallbackPacket
{
};

class J3DMatPacket : public J3DDrawPacket
{
public:
	J3DShapePacket *shapePacket; // _34
	u32 _38;
	u32 _3C;
	u32 _40;
	u32 _44;
};

#endif