#pragma once

#include "JUTColor.hxx"
#include "types.h"


class JUTFont {
public:
  class TWidth {

  };

  JUTFont();
  virtual ~JUTFont();

  virtual void setGX() = 0;
  virtual void setGX(JUtility::TColor, JUtility::TColor);
  virtual void drawChar_scale(f32 x, f32 y, f32 w, f32 h, int ascii, bool) = 0;
  virtual bool getLeading() const = 0;
  virtual int getAscent() const = 0;
  virtual int getDescent() const = 0;
  virtual int getHeight() const = 0;
  virtual int getWidth() const = 0;
  virtual void getWidthEntry(int entry, JUTFont::TWidth *out) = 0;
  virtual int getFontType() const = 0;
  virtual bool isLeadByte(int byte) const = 0;

  void drawString_size_scale(f32, f32, f32, f32, const char *, u32, bool);
  void initiate(); // nullsub
  virtual bool isLeadByte_1Byte(int byte);
  virtual bool isLeadByte_2Byte(int byte);
  virtual bool isLeadByte_ShiftJIS(int byte);
  void setCharColor(JUtility::TColor color);
  void setGradColor(JUtility::TColor top, JUtility::TColor bottom);

  u8 _4;
  u32 _8;
  JUtility::TColor mGradColorTopL;
  JUtility::TColor mGradColorTopR;
  JUtility::TColor mGradColorBottomL;
  JUtility::TColor mGradColorBottomR;
};