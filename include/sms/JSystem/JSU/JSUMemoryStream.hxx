#pragma once

#include "JSUInputStream.hxx"
#include "JSUOutputStream.hxx"
#include "types.h"

class JSUMemoryInputStream : public JSURandomInputStream {
public:
  JSUMemoryInputStream(void *buffer, size_t size)
      : mBuffer(buffer), mLength(size), mPosition(0) {}
  virtual ~JSUMemoryInputStream();

  virtual void readData(void *, s32) override;
  virtual size_t getLength() const override;
  virtual s32 getPosition() const override;
  virtual s32 seekPos(s32, JSUStreamSeekFrom) override;

  void setBuffer(const void *, s32);

private:
  void *mBuffer;
  size_t mLength; // _C
  s32 mPosition;  // _10
};

class JSUMemoryOutputStream : public JSURandomOutputStream {
public:
  JSUMemoryOutputStream(void *buffer, size_t size)
      : mBuffer(buffer), mLength(size), mPosition(0) {}
  virtual ~JSUMemoryOutputStream();

  virtual void writeData(void *, s32) override;
  virtual size_t getLength() const override;
  virtual s32 getPosition() const override;
  virtual s32 seek(s32, JSUStreamSeekFrom) override;
  virtual s32 getAvailable() const override;
  virtual s32 seekPos(s32, JSUStreamSeekFrom) override;

private:
  void *mBuffer;
  size_t mLength; // _C
  s32 mPosition;  // _10
};

#define JSU_READ_PARAM(param, stream) stream.read(&param, sizeof(typeof(param))) 