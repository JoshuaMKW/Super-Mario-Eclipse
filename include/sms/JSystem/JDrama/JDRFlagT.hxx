#pragma once

namespace JDrama {

template <typename T> class TFlagT {
public:
  TFlagT(T);
  TFlagT(const TFlagT &);
  void set(T);

private:
  T mFlag;
};

} // namespace JDrama