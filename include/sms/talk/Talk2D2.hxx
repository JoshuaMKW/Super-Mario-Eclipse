#pragma once

#include "types.h"
#include "JSU/JSUMemoryStream.hxx"

enum BoxState {
    BOX_IDLE = 0,
    BOX_SCROLLING = 4,
    BOX_WAITING = 5,
    BOX_CLOSE = 6,
    BOX_NEXTMSG = 7
};
        

class Talk2D2
{

    // 0x220 = s16 mMessageCurve;
    // 0x224 = u8 mLineCharIndex[3];
    // 0x228 = size_t mLineCharNum[3];
    // 0x234 = f32 mBoxScrollPoint[3];
    // 0x251 = s8 mMessageDelay;
    // 0x25C = TMessageLoader *mMessageLoader;
    // 0x26A = bool mIsMessageEnding;
    // 0x274 = s32 mLastLineIndex;
    // 0x278 = s32 mCurLineIndex;
    // 0x2DE = s16 mBoxCharIndex;
    // 0x330 = s16 mPosX;
    // 0x332 = s16 mPosY;
    // 0x334 = s16 mRot;
    // 0x338 = f32 mBoxAppearRate;
    // 0x33C = f32 mTextDelayInvScale;

public:

    void setTagParam(JSUMemoryInputStream &stream, J2DTextBox &textbox, int *charLength, int *lineNum);

    u32 _00[0x28 / 4];  //0x0000
    bool isBillBoard;   //0x0028
    u8 _01;             //0x0029
    u16 _02;            //0x002A
    u32 _03[0x248 / 4]; //0x002C
    u32 lastMsgIndex;   //0x0274
    u32 curMsgIndex;    //0x0278
};