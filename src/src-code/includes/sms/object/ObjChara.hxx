#pragma once

#include "types.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"

class TObjChara : public JDrama::TCharacter
{
    public:
    virtual ~TObjChara();

    virtual void load(JSUMemoryInputStream &);
    virtual JKRArchive *getGlbResource(const char *) const;
};