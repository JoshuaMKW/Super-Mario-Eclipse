#pragma once

#include "Allocator.hxx"
#include "Node.hxx"
#include "types.h"

namespace JGadget {

template <class _T, template<class> class _A> class TVector {
  class TNode_ {
    TNode_ *mPrev;
    TNode_ *mNext;
    void *mItem;
  };

public:
  class iterator {
  public:
    iterator(TNode_ *node) : mCurrent(node) {}
    iterator(const iterator &iter) : mCurrent(iter.mCurrent) {}

    bool operator==(iterator &rhs) const { return mCurrent == rhs.mCurrent; }
    bool operator!=(iterator &rhs) const { return mCurrent != rhs.mCurrent; }
    TNode_ *operator->() const { return mCurrent; }

    TNode_ *mCurrent;
  };

  Vector(_A<_T> allocator) { _00 = allocator->_00; }

  TNode_ *CreateNode_(TNode_ *prev, TNode_ *next, void **item);
  void DestroyElement_(void **a, void **b) {
    for (; a != b; ++a) {
    }
  }
  void InsertRaw(void **a, u32); // complete later

  iterator begin() { return iterator(mStart.mCurrent); }
  iterator end() { return iterator(mEnd.mCurrent); }
  iterator erase(_T *a, _T *b);       // complete later
  iterator insert(_T *a, u32, _T *b); // complete later

  u8 _00;
  iterator mStart;
  iterator mEnd;
  u32 _0C;
  f32 _10;
  u32 _14;

  class TVector_pointer_void : public TVector<void *, TAllocator> {
    TVector_pointer_void();
    ~TVector_pointer_void();

    void insert(void **a, void **b);
    void reserve(size_t);
  };
};

} // namespace JGadget