#pragma once

#include "Allocator.hxx"
#include "Node.hxx"
#include "types.h"

namespace JGadget {

namespace {
template <typename _T> class TPRIsEqual_Pointer_ {
  _T *pointer;

public:
  TPRIsEqual_Pointer_() = delete;
  TPRIsEqual_Pointer_(_T *p) : pointer(p) {}

  bool operator==(TPRIsEqual_Pointer_ &rhs) const {
    return pointer == rhs.pointer;
  }
  bool operator==(_T *rhs) const { return pointer == rhs; }
};
} // namespace

class TNodeLinkList {
public:
  class iterator {
  public:
    iterator(TLinkListNode *node) : mCurrent(node) {}
    iterator(const iterator &iter) : mCurrent(iter.mCurrent) {}

    bool operator==(iterator &rhs) const { return mCurrent == rhs.mCurrent; }
    bool operator!=(iterator &rhs) const { return mCurrent != rhs.mCurrent; }
    TLinkListNode *operator->() const { return mCurrent; }

    TLinkListNode *mCurrent;
  };

  TLinkListNode *CreateNode_(TLinkListNode *prev, TLinkListNode *next,
                             void **item);

  iterator begin() { return iterator(mStart.mCurrent); }
  iterator end();
  iterator Erase(iterator start, iterator end);
  iterator Insert(iterator iter);
  void Remove(TLinkListNode *node);
  template <typename _T> void Remove_if(_T _p, TNodeLinkList &list);

  size_t mSize;
  iterator mStart;
};

template <class _T, template<class> class _A> class TList {
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

  TList(_A<_T> *allocator) { _00 = allocator->_00; }

  TNode_ *CreateNode_(TNode_ *prev, TNode_ *next, void **item);

  iterator begin() { return iterator(mStart.mCurrent); }
  iterator end() { return mStart; }

  iterator erase(iterator iter) {
    TNode_ *prev = iter->prev;
    TNode_ *next = iter->next;

    next->mPrev = prev;
    prev->mNext = next;
    delete iter.mCurrent;

    mSize -= 1;
    return iterator(prev);
  }

  iterator erase(iterator start, iterator end) {
    iterator iter = start;
    while (iter != end) {
      iter = erase(iter);
    }
    return iterator(iter);
  }

  iterator insert(iterator iter, const _T *&node) {
    TNode_ *current = iter.mCurrent;
    TNode_ *next = current->mNext;

    TNode_ *newNode = CreateNode_(iter.mCurrent, iter.mCurrent, &node);
    if (!newNode)
      return mStart;

    current->mNext = newNode;
    next->mPrev = newNode;
    mSize += 1;

    return iterator(newNode);
  }

  u8 _00;
  size_t mSize;
  iterator mStart;
};

template <typename _T, size_t _S> class TLinkList {
public:
  TLinkListNode *Element_getNode();
  _T *Element_getValue();
};

class TSingleNodeLinkList {
public:
  class iterator {
  public:
    iterator(TSingleLinkListNode **nodelist);
    TSingleLinkListNode **mNodeList;
  };

  void Initialize();
  iterator Insert(iterator iter, TSingleLinkListNode *node);
  iterator end();

  size_t mSize;
  TSingleLinkListNode *mStart;
  TSingleLinkListNode *mEnd;
};

template <typename _T, size_t _S> class TSingleLinkList {
public:
  class iterator {
  public:
    iterator(TSingleLinkListNode *node) : mCurrent(node) {}
    iterator(const iterator &iter) : mCurrent(iter.mCurrent) {}

    TSingleLinkListNode *mCurrent;
  };
  size_t mSize;
  TSingleLinkList mNode;
};

class TList_pointer_void : public TAllocator<void *> {
public:
  void insert(TList<void *, TAllocator>::iterator iterator,
              void **node);
};

template <typename _T> class TList_pointer : public TList_pointer_void {
  class iterator {
  public:
    iterator(_T *node) : mCurrent(node) {}
    iterator(const iterator &iter) : mCurrent(iter.mCurrent) {}

    _T *mCurrent;
  };

  iterator end() { return iterator(mEnd); }

  void insert(TList_pointer<_T>::iterator iterator, _T **node) {
    TList_pointer_void::insert(
        reinterpret_cast<TList<void *, TAllocator>::iterator>(iterator),
        node);
  }

  size_t mSize;
  TSingleLinkListNode *mStart;
  TSingleLinkListNode *mEnd;
};

} // namespace JGadget