#include "Renderer.h"
#include "Common.h"


Renderer *gRenderer = NULL;
TextureManager *gTextureManager = NULL;

Renderer::Renderer()
{
	m_bIsReleased = false;
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
}

Renderer::~Renderer()
{

}

HRESULT Renderer::Initiate(HWND hwnd, int width, int height, bool fullscreen, bool vsync, unsigned int refRate)
{
	m_hWnd = hwnd;
	// create the IDirect3D9 object
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == NULL)
		return E_FAIL;
	
	// get the display mode
	D3DDISPLAYMODE d3ddm;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	// set the presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = !fullscreen;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.FullScreen_RefreshRateInHz = refRate;
	if(vsync)
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	// create the device
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_FPU_PRESERVE | D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_pD3DDevice)))
	{
		//Fallback to software rendering.
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_REF, hwnd,
			D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &m_pD3DDevice)))
		{
			return E_FAIL;
		}
	}

	m_pD3DDevice->GetRenderTarget(0, &m_pOriginalBackBuffer);
	m_pD3DDevice->GetDepthStencilSurface(&m_pOriginalDepthStencil);

	//Set default render attributes.
	m_bDepthBufferRead = true;
	m_bDepthBufferWrite = true;
	m_bBlendEnable = true;
	m_fConstantOpacity = 1.0f;
	m_bZEnable = true;
	m_bFogEnable = false;
	m_nWidth = width;
	m_nHeight = height;
	m_nTargetHeight = height;
	m_nTargetWidth = width;
	m_bRenderToBackBuffer = true;

	restoreRenderStates();

	gTextureManager = new TextureManager();

	m_listVertexBuffer = NULL;
	m_listIndexBuffer = NULL;

	int vert_count;
	int byte_count;
	int ind_count;
	HRESULT hres;
	int sprite_count = 3 * (int)GetCoordOffsetInv() * (int)GetCoordOffsetInv();
	vert_count=6;//BillboardSprite::GetNumVerticesStatic();
	byte_count = vert_count*sizeof(RenderVertexTL) * sprite_count;
	m_nLengthVertexBuffer = vert_count * sprite_count;
	hres = m_pD3DDevice->CreateVertexBuffer(	
												byte_count, //length
												D3DUSAGE_WRITEONLY, //usage flags
												RenderVertexTL::FVF, //FVF
												D3DPOOL_MANAGED, //Pool
												&m_listVertexBuffer, //vertex buffer to be used.
												NULL //should be null?
											);

	if(FAILED(hres))
	{
		switch(hres)
		{
		case D3DERR_INVALIDCALL:
			DEBUGPRINTF("Invalid call in CreateVertexBuffer\n");
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			DEBUGPRINTF("Out of video memory for CreateVertexBuffer\n");
			break;
		case E_OUTOFMEMORY:
			DEBUGPRINTF("Out of memory for CreateVertexBuffer\n");
			break;
		}
		DEBUGPRINTF("%d bytes into vertex buffer fails\n", hres);
		return E_FAIL;
	}
	ind_count=BillboardSprite::GetNumIndicesStatic();
	byte_count = ind_count*sizeof(int) * sprite_count;
	m_nLengthIndexBuffer = ind_count * sprite_count;
	hres = m_pD3DDevice->CreateIndexBuffer(	
												byte_count, //length
												D3DUSAGE_WRITEONLY, //usage flags
												D3DFMT_INDEX16, //format of indices
												D3DPOOL_MANAGED, //Pool
												&m_listIndexBuffer, //vertex buffer to be used.
												NULL //should be null?
											);

	if(FAILED(hres))
	{
		switch(hres)
		{
		case D3DERR_INVALIDCALL:
			DEBUGPRINTF("Invalid call in CreateVertexBuffer\n");
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			DEBUGPRINTF("Out of video memory for CreateVertexBuffer\n");
			break;
		case E_OUTOFMEMORY:
			DEBUGPRINTF("Out of memory for CreateVertexBuffer\n");
			break;
		}
		DEBUGPRINTF("%d bytes into vertex buffer fails\n", hres);
		return E_FAIL;
	}

	DEBUGPRINTF("Size of input: %d\n", sizeof(RenderVertexTL[4]));
	m_nCurrentSprite = 0;
	m_uNumTriangles = 0;
	return S_OK;
}

