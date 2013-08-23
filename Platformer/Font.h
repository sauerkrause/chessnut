#ifndef __FONT_H_INCLUDED__
#define __FONT_H_INCLUDED__

#include <d3d9.h>
#include <d3dx9.h>
#include "Common.h"

class Font : UniversalInterface<Font>
{
public:
	Font();
	~Font();
	bool Init();
	bool Process();
	void Release();
	void RenderText(char *p, float left, float right, float top, unsigned int numberoflines = 1);
	
private:
	ID3DXFont *m_pFont;
};

#endif