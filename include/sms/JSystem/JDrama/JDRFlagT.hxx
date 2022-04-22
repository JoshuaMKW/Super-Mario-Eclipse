#pragma once

namespace JDrama {

template <typename T> class TFlagT {
public:
  TFlagT(T val) : mFlag(val) {}
  TFlagT(const TFlagT &);
  void set(T);

private:
  T mFlag;
};

} // namespace JDrama