unsigned long int Renderer::GetNumTrianglesRendered()
{
	unsigned long int retValue = m_uNumTriangles;
	m_uNumTriangles = 0;
	return retValue;

}

//Restore the default render states
void Renderer::restoreRenderStates()
{
	SetRenderState(D3DRS_ALPHABLENDENABLE, m_bBlendEnable);
	SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void Renderer::BeginScene()
{
	m_pD3DDevice->BeginScene();
}

void Renderer::EndScene()
{
	m_pD3DDevice->EndScene();
}

void Renderer::SetRenderState(D3DRENDERSTATETYPE state, unsigned value)
{
	// Make sure device exists
	if (m_pD3DDevice == NULL) 
	{
		assert(false);
		return;
	}

	// Set the state
	HRESULT result = m_pD3DDevice->SetRenderState(state, value);

	// Check for error
	assert(SUCCEEDED(result));
}

void Renderer::FillColor(D3DCOLOR color)
{
	// clear the back buffer
	if(m_pD3DDevice == NULL)
	{
		assert(false);
		return;
	}
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		color, 1.0f, 0);

/*	// begin rendering
	m_pD3DDevice->BeginScene();

	// all of your rendering would go here

	// end rendering
	m_pD3DDevice->EndScene();
*/	

}

void Renderer::SelectTexture(int handle, int stage)
{
	if (handle < 0 || handle >= (int)gTextureManager->getSize()) 
	{  

		if (stage >= 0 && stage < 8)
			m_pD3DDevice->SetTexture(stage, NULL);

			return;
	}
	TextureEntry *t = gTextureManager->m_listTextureCache[handle];
	if(t->m_texture == NULL)
	{
		assert(false);
		return;
	}

	//Select in D3D
	assert(m_pD3DDevice != NULL);
	HRESULT result = m_pD3DDevice->SetTexture((DWORD)stage, t->m_texture);
	assert(SUCCEEDED(result));

	return;
}

void Renderer::DrawTextureOverScreen()
{
	return;
}

void Renderer::RenderSprite(float x, float y, float width, float height)
{
	RenderSpriteSheetIndex(x,y,width,height, -1);
}

void Renderer::RenderSpriteSheetIndex(float x, float y, float width, float height, int index)
{
	/*if(x < 0.0f || x > 1.0f)
		return;
	if(y < 0.0f || y > 1.0f)
		return;
	*/
	HRESULT result;


	//we will want to assume that we have a 16x16 sprite sheet for now.
	assert(index >= -1 && index < 256);
	
	//make new temp U,V coords.
	float newUMin, newVMin, newUMax, newVMax;
	newUMin = newVMin = 0.0f; 
	newUMax = newVMax = 1.0f;
	if(index != -1)
	{
		//since individual sprites are on 16x16
		unsigned int xCoord = 0;
		unsigned int yCoord = 0;

		xCoord = index % 16;
		yCoord = index / 16;

		float offset = 1.0f / 16.0f;
		newUMin = (float)xCoord * offset;
		newUMax = newUMin + offset;
		newVMin = (float)yCoord * offset;
		newVMax = newVMin + offset;
	}

	BillboardSprite sprite = BillboardSprite();
	// Make sure device exists
	if(m_pD3DDevice == NULL)
	{
		assert(false);
		return;
	}
	//Yeah, now we set stream source to be the vertex buffer we just dumped into.


	//set FVF.
	result = m_pD3DDevice->SetFVF(RenderVertexTL::FVF);
	assert(SUCCEEDED(result));

	sprite.SetAspectRatio((float)m_nTargetWidth,(float)m_nTargetHeight);
	sprite.SetWidth(width);
	sprite.SetHeight(height);
	if(m_bRenderToBackBuffer)
	{
		sprite.SetX(x,false);
		sprite.SetY(y,false);
	}
	else
	{
		sprite.SetX(x,true);
		sprite.SetY(y,true);
	}
	sprite.SetZ(1.0f);
	sprite.SetU(newUMin);
	sprite.SetV(newVMin);
	sprite.SetUWidth(newUMax-newUMin);
	sprite.SetVWidth(newVMax-newVMin);
	sprite.ApplyTextureCoords();

#if 0
	result = m_pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		sprite.GetFirstVertex(),
		sprite.GetNumVertices(),
		sprite.GetNumPrimitives(),
		triList,
		D3DFMT_INDEX16,
		sprite.GetVertexList(),
		sizeof(RenderVertexTL)
	);
