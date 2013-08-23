#include "TextureEntry.h"
#include "Renderer.h"
#include "Common.h"

TextureEntry::TextureEntry(bool isManaged)
{
	m_bIsManaged = isManaged;
	m_strName = "";
	m_xSize = m_ySize = 0;
	m_bHasDepthStencil = false;
	m_bIsRenderTarget = false;
	m_texture = NULL;
	m_surfaceLocked = NULL;
	m_surfaceDepthBuffer = NULL;
}


TextureEntry::~TextureEntry()
{
	Release();
}


void TextureEntry::Release()
{
	//Clean up
	if(m_texture)
		m_texture->Release();
	if(m_surfaceLocked)
		m_surfaceLocked->Release();
	if(m_surfaceDepthBuffer)
		m_surfaceDepthBuffer->Release();
	m_bHasDepthStencil = false;
	m_bIsRenderTarget = false;
	m_texture = NULL;
	m_surfaceLocked = NULL;
	m_surfaceDepthBuffer = NULL;


}

void TextureEntry::Restore()
{
		// compute usage
	DWORD usage = D3DUSAGE_AUTOGENMIPMAP;
	D3DPOOL pool = D3DPOOL_MANAGED;
	
	UINT levels = 0;

	if (m_bIsRenderTarget) 
	{
		usage = usage | D3DUSAGE_RENDERTARGET;
		pool = D3DPOOL_DEFAULT;
		levels = 1;   
	}

	// Allocate D3D texture
	HRESULT result = gRenderer->m_pD3DDevice->CreateTexture(
		m_xSize,			// width
		m_ySize,			// height
		levels,			// number of miplevels - zero for automatic mip-map generation
		usage,	// usage - autogen mipmaps and maybe rendertarget
		D3DFMT_A8R8G8B8,	// pixel format - we will only use 32-bit
		pool,	// memory pool - we'll let D3D manage the textures so we don't have to "restore."
		&m_texture,		// return pointer to texture interface object here
		NULL // this parameter must be NULL
	);

	// Check for failure.  We won't handle errors gracefully.

	if (FAILED(result)) {
		ABORT("Can't allocate %dx%d 32-bit texture", m_xSize, m_ySize);
	}



  // Create a depth stencil if requested
  if (m_bHasDepthStencil)
  {
    // acceptable formats in order of preference
    D3DFORMAT formats[] = {D3DFMT_D32, D3DFMT_D16};

    for (int a = 0; a < 2; a++)
    {  
      result = 
      gRenderer->m_pD3DDevice->CreateDepthStencilSurface(
        m_xSize,
        m_ySize,
        formats[1],
        D3DMULTISAMPLE_NONE,
        NULL,
        true,
        &m_surfaceDepthBuffer,
        NULL
        );

      // leave if a buffer was successfully created
      if (SUCCEEDED(result)) break;
      
    }
    if (FAILED(result)) // if a depth buffer could not be created
    {
      ABORT("Can't create a depth buffer", m_xSize, m_ySize);
    }
  }


}