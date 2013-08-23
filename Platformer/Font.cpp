#include "Font.h"
#include "Renderer.h"

Font::Font()
{

}

Font::~Font()
{

}

bool Font::Init()
{
	return (D3D_OK == D3DXCreateFont( gRenderer->m_pD3DDevice, 16, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_pFont ));
}

bool Font::Process()
{
	return true;
}

void Font::RenderText(char *p, float left, float right, float top, unsigned int numberoflines)
{
	D3DCOLOR fontColor = D3DCOLOR_ARGB(255,255,255,255);
	RECT rct;
	rct.left=(int)(left * (float)gRenderer->GetWidth());
	rct.right=(int)(right * (float)gRenderer->GetWidth());
	rct.top=(int)(top * (float)gRenderer->GetHeight());
	rct.bottom=rct.top+(16*numberoflines);
	HRESULT res = m_pFont->DrawText(NULL, p, -1, &rct, 0, fontColor);
	if(SUCCEEDED(res))
		return;
	DEBUGPRINTF("Failed to draw text in rect: %d,%d,%d,%d\n",rct.top,rct.left, rct.bottom,rct.right);
	return;
}

void Font::Release()
{
	if(m_pFont)
	{
		m_pFont->Release();
		m_pFont = NULL;
	}
}