#endif
	//Let's fill the vertex buffer. (yay?)
	
	void *vb_vertices;

	//assert(localSpriteCount == m_nCurrentSprite);
	int localSpriteCount = m_nCurrentSprite++;
	//	if(spriteCount != -1)
	//		localSpriteCount = spriteCount;


	//Make sure there's still room in vertex buffer
	bool EnoughRoomInVertexBuffer = (localSpriteCount * BillboardSprite::GetNumVerticesStatic()) < (int)m_nLengthVertexBuffer;
	if(!EnoughRoomInVertexBuffer)
	{
		DEBUGPRINTF("Attempted to write %d to a %d length vertex buffer.\nForgetting to flush our buffer, are we?\n", localSpriteCount * BillboardSprite::GetNumVerticesStatic(), m_nLengthVertexBuffer);
		assert(EnoughRoomInVertexBuffer);
		return;
	}

	result = m_listVertexBuffer->Lock(localSpriteCount * sprite.GetNumIndices()/*sprite.GetNumVertices()*/ * sizeof(RenderVertexTL),	//Offset
								/*sprite.GetNumVertices()*/sprite.GetNumIndices() * sizeof(RenderVertexTL),            //SizeToLock
								&vb_vertices,					//Vertices
								D3DLOCK_DISCARD);				//Flags
	
	assert(SUCCEEDED(result));

	//vb_vertices now points to an array of vertices. To fill the Vertex Buffer with our data, we'll use memcpy from the standard C library.

	RenderVertexTL *tmp = (RenderVertexTL *)sprite.GetVertexList();
	int *a = (int *)sprite.GetIndexList();
	RenderVertexTL tempSprite[6] = {tmp[a[0]],tmp[a[1]],tmp[a[2]],tmp[a[3]],tmp[a[4]],tmp[a[5]]};

	memcpy	(
				vb_vertices, //Destination
				(const void *)tempSprite,
				//sprite.GetVertexList(),       //Source
				sprite.GetNumIndices() * sizeof(RenderVertexTL)
				//sprite.GetNumVertices() * sizeof(RenderVertexTL)
			); //Amount of data to copy

	//After we've copied our data into the buffer, we can unlock the buffer so it can be used for rendering. The Unlock call takes no parameters.

	m_listVertexBuffer->Unlock();

#if 0 //This section is disabled due to lack of proper index buffer support.
	void *ib_indices;

	//Make sure there's still room in index buffer
	bool EnoughRoomInIndexBuffer = (localSpriteCount * BillboardSprite::GetNumIndicesStatic()) < (int)m_nLengthIndexBuffer;
	if(!EnoughRoomInIndexBuffer)
	{
		DEBUGPRINTF("Attempted to write %d to a %d length vertex buffer.\nForgetting to flush our buffer, are we?\n", localSpriteCount * BillboardSprite::GetNumIndicesStatic(), m_nLengthIndexBuffer);
		assert(EnoughRoomInIndexBuffer);
		return;
	}

	result = m_listIndexBuffer->Lock(localSpriteCount * sprite.GetNumIndices() * sizeof(int),	//Offset
								sprite.GetNumIndices() * sizeof(int),            //SizeToLock
								&ib_indices,					//Vertices
								D3DLOCK_DISCARD);				//Flags
	
	assert(SUCCEEDED(result));

	//vb_vertices now points to an array of vertices. To fill the Vertex Buffer with our data, we'll use memcpy from the standard C library.

	memcpy	(
				ib_indices, //Destination
				sprite.GetIndexList(),       //Source
				sprite.GetNumIndices() * sizeof(int)
			); //Amount of data to copy

	//After we've copied our data into the buffer, we can unlock the buffer so it can be used for rendering. The Unlock call takes no parameters.

	m_listIndexBuffer->Unlock();
