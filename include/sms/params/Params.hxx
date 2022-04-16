#pragma once

#include "macros.h"
#include "types.h"

#include "BaseParam.hxx"
#include "JKR/JKRMemArchive.hxx"
#include "JSU/JSUMemoryStream.hxx"

class TParams {
public:
  TParams() : mPrmPath(nullptr), mBaseParam(nullptr) {}
  TParams(const char *prm) : mPrmPath(prm), mBaseParam(nullptr) {}
  ~TParams() {}

  static void finalize();
  
  void init();
  void load(const char *);
  void load(JSUMemoryInputStream &);

  const char *mPrmPath;
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

  void load(JSUMemoryInputStream &stream) override {
    u32 fakeit;
    SME_FROM_GPR(29, fakeit); // Hack to keep r29 from being used..

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