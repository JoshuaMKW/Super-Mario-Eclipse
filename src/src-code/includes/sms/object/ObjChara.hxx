#pragma once

#include "types.h"
#include "sms/JDrama.hxx"
#include "sms/JSUStream.hxx"

class TObjChara : public JDrama::TCharacter
{
    public:
    virtual ~TObjChara();

    virtual void load(JSUMemoryInputStream &);
    virtual JKRArchive *getGlbResource(const char *) const;
};