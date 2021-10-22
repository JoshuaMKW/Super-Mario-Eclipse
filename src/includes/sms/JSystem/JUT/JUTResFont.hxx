#pragma once

#include "GX.h"
#include "types.h"


#include "JKR/JKRArchivePri.hxx"
#include "JUTColor.hxx"
#include "JUTFont.hxx"


class ResFONT {};

class JUTResFont : public JUTFont {
public:
  JUTResFont(const u32 *, JKRArchive *); // ResFont
  virtual ~JUTResFont();

  virtual void setGX();
  virtual void setGX(JUtility::TColor, JUtility::TColor);
  virtual void drawChar_scale(f32 x, f32 y, f32 w, f32 h, int ascii, bool);
  virtual bool getLeading() const;
  virtual int getAscent() const;
  virtual int getDescent() const;
  virtual int getHeight() const;
  virtual int getWidth() const;
  virtual void getWidthEntry(int entry, JUTFont::TWidth *out);
  virtual int getFontType() const;
  virtual bool isLeadByte(int byte) const;

  void convertSjis(int, u16 *);
  void countBlock();
  void initiate(const u32 *, JKRArchive *); // ResFont
  void loadFont();
  void loadImage(int, GXTexMapID tmapID);

  void protected_initiate();

  u32 _1C;
  u32 _20;
  u8 _24[0x44 - 0x24]; // GXTexObj
  u32 _44;
  ResFONT *mFont;
  u32 _4C; // padding?
  u32 _50;
  u32 _54;
  u32 _58;
  u16 _5C;
  u16 _5E;
  u16 _60;
  u16 _62;
  u16 _64;
  u16 _66;
  u32 _68;
};