#ifndef __GAME_H_INCLUDED__
#define __GAME_H_INCLUDED__

#include "Common.h"
#include "Random.h"
#include "Input.h"
#include "Renderer.h"
#include "Settings.h"
#include "ObjectManager.h"
#include "Font.h"
#include "Chess.h"

class Game : UniversalInterface<Game>
{
public:
	Game();
	Game(HWND hwnd, int width, int height, bool fullscreen, bool vsync, unsigned int refRate, SettingsManager* settings);
	~Game();
	bool Init();
	bool Process();
	void Release();
	void SetFPS(double fps);
	double GetAverageFPS();

private:
	HWND m_hwnd;
	int m_nWidth, m_nHeight;
	bool m_bFullscreen;
	bool m_bVsync;
	unsigned int m_uRefRate;
	int m_nH1, m_nBGHandle;
	Random m_rndRand;
	Input m_inInputP1;
	XINPUT_STATE *m_pCurrentState; //input info for current controller being looked at.
	bool m_bDisplay;
	Font m_fntFont;
	double m_dFPS;
	double m_dAvgFPS;
	int selectedTileX;
	int selectedTileY;
	Chess m_gameChess;
	bool m_bPieceSelected;

	//for testing purposes.
	int life[(int)COORDOFFSETINV][(int)COORDOFFSETINV];
	int oldlife[(int)COORDOFFSETINV][(int)COORDOFFSETINV];

	SettingsManager *m_pSettingsManager;
	ObjectManager *m_pObjectManager;

	unsigned short frameCounter;
	char m_szFontBuffer[100];
};

#endif