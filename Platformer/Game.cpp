#include "Game.h"
#include "Renderer.h"
#include <cstdio>
#include <climits>

Game::Game()
: m_inInputP1(0){
	m_inInputP1 = Input(0);
}

Game::Game(HWND hwnd, int width, int height, bool fullscreen, bool vsync, unsigned int refRate, SettingsManager* settings)
: m_inInputP1(0){
	m_hwnd = hwnd;
	m_nWidth = width;
	m_nHeight = height;
	m_bFullscreen = fullscreen;
	m_bVsync = vsync;
	m_uRefRate = refRate;
	m_rndRand = Random(timeGetTime());
	m_inInputP1 = Input((DWORD)0);
	m_pCurrentState = new XINPUT_STATE();
	m_pSettingsManager = settings;
	m_pObjectManager = new ObjectManager();
	m_fntFont = Font();
	m_dFPS = 0.0;
	m_gameChess = Chess();
}

Game::~Game()
{
	m_pSettingsManager = NULL;
	//Do not call Game->Release() here plox. kthx
}
void Game::SetFPS(double fps)
{
	m_dFPS = fps;
}
double Game::GetAverageFPS()
{
	if((frameCounter % 16) == 15)
		m_dAvgFPS = m_dFPS;
	return m_dAvgFPS;
}

bool Game::Init()
{
	gRenderer = new Renderer();
	

	if( FAILED(gRenderer->Initiate(m_hwnd, m_nWidth, m_nHeight, m_bFullscreen, m_bVsync, m_uRefRate)))
	{
		gRenderer->Release();
		gRenderer = NULL;
		return false;
	}
	m_nH1 = gTextureManager->cacheTexture((const char *)TEXT("chess.png"), "chesstiles");

	m_nBGHandle = gTextureManager->findTexture("chesstiles");

	int m_handleRenderTarget = gTextureManager->allocTexture("renderTarget", 2048, 2048, true, true);
	bool tmp = m_inInputP1.Init(m_pSettingsManager->m_szP1KeyMap, m_pSettingsManager->m_bUseKeyboard);
	if(!tmp)
	{
		DEBUGPRINTF("Initializing input failed.\n");
		return false;
	}
	m_bDisplay = false;
	tmp = m_pSettingsManager->m_bUseKeyboard;
	frameCounter = 0;
	m_pObjectManager->Init();

	tmp = m_fntFont.Init();
	if(!tmp)
	{
		DEBUGPRINTF("Font failed to initialize.\n");
		return false;
	}


	//Do some matrix stuff.
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	//D3DXMatrixTranslation(&matWorld,10.0f, 10.0f, 10.0f);
	D3DXMATRIX matView;
	D3DXMatrixIdentity(&matView);
	D3DXMATRIX matProj;
	D3DXMatrixIdentity(&matProj);
	//D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI/4,16.0f/9.0f,0.1f,5.0f);
	gRenderer->SetTransform(World, &matWorld);
	gRenderer->SetTransform(View, &matView);
	gRenderer->SetTransform(Projection, &matProj);

	selectedTileX = 4;
	selectedTileY = 4;


	m_bPieceSelected = false;

	m_gameChess.Init();

	return true;
}

int distance(int x1, int y1, int x2, int y2)
{
	int retVal;
	int xDisp = abs(x2 - x1); 
	int yDisp = abs(y2 - y1);
	retVal = max(xDisp, yDisp);
	return retVal;
}

