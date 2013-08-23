#ifndef __TEXTURE_MANAGER_H_INCLUDED__
#define __TEXTURE_MANAGER_H_INCLUDED__

#include <string>
#include <vector>

#include "TextureEntry.h"


class TextureManager
{
public:
	TextureManager();
	~TextureManager();
	void release();
	int allocTexture(const char *name, int xSize, int ySize, 
				  bool renderTarget = false, bool createDepthStencil = false); //make a new texture
	int cacheTexture(const char *filename, const char *name); //cache a texture
	int findTexture(const char *name); //find a texture by name
	void freeTexture(int handle);
	size_t getSize(){return m_listTextureCache.size();}
	std::vector<TextureEntry *> m_listTextureCache;

private:
	int m_arraySelectedTextures[8]; // We will support 8 selected textures at once.
};

#endif
