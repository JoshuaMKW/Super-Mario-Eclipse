#pragma once

#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JGadget/SingleLinkList.hxx"

class TPerformList : public JDrama::TNameRef {
public:
    u32 _0C;
    JGadget::TSingleNodeLinkedList mPerformList;
};