bool Game::Process()
{
	/*
	 * Let's do some non-rendering so the user is happy.
	 */

	frameCounter = frameCounter % 256;


	unsigned long sideLength = (unsigned long)gRenderer->GetCoordOffsetInv();

	if(!m_inInputP1.Process())
	{
		//Controller disconnected.
		DEBUGPRINTF("Controller 1 disconnected. Exiting.\n");
		return false;
	}
	if(m_inInputP1.ButtonPressed(A))
	{	
		bool moved = false;
		if(m_bPieceSelected)
		{
			m_gameChess.MovePiece(selectedTileX, selectedTileY);
			m_gameChess.SelectPiece(-1,-1);
			m_bPieceSelected = false;
			moved = true;
		}
		else
			m_bPieceSelected = m_gameChess.SelectPiece(selectedTileX, selectedTileY);
		
	}
	if(m_inInputP1.ButtonPressed(B))
	{
		m_bPieceSelected = m_gameChess.SelectPiece(-1,-1);
	}

	if(m_inInputP1.ButtonPressed(UP))
	{
		selectedTileY--;
		if(selectedTileY < 0)
			selectedTileY = 0;
	}
	if(m_inInputP1.ButtonPressed(DOWN))
	{
		selectedTileY++;
		if((unsigned long)selectedTileY == (int)sideLength)
			selectedTileY = (int)sideLength - 1;
	}
	if(m_inInputP1.ButtonPressed(LEFT))
	{
		selectedTileX--;
		if(selectedTileX < 0)
			selectedTileX = 0;
	}
	if(m_inInputP1.ButtonPressed(RIGHT))
	{
		selectedTileX++;
		if(selectedTileX == (int)sideLength)
			selectedTileX = (int)sideLength - 1;
	}


	if(m_inInputP1.ButtonDown(BACK))
	{
		DEBUGPRINTF("User has decided to quit.\n");
		return false;
	}




	//Let's do objects now that we have input.
	m_pObjectManager->Process();

	/*
	 * Let's get to running the graphics now.
	 */





	gRenderer->BeginScene();

	gRenderer->FillColor(D3DCOLOR_XRGB(0,0,0));

	//Set a texture to be render target.
	int handleRenderTarget = gTextureManager->findTexture("renderTarget");
	gRenderer->SetRenderTarget(handleRenderTarget, 0);

	gRenderer->FillColor(D3DCOLOR_ARGB(0,0,0,0));
	gRenderer->SelectTexture(m_nBGHandle);
	
	//	In order to future proof in the case that we can use openmp to speed up the filling of the vertex buffer, 
	//I have added a spriteCount parameter to Render calls (not including flush). This should optimize
	//vertex buffer filling for multithreaded / multicore systems.


	unsigned long i, j;
	for(i = 0; i < sideLength; i++)
	{
		for(j = 0; j < sideLength; j++)
		{
				gRenderer->RenderTileByXY(i,j,(j+i)%2);
		}
	}
	//gRenderer->RenderFlush();

	m_gameChess.Process();

	//gRenderer->RenderFlush();

	for(i = 0; i < sideLength; i++)
	{
		for(j = 0; j < sideLength; j++)
		{
			if(i == (unsigned long)selectedTileX && j == (unsigned long)selectedTileY)
			{
				if(m_bPieceSelected)
				{
					gRenderer->RenderTileByXY(i, j, 3);
				}
				else
				{
					gRenderer->RenderTileByXY(i, j, 2);
				}
			}
		}
	}
	gRenderer->RenderFlush();
		
	gRenderer->SetRenderTarget(Renderer::BACKBUFFERTARGET, 0);
	
	//Render the texture we just rendered to.
	gRenderer->SelectTexture(handleRenderTarget,0);
	gRenderer->RenderSprite(3.5f/16.0f,0.0f,9.0f/16.0f,1.0f);

	if(m_bDisplay)
	{
		gRenderer->SelectTexture(m_nH1);
		gRenderer->RenderSprite(0.0f,0.0f,1.0f,1.0f);
		//gRenderer->RenderFlush();
	}


	gRenderer->RenderFlush();


	unsigned long int num =	gRenderer->GetNumTrianglesRendered();
	//Render FPS

	int n = sprintf_s(m_szFontBuffer, 100, "Frames per second: %g\nTriangles per frame: %d", m_dFPS,num);


	m_fntFont.RenderText(m_szFontBuffer, 0.0f, 1.0f, 0.0f, 2);
		
	//gRenderer->SelectTexture(-1);
	gRenderer->EndScene();
	

	gRenderer->FlipPages();
	frameCounter++;
	return true;
}

void Game::Release()
{
	m_fntFont.Release();
	m_inInputP1.Release();
	m_pObjectManager->Release();
	m_pObjectManager = NULL;
	gRenderer->Release();
	gRenderer = NULL;
	return;
}
