#pragma once

namespace JGadget {

class TSingleLinkListNode {
public:
  TSingleLinkListNode *mPrev;
  TSingleLinkListNode *mNext;
};

class TLinkListNode {
  friend class TNodeLinkList;

public:
  TLinkListNode *getNext();
  TLinkListNode *getPrev();

protected:
  TLinkListNode *mNext;
  TLinkListNode *mPrev;
};

} // namespace JGadget