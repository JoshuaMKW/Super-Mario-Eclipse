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

template <class _T, template <class> class _A> class TList {
  struct TNode_ {
    TNode_ *mPrev;
    TNode_ *mNext;
    _T mItem;
  };

public:
  class iterator {
  public:
    iterator(TNode_ *node) : mCurrent(node) {}
    iterator(const iterator &iter) : mCurrent(iter.mCurrent) {}

    bool operator==(iterator &rhs) const { return mCurrent == rhs.mCurrent; }
    bool operator!=(iterator &rhs) const { return mCurrent != rhs.mCurrent; }
    TNode_ *operator->() const { return mCurrent; }
    iterator operator++() { return iterator(mCurrent->mNext); }
    iterator operator--() { return iterator(mCurrent->mPrev); }
    iterator operator*() { return iterator(mCurrent); }

    TNode_ *mCurrent;
  };

  TList() : _00(0), mSize(0), mStart(nullptr) {}
  TList(_A<_T> *allocator) { _00 = allocator->_00; }

  TNode_ *CreateNode_(TNode_ *prev, TNode_ *next, const _T &item) {
    TNode_ *node = new TNode_();
    if (!node)
      return nullptr;
    node->mPrev = prev;
    node->mNext = next;
    if (item)
      node->mItem = item;
    return node;
  }

  iterator begin() { return iterator(mStart.mCurrent); }
  iterator end() { return mStart; }

  iterator erase(iterator iter) {
    TNode_ *prev = iter->mPrev;
    TNode_ *next = iter->mNext;

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

  iterator insert(iterator iter, const _T &node) {
    TNode_ *current = iter.mCurrent;
    TNode_ *next = current->mNext;

    TNode_ *newNode = CreateNode_(iter.mCurrent, iter.mCurrent,
                                  node);// reinterpret_cast<void *const *>(&node));
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

using void_item = void *;
class TList_pointer_void : public TAllocator<void_item> {
public:
  void insert(TList<void_item, TAllocator>::iterator iterator, const void_item &node);
};

template <typename _T> class TList_pointer : public TList_pointer_void {

  
  class TNode_ {
    public:
      TNode_ *mPrev;
      TNode_ *mNext;
      void *mItem;
  };

  public:
  class iterator {
  public:
    iterator(_T *node) : mCurrent(node) {}
    iterator(const iterator &iter) : mCurrent(iter.mCurrent) {}

    _T *mCurrent;
  };

  iterator end();

  iterator insert(TList_pointer<_T>::iterator iterator, _T const &node);

  size_t mSize;
  TNode_ *mStart;
  TNode_ *mEnd;
};

} // namespace JGadget