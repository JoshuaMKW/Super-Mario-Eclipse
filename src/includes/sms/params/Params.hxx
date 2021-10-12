#pragma once

#include "BaseParam.hxx"
#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JKR/JKRMemArchive.hxx"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"
#include "types.h"
#include "macros.h"

class TParams {
public:
  TParams() : _00(0), mBaseParam(nullptr) {}
  ~TParams() {}

  void finalize();
  void init();
  void load(const char *);
  void load(JSUMemoryInputStream &);

  u32 _00;
  TBaseParam *mBaseParam;

  static JKRMemArchive *mArc;
  static JKRMemArchive *mSceneArc;
};

// sizeof = 0x14
template <typename T> class TParamT : public TBaseParam {
  T mValue;

public:
  TParamT(TParams *parent, T param, u16 keycode, const char *name)
      : TBaseParam(parent, keycode, name), mValue(param) {}

  T get() const { return mValue; }
  void set(T param) { mValue = param; }

  void load(JSUMemoryInputStream &stream) {
    u32 fakeit;
    SME_FROM_GPR(29, fakeit); //Hack to keep r29 from being used..

    u32 buffer;
    stream.read(&buffer, 4);
    stream.read(&mValue, sizeof(T));
  }
};

template <typename T> class TParamRT : public TParamT<T> {
public:
  TParamRT(TParams *parent, T param, u16 keycode, const char *name)
      : TParamT<T>(parent, param, keycode, name) {}

  void set(T param){};
};