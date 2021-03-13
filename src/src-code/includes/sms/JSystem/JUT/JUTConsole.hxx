#pragma once

#include "types.h"
#include "../JKR/JKRDisposer.hxx"

class JUTConsole : public JKRDisposer
{
public:
    enum EConsoleType
    {
    };

    JUTConsole(unsigned int, unsigned int, bool);
    ~JUTConsole();

    void clear();
    JUTConsole *create(unsigned int, void *, u32);
    void doDraw(JUTConsole::EConsoleType) const;
    u32 getLineFromObjectSize(u32, unsigned int);
    u32 getLineOffset() const;
    u32 getUsedLine() const;
    void print(const char *);
    void print_f(const char *, ...);
    void scroll(int);

    u8 _00[0x4E];
};

void JUTConsole_print_f_va_(void);