#endif
}

void Renderer::RenderFlush(bool render)
{
	if(render)
	{
		HRESULT result = m_pD3DDevice->SetStreamSource(
									0,						//Stream number.
									m_listVertexBuffer,		//stream data.
									0,						//offset in bytes.
									sizeof(RenderVertexTL)	//stride.
							);
		assert(SUCCEEDED(result));
//		result = m_pD3DDevice->SetIndices(m_listIndexBuffer);
		assert(SUCCEEDED(result));
		m_pD3DDevice->SetFVF(RenderVertexTL::FVF);
		
	/*	HRESULT DrawIndexedPrimitiveUP(
			D3DPRIMITIVETYPE PrimitiveType,
			UINT MinVertexIndex,
			UINT NumVertices,
			UINT PrimitiveCount,
			CONST void * pIndexData,
			D3DFORMAT IndexDataFormat,
			CONST void* pVertexStreamZeroData,
			UINT VertexStreamZeroStride
		);
		HRESULT DrawIndexedPrimitive(
			D3DPRIMITIVETYPE Type,
			INT BaseVertexIndex,
			UINT MinIndex,
			UINT NumVertices,
			UINT StartIndex,
			UINT PrimitiveCount
		);
		*/
#if 1
		result = m_pD3DDevice->DrawPrimitive(	D3DPT_TRIANGLELIST,
												0,
												2 * m_nCurrentSprite
									);
#endif
#if 0
		result = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
													0,
													0,
													m_nCurrentSprite,
													0,
													2*m_nCurrentSprite);
#endif
		assert(SUCCEEDED(result));

		m_uNumTriangles += (unsigned long int)(2 * m_nCurrentSprite);

	}
	m_nCurrentSprite = 0;
}

void Renderer::RenderTileByIndex(unsigned int tileIndex, unsigned int spriteIndex)
{
	//I will go ahead and assume that tiles will wrap (no error case if index out of bounds).
	tileIndex %= (int)(GetCoordOffsetInv() * GetCoordOffsetInv());
	spriteIndex %= 256;

	float x = GetXCoord((unsigned short)tileIndex);
	float y = GetYCoord((unsigned short)tileIndex);
	float length = GetCoordOffset();

	RenderSpriteSheetIndex(x, y, length, length, spriteIndex);
}

void Renderer::RenderTileByIndex(unsigned int tileIndex, float xTileOffset, float yTileOffset, unsigned int spriteIndex)
{
	if(yTileOffset < -GetCoordOffset() || yTileOffset > 1.0f || xTileOffset < -GetCoordOffset() || xTileOffset > 1.0f)
		return;
	//I will go ahead and assume that tiles will wrap (no error case if index out of bounds).
	tileIndex %= (int)(GetCoordOffsetInv() * GetCoordOffsetInv());
	spriteIndex %= 256;

	float x = GetXCoord((unsigned short)tileIndex);
	float y = GetYCoord((unsigned short)tileIndex);
	float length = GetCoordOffset();

	RenderSpriteSheetIndex(x + xTileOffset, y + yTileOffset, length, length, spriteIndex);
}

void Renderer::RenderTileByXY(unsigned long x, unsigned long y, unsigned int spriteIndex)
{
	
	float fx = (float)x * GetCoordOffset();
	float fy = (float)y * GetCoordOffset();
	float length = GetCoordOffset();

	RenderSpriteSheetIndex(fx, fy, length, length, spriteIndex);
}

