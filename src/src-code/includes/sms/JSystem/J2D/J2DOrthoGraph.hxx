#pragma once

#include "types.h"
#include "J2DPane.hxx"
#include "../JUT/JUTRect.hxx"

class J2DOrthoGraph : public J2DGrafContext
{
public:
    J2DOrthoGraph(u32, u32, u32, u32);
	J2DOrthoGraph(const JUTRect &);
	~J2DOrthoGraph();

	void setPort();
	void setLookat();
	void scissorBounds(JUTRect *, JUTRect *);

    JUTRect _D8;
    u32 _DC;
    u32 _E0;
    u32 _E4;
    f32 _E8;
    f32 _EC;
};