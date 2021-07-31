#pragma once

namespace JGadget {

class TSingleLinkListNode {
    TSingleLinkListNode *mPrev;
    TSingleLinkListNode *mNext;
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
    TSingleLinkListNode mNode;
};

template <typename T_node, size_t size>
class TSingleLinkList {
public:
    class iterator {
    public:
        iterator(TSingleLinkListNode::iterator iter);
    };
};
    
}