void Renderer::RenderTileByXY(unsigned int x, unsigned int y, float xTileOffset, float yTileOffset, unsigned int spriteIndex)
{

	float fx = (float)x * GetCoordOffset();
	float fy = (float)y * GetCoordOffset();
	float length = GetCoordOffset();

	RenderSpriteSheetIndex(fx + xTileOffset, fy + yTileOffset, length, length, spriteIndex);
}


void Renderer::FlipPages()
{
	// present the back buffer to the screen
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void Renderer::SetRenderTarget(int handle, int stage)
{
	if(handle == -1)
	{
		m_bRenderToBackBuffer = true;
		m_nTargetHeight = m_nHeight;
		m_nTargetWidth = m_nWidth;
		m_arrayCurrentRenderTargets[stage] = handle;
		//A call with -1 as handle resets the target stage to the backbuffer's render target.
		m_pD3DDevice->SetRenderTarget((DWORD)stage, m_pOriginalBackBuffer);      
		m_pD3DDevice->SetDepthStencilSurface(m_pOriginalDepthStencil);
		return;
	}
	m_bRenderToBackBuffer = false;
	//Otherwise, assert handle is a valid number.
	size_t numTextures = gTextureManager->getSize();
	if(handle < 0 || handle >= (int)numTextures)
	{
		//Preposterous! Why would you do this!?
		assert(handle < 0 || handle >= (int)numTextures);
		DEBUGPRINTF("Not a valid handle: %d \n", handle);
		return;

	}
	//Now we want a texture element from the texturemanager.
	TextureEntry* renderTarget = gTextureManager->m_listTextureCache[handle];
	//Got it.
	if(renderTarget == NULL)
		return;
	//if there's not a surface, get one.
	if(renderTarget->m_surfaceLocked == NULL)
	{
		//I'm helping!
		renderTarget->m_texture->GetSurfaceLevel(0,&renderTarget->m_surfaceLocked);
	}
	//if we have a depth stencil, set it.
	if(renderTarget->m_bHasDepthStencil)
		m_pD3DDevice->SetDepthStencilSurface(renderTarget->m_surfaceDepthBuffer);
	else
		m_pD3DDevice->SetDepthStencilSurface(m_pOriginalDepthStencil);

	//Now, since we've got error checking out of the way, set the render target to the texture.

	HRESULT hr = m_pD3DDevice->SetRenderTarget(stage, renderTarget->m_surfaceLocked);
	if(SUCCEEDED(hr))
	{
		//we're fine.
		m_nTargetHeight = renderTarget->m_ySize;
		m_nTargetWidth = renderTarget->m_xSize;
		m_arrayCurrentRenderTargets[stage] = handle;
		return;
	}
	else
	{
		assert(SUCCEEDED(hr));
		
		DEBUGPRINTF("Failed to set texture: %s as a render target.\n", renderTarget->m_strName.c_str());
		return;
	}
}

void Renderer::Release()
{
	if(m_listVertexBuffer != NULL)
	{
		m_listVertexBuffer->Release();
		m_listVertexBuffer = NULL;
	}
	if(m_listIndexBuffer != NULL)
	{
		m_listIndexBuffer->Release();
		m_listIndexBuffer = NULL;
	}
	if (gTextureManager)
	{
		gTextureManager->release();
		delete gTextureManager;
		gTextureManager = NULL;
	}
	if (m_pD3DDevice)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	if (m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
	m_bIsReleased = true;
}

float Renderer::GetCoordOffsetInv()
{
	return COORDOFFSETINV;
}

float Renderer::GetCoordOffset()
{
	return 1.0f / GetCoordOffsetInv();
}


float Renderer::GetXCoord(unsigned short index)
{
	unsigned short xCoord = index % (unsigned short)GetCoordOffsetInv();
	return (float)xCoord * GetCoordOffset();
}

float Renderer::GetYCoord(unsigned short index)
{
	unsigned short yCoord = index / (unsigned short)GetCoordOffsetInv();
	return (float)yCoord * GetCoordOffset();
}

void Renderer::GetTransform(TransformState transform, D3DXMATRIX *matTransform)
{
	switch (transform)
	{
	case World:
		m_pD3DDevice->GetTransform(D3DTS_WORLD, matTransform);
		break;
	case View:
		m_pD3DDevice->GetTransform(D3DTS_VIEW, matTransform);
		break;
	case Projection:
		m_pD3DDevice->GetTransform(D3DTS_PROJECTION, matTransform);
		break;
	}
	return;
}
void Renderer::SetTransform(TransformState transform, D3DXMATRIX *matTransform)
{
	switch (transform)
	{
	case World:
		m_pD3DDevice->SetTransform(D3DTS_WORLD, matTransform);
		break;
	case View:
		m_pD3DDevice->SetTransform(D3DTS_VIEW, matTransform);
		break;
	case Projection:
		m_pD3DDevice->SetTransform(D3DTS_PROJECTION, matTransform);
		break;
	}
	return;
}
int Renderer::GetWidth()
{
	return m_nWidth;
}
int Renderer::GetHeight()
{
	return m_nHeight;
}
BillboardSprite::BillboardSprite()
{
		VertexList[0].x = 0.0f;
		VertexList[0].y = 0.0f;
		VertexList[0].z = 0.0f;
		VertexList[0].argb = 0xffffffff;
		VertexList[0].oow = 1.0f;
	
		VertexList[1].x = 0.0f;
		VertexList[1].y = 0.0f;
		VertexList[1].z = 0.0f;
		VertexList[1].argb = 0xffffffff;
		VertexList[1].oow = 1.0f;
	
		VertexList[2].x = 0.0f;
		VertexList[2].y = 0.0f;
		VertexList[2].z = 0.0f;
		VertexList[2].argb = 0xffffffff;
		VertexList[2].oow = 1.0f;	
		
		VertexList[3].x = 0.0f;
		VertexList[3].y = 0.0f;
		VertexList[3].z = 0.0f;
		VertexList[3].argb = 0x00ffffff;
		VertexList[3].oow = 1.0f;
		m_fHeight =
		m_fWidth = 
		m_fNewHeight = 
		m_fNewWidth = 
		m_fDisplacementHeight = 
		m_fDisplacementWidth =
		m_fSpriteWidth =
		m_fSpriteHeight =
		m_fSpriteX =
		m_fSpriteY = 0.0f;
		m_fU = 0.0f;
		m_fV = 0.0f;
		m_fUWidth = 1.0f;
		m_fVWidth = 1.0f;
		m_nOffset = 0;
}

void BillboardSprite::SetAspectRatio(float width, float height)
{
		m_fWidth = width;
		m_fHeight = height;
		m_fNewHeight = m_fWidth / 16.0f * 9.0f;
		m_fDisplacementHeight = (m_fHeight - m_fNewHeight) * 0.5f;

		m_fNewWidth = m_fHeight / 9.0f * 16.0f;
		m_fDisplacementWidth = (m_fWidth - m_fNewWidth) * 0.5f;
}

void BillboardSprite::SetX(float x,bool square)
{
	float tmp;
	m_fSpriteX = x;
	if(!square)
	{
		if(m_fWidth / m_fHeight <= 16.0f / 9.0f)
		{
			tmp = m_fSpriteX * m_fWidth;
			VertexList[0].x = tmp;

			tmp = (m_fSpriteX + m_fSpriteWidth) * m_fWidth;
			VertexList[1].x = tmp;
			VertexList[2].x = tmp;

			tmp = m_fSpriteX * m_fWidth;
			VertexList[3].x = tmp;
		}
		else
		{
			tmp = m_fSpriteX * m_fNewWidth + m_fDisplacementWidth;
			VertexList[0].x = tmp;

			tmp = (m_fSpriteX + m_fSpriteWidth) * m_fNewWidth + m_fDisplacementWidth;
			VertexList[1].x = tmp;
			VertexList[2].x = tmp;

			tmp = m_fSpriteX * m_fNewWidth + m_fDisplacementWidth;
			VertexList[3].x = tmp;

		}
	}
	else
	{
		tmp = m_fSpriteX * m_fWidth;
		VertexList[0].x = tmp;
		tmp = (m_fSpriteX + m_fSpriteWidth) * m_fWidth;
		VertexList[1].x = tmp;
		VertexList[2].x = tmp;
		tmp = m_fSpriteX * m_fWidth;
		VertexList[3].x = tmp;
	}
}
void BillboardSprite::SetY(float y,bool square)
{
	m_fSpriteY = y;
	
	float tmp;
	if(!square)
	{
		if((m_fWidth / m_fHeight) <= (16.0f / 9.0f))
		{
			tmp = m_fSpriteY * m_fNewHeight + m_fDisplacementHeight;

			VertexList[0].y = tmp;
			VertexList[1].y = tmp;

			tmp = (m_fSpriteY + m_fSpriteHeight) * m_fNewHeight + m_fDisplacementHeight;
			VertexList[2].y = tmp;
			VertexList[3].y = tmp;
		}
		else
		{
			tmp = m_fSpriteY * m_fHeight;
			VertexList[0].y = tmp;
			VertexList[1].y = tmp;
		
			tmp = (m_fSpriteY + m_fSpriteHeight) * m_fHeight;
			VertexList[2].y = tmp;
			VertexList[3].y = tmp;
		
		}
	}
	else
	{
		tmp = m_fSpriteY * m_fHeight;
		VertexList[0].y = tmp;
		VertexList[1].y = tmp;
	
		tmp = (m_fSpriteY + m_fSpriteHeight) * m_fHeight;
		VertexList[2].y = tmp;
		VertexList[3].y = tmp;
	}
}

void BillboardSprite::SetZ(float z)
{
	VertexList[0].z = z;
	VertexList[1].z = z;
	VertexList[2].z = z;
	VertexList[3].z = z;
}
void BillboardSprite::SetWidth(float width)
{
	m_fSpriteWidth = width;
}
void BillboardSprite::SetHeight(float height)
{
	m_fSpriteHeight = height;
}
void BillboardSprite::SetU(float u)
{
	m_fU = u;
}
void BillboardSprite::SetV(float v)
{
	m_fV = v;
}
void BillboardSprite::SetUWidth(float uwidth)
{
	m_fUWidth = uwidth;
}
void BillboardSprite::SetVWidth(float vwidth)
{
	m_fVWidth = vwidth;
}

void BillboardSprite::SetOffset(int off)
{
	m_nOffset = off;
}

void BillboardSprite::ApplyTextureCoords()
{
	VertexList[0].u = m_fU;
	VertexList[0].v = m_fV;

	VertexList[1].u = m_fU + m_fUWidth;
	VertexList[1].v = VertexList[0].v;

	VertexList[2].u = VertexList[1].u;
	VertexList[2].v = m_fV + m_fVWidth;

	VertexList[3].u = VertexList[0].u;
	VertexList[3].v = VertexList[2].v;
}

const void* BillboardSprite::GetVertexList()
{

	OutVertList[0] = VertexList[0];
	OutVertList[1] = VertexList[1];
	OutVertList[2] = VertexList[2];
	OutVertList[3] = VertexList[3];

	return (const void *)OutVertList;
}

const void* BillboardSprite::GetIndexList()
{
	OutIndList[0] = 0 + m_nOffset;
	OutIndList[1] = 1 + m_nOffset;
	OutIndList[2] = 2 + m_nOffset;
	OutIndList[3] = 3 + m_nOffset;
	OutIndList[4] = 0 + m_nOffset;
	OutIndList[5] = 2 + m_nOffset;
	return (const void *)OutIndList;
}