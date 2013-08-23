#ifndef __TEXTURE_ENTRY_H_INCLUDED__
#define __TEXTURE_ENTRY_H_INCLUDED__

#include <d3d9.h>
#include <string>

class TextureEntry
{
public:
	TextureEntry(bool isManaged);

	~TextureEntry();
	void Release();
	void Restore();

	bool m_bIsManaged;
	std::string m_strName;
	UINT m_xSize, m_ySize;
	bool m_bHasDepthStencil;
	bool m_bIsRenderTarget;

	LPDIRECT3DTEXTURE9 m_texture;
	LPDIRECT3DSURFACE9 m_surfaceLocked;
	LPDIRECT3DSURFACE9 m_surfaceDepthBuffer;

};

#endif