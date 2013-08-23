#ifndef __RENDERER_H_INCLUDED__

#define __RENDERER_H_INCLUDED__

#define DEGTORAD(degree) ((D3DX_PI / 180.0f)*(degree)) //convert from degrees to radians

#include <d3d9.h>
#include <d3dx9.h>
#include "TextureManager.h"
#include <assert.h>
#include "Vector.h"

#define COORDOFFSETINV 8.0

struct RenderVertexTL
{
	float x, y, z;
	float oow;
	unsigned argb;
	float u;
	float v;
	static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
};

struct BillboardSprite
{
public:
	BillboardSprite();
	void SetX(float x,bool square = false);
	void SetY(float y,bool square = false);
	void SetZ(float z);
	void SetWidth(float width);
	void SetHeight(float height);
	void SetAspectRatio(float width, float height);
	void SetU(float u);
	void SetV(float v);
	void SetUWidth(float uwidth);
	void SetVWidth(float vwidth);
	void ApplyTextureCoords();
	const void* GetVertexList();
	const void* GetIndexList();
	void SetOffset(int off);
	int GetFirstVertex()
	{
		return 0;
	}
	int GetNumVertices()
	{
		return 4;
	}
	int GetNumIndices()
	{
		return 6;
	}
	int GetNumPrimitives()
	{
		return 2;
	}
	static int GetNumIndicesStatic()
	{
		return 6;
	}
	static int GetNumVerticesStatic()
	{
		return 4;
	}
private:
	RenderVertexTL VertexList[4];
	float m_fHeight;
	float m_fWidth;
	float m_fNewHeight, m_fNewWidth, m_fDisplacementHeight, m_fDisplacementWidth;
	float m_fSpriteWidth;
	float m_fSpriteHeight;
	float m_fSpriteX;
	float m_fSpriteY;
	float m_fU;
	float m_fV;
	float m_fUWidth;
	float m_fVWidth;
	int m_nOffset;
	//Since indexed draw call fails (need to fix that), store a member variable in sprite to use as an output when dumping shit to vertex buffer.
	RenderVertexTL OutVertList[4];
	int OutIndList[6];

};


enum TransformState
{
	World,
	View,
	Projection
};

class Renderer
{
public:
	Renderer();
	~Renderer();
	HRESULT Initiate(HWND hwnd, int width, int height, bool fullscreen, bool vsync, unsigned int refRate);
	void Release(); //releases direct3d9 device and interfaces.

	void FillColor(D3DCOLOR color); //fill render target with color.
	void SelectTexture(int handle, int stage = 0);
	void DrawTextureOverScreen();

	void RenderSprite(float x, float y, float width, float height);
	void RenderSpriteSheetIndex(float x, float y, float width, float height, int index = -1); //Adds vertex information to vertex buffer.
	void RenderFlush(bool render = true); //Renders data in vertex buffer.
	void RenderTileByIndex(unsigned int tileIndex, unsigned int spriteIndex); //renders a sprite index to a tile position on screen.
	void RenderTileByIndex(unsigned int tileIndex, float xTileOffset, float yTileOffset, unsigned int spriteIndex); //renders an offset tile to screen (smooth scrolling feature)
	void RenderTileByXY(unsigned long x, unsigned long y, unsigned int spriteIndex); //renders a sprite index to a tile position on screen.
	void RenderTileByXY(unsigned int x, unsigned int y, float xTileOffset, float yTileOffset, unsigned int spriteIndex); //renders an offset tile to screen (smooth scrolling feature)

	void FlipPages(); //presents backbuffer to screen.
	IDirect3D9* m_pD3D; //Direct3d 9 interface
	IDirect3DDevice9* m_pD3DDevice; //direct3d 9 device
	HWND GetWindowHandle(){return m_hWnd;} // return the window handle.
	void SetRenderTarget(int handle = -1, int stage = 0); //select a texture as rendering target.

	void SetRenderState(D3DRENDERSTATETYPE state, unsigned value);
	void BeginScene();
	void EndScene();

	//translated coordinates for rendering an indexed sprite sheet's elements.
	static float GetCoordOffset();
	static float GetCoordOffsetInv();
	static float GetXCoord(unsigned short index);
	static float GetYCoord(unsigned short index);

	void SetTransform(TransformState transform, D3DXMATRIX *matTransform);
	void GetTransform(TransformState transform, D3DXMATRIX *matTransform);

	int GetWidth();
	int GetHeight();

	bool m_bRenderToBackBuffer;

	unsigned long int GetNumTrianglesRendered();

	//so that we do not use -1 as magic number.
	static const int BACKBUFFERTARGET = -1;


private:
	bool m_bIsReleased; //tells whether or not the Renderer needs to be released on destruction.
	HWND m_hWnd; //handle to current window.
	int m_arrayCurrentRenderTargets[4]; //Support up to 4 render targets.
	
	LPDIRECT3DSURFACE9 m_pOriginalBackBuffer; // keep the original back buffer surface.
	LPDIRECT3DSURFACE9 m_pOriginalDepthStencil; // keep the original depth stencil surface.

	//variables for render states
	bool m_bDepthBufferRead;
	bool m_bDepthBufferWrite;
	bool m_bBlendEnable;
	float m_fConstantOpacity;
	bool m_bZEnable;
	bool m_bFogEnable;
	unsigned long int m_uNumTriangles;

	void restoreRenderStates();
	int m_nWidth;
	int m_nHeight;
	int m_nTargetWidth;
	int m_nTargetHeight;
	IDirect3DVertexBuffer9 *m_listVertexBuffer;
	IDirect3DIndexBuffer9 *m_listIndexBuffer;
	unsigned int m_nLengthVertexBuffer;
	unsigned int m_nLengthIndexBuffer;
	unsigned int m_nCurrentSprite;



};

extern TextureManager *gTextureManager;
extern Renderer *gRenderer;

#endif