#include "J2D/J2DOrthoGraph.hxx"
#include "J2D/J2DPane.hxx"
#include "J2D/J2DPicture.hxx"
#include "J2D/J2DScreen.hxx"
#include "J2D/J2DTextBox.hxx"
#include "SME.hxx"

class TOption {
public:
  enum class Kind { INT, FLOAT, BOOL, PAGE };

  TOption(const char *name, Kind kind);
  virtual ~TOption();

  const char *getName() const { return mName; }
  void setName(const char *name) { mName = name; }
  Kind getKind() const { return mKind; }
  void setKind(Kind kind) { mKind = kind; }

  virtual void *getValue() const = 0;
  virtual void setValue(s32 value) = 0;
  virtual void setValue(f32 value) = 0;
  virtual void setValue(bool value) = 0;

  virtual f32 getMin() = 0;
  virtual f32 getMax() = 0;
  virtual void setMin(f32 min) = 0;
  virtual void setMax(f32 max) = 0;

  virtual void incValue(s32 value) = 0;

private:
  const char *mName;
  Kind mKind;
};

class TOptionEntryValue : public TOption {
  virtual void *getValue() const override { return (void *)&mValue; }
  virtual void setValue(s32 value) override { mValue.asInt = value; }
  virtual void setValue(f32 value) override { mValue.asFloat = value; }
  virtual void setValue(bool value) override { mValue.asBool = value; }

  virtual f32 getMin() override { return mMin; }
  virtual f32 getMax() override { return mMax; }
  virtual void setMin(f32 min) override { mMin = min; }
  virtual void setMax(f32 max) override { mMax = max; }

  virtual void incValue(s32 value) override {
    switch (getKind()) {
    case Kind::INT:
      mValue.asInt += value;
      if (mValue.asInt > mMax) {
        mValue.asInt = mMin + (mValue.asInt - mMax);
        mEventCB(this);
      }
      break;
    case Kind::FLOAT:
      mValue.asFloat += value;
      if (mValue.asFloat > mMax) {
        mValue.asFloat = mMin + (mValue.asFloat - mMax);
        mEventCB(this);
      }
      break;
    case Kind::BOOL:
      mValue.asBool == true ? mValue.asBool = false : mValue.asBool = true;
      break;
    default:
      break;
    }
  }

  typedef void (*TOptionCallback)(TOptionEntryValue *option);

private:
  union {
    s32 asInt;
    f32 asFloat;
    bool asBool;
  } mValue;
  f32 mMin, mMax;
  TOptionCallback mEventCB;
};

class TOptionPage : JDrama::TNameRef {
public:
  TOptionPage(const char *name, TOptionPage *parent);
  virtual ~TOptionPage();

  virtual void perform(u32 flags, JDrama::TGraphics *graphics);
  virtual void move(s32 index);
  virtual void enterPage();

  bool insertOption(TOptionEntryValue *option, s32 index);
  void addOption(TOptionEntryValue *option);
  void removeOption(TOptionEntryValue *option);
  void removeOption(const char *optionName);

private:
  J2DScreen *mScreen;
  TOptionPage *mParent;
  JGadget::TList<TOptionEntryValue *, JGadget::TAllocator> mOptions;
};

class TOptionSelectMenu : public TOptionPage {
public:
  TOptionSelectMenu(const char *name);
  virtual ~TOptionSelectMenu();

  void move(s32 index);
  void enterPage();
};