#include <d3d9.h>
#include <assert.h>
#include "TextureManager.h"
#include "Renderer.h"
#include "Common.h"

TextureManager::TextureManager()
{
	m_listTextureCache = std::vector<TextureEntry *>();
}

TextureManager::~TextureManager()
{
	release();
}

void TextureManager::release()
{
	for (unsigned int i = 0; i < m_listTextureCache.size(); i++)
	{
		freeTexture((int)i);
	}
}

int TextureManager::allocTexture(const char *name, int xSize, int ySize, 
				  bool renderTarget, bool createDepthStencil)
{
	TextureEntry *t;

	bool isManaged = !renderTarget;

	assert(NULL != gRenderer->m_pD3DDevice);

	const DWORD kMaxTextureSz = 8192;
	assert(xSize > 0);
	assert(xSize<=kMaxTextureSz);
	assert((xSize & (xSize-1)) == 0); // make sure it's a power of two
	assert(ySize > 0);
	assert(ySize <= kMaxTextureSz);
	assert((ySize & (ySize-1)) == 0); // make sure it's a power of two.

	//does it have a name?
	int slot = -1;
	if((name!=NULL) && (name[0] != '\0'))
		slot = findTexture(name);
	//if a texture with that name has been found already.
	if(slot != -1)
	{
		t = m_listTextureCache[slot];
		if(t->m_texture && (t->m_xSize == (UINT)xSize) && (t->m_ySize == (UINT)ySize))
		{
			t = NULL;
			return slot;
		}
	}

	for(int i = 1; i < (int)m_listTextureCache.size(); ++i)
	{
		//if we find an empty spot
		if(m_listTextureCache[i] == NULL)
		{
			//then we can fill er up
			slot = i;
			break;
		}
	}

	if(slot == -1)
	{
		m_listTextureCache.resize(m_listTextureCache.size() + 1, NULL);
		slot = (int)m_listTextureCache.size() - 1;
	}

	freeTexture(slot);

	t = m_listTextureCache[slot] = new TextureEntry(isManaged);
	
	if (name != NULL)
		t->m_strName = name;
	t->m_xSize = xSize;
	t->m_ySize = ySize;
	t->m_surfaceLocked = NULL;

	// compute usage
	DWORD usage = D3DUSAGE_AUTOGENMIPMAP;
	D3DPOOL pool = D3DPOOL_MANAGED;
	UINT levels = 0;

	if(renderTarget)
	{
		usage = usage | D3DUSAGE_RENDERTARGET;
		pool = D3DPOOL_DEFAULT;
		levels = 1;
		t->m_bIsRenderTarget = true;
	}

	// Allocate D3D texture
	HRESULT result = gRenderer->m_pD3DDevice->CreateTexture(
		xSize,			// width
		ySize,			// height
		levels,			// number of miplevels - zero for automatic mip-map generation
		usage,	// usage - autogen mipmaps and maybe rendertarget
		D3DFMT_A8R8G8B8,	// pixel format - we will only use 32-bit
		pool,	// memory pool - we'll let D3D manage the textures so we don't have to "restore."
		&t->m_texture,		// return pointer to texture interface object here
		NULL // this parameter must be NULL
	);

	// Check for failure.  We won't handle errors gracefully.

	if (FAILED(result)) {
		ABORT("Can't allocate %dx%d 32-bit texture", xSize, ySize);
	}

	if(createDepthStencil)
	{
		t->m_bHasDepthStencil = true;
		D3DFORMAT formats[] = {D3DFMT_D32, D3DFMT_D16};
		for (int a = 0; a < 2; a++)
		{  
			result = 
			gRenderer->m_pD3DDevice->CreateDepthStencilSurface(
				xSize,
				ySize,
				formats[a],
				D3DMULTISAMPLE_NONE,
				NULL,
				true,
				&t->m_surfaceDepthBuffer,
				NULL
			);

			// leave if a buffer was successfully created
			if (SUCCEEDED(result)) break;
		}
		if (FAILED(result)) // if a depth buffer could not be created
		{
			ABORT("Can't create a depth buffer", xSize, ySize);
		}
	}

	//return the index of the texture entry.

	return slot;


}

//returns index of texture entry.
//params: filename is filename of image cached, name is name in texture manager.
int TextureManager::cacheTexture(const char *filename, const char* name)
{
	TextureEntry *t;

	//does it have a name?
	int slot = -1;
	if(m_listTextureCache.size())
	{
		if((name!=NULL) && (name[0] != '\0'))
			slot = findTexture(name);
	}
	//if a texture with that name has been found already.
	if(slot != -1)
	{
		t = m_listTextureCache[slot];
		if(t->m_texture)
		{
			t = NULL;
			return slot;
		}
	}

	for(int i = 1; i < (int)m_listTextureCache.size(); ++i)
	{
		//if we find an empty spot
		if(m_listTextureCache[i] == NULL)
		{
			//then we can fill er up
			slot = i;
			break;
		}
	}

	if(slot == -1)
	{
		m_listTextureCache.resize(m_listTextureCache.size() + 1, NULL);
		slot = (int)m_listTextureCache.size() - 1;
	}
	// now that we have a slot number, we should do something with it
	
	//make sure we clear out any residual data in the slot.
	freeTexture(slot);

	//no loading of texture from file as of yet.
	t = m_listTextureCache[slot] = new TextureEntry(true);

	if (name != NULL)
		t->m_strName = name;
	t->m_xSize = 0;
	t->m_ySize = 0;
	t->m_surfaceLocked = NULL;

	// compute usage
	DWORD usage = D3DUSAGE_AUTOGENMIPMAP;
	D3DPOOL pool = D3DPOOL_MANAGED;

	D3DXIMAGE_INFO information;

	// Create D3D texture from file :-)
	HRESULT result = D3DXCreateTextureFromFileEx(
		gRenderer->m_pD3DDevice,
		filename,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_FROM_FILE,
		usage,
		D3DFMT_A8R8G8B8,
		pool,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0x00000000,
		&information,
		NULL,
		&t->m_texture
	);
	t->m_xSize = information.Width;
	t->m_ySize = information.Height;

	// Check for failure.  We won't handle errors gracefully.

	if (FAILED(result)) {
		ABORT("Can't allocate %dx%d 32-bit texture", t->m_xSize, t->m_ySize);
	}

	return slot;
}


int TextureManager::findTexture(const char *name)
{
	int slot = -1;
	for(int i = 0; i < (int)m_listTextureCache.size(); ++i)
	{
		if(!strcmp(m_listTextureCache[i]->m_strName.c_str(), name))
		{
			return i;
		}
	}
	return slot;
}

void TextureManager::freeTexture(int handle)
{
	// Sanity check handle validity
	if ((handle < 0) || (handle >= (int)m_listTextureCache.size()))
	{

		//Let's call a crash.
		assert(false);
		return;
	}
	//TODO: Implement error checking in case the handle is the currently selected texture.
	
	TextureEntry *tr = m_listTextureCache[handle];
	if(tr)
	{
		tr->Release();
		delete tr;
		tr = m_listTextureCache[handle] = NULL;
	}
	return;
}

