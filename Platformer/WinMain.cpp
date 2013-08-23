
#include <windows.h>
#include "Random.h"
#include "Renderer.h"
#include "Common.h"
#include "SoundManager.h"
#include "Game.h"

#define FPS_UPDATE_NUM 70

#pragma message ("START PLATTFORM DETECTION")

#if defined(_M_IX86)

#pragma message ("_M_IX86 defined.")

#endif

#if defined(_X86_)

#pragma message ("_X86_ defined.")

#endif

#if defined(_M_AMD64)

#pragma message ("_M_AMD64 defined.")

#endif

#if defined(_AMD64_)

#pragma message ("_AMD64_ defined.")

#endif

#if defined(_M_IA64)

#pragma message ("_M_IA64 defined.")

#endif

#if defined(_IA64_)

#pragma message ("_IA64_ defined.")

#endif

#if defined(_68K_)

#pragma message ("_68K_ defined.")

#endif

#if defined(_MPPC_)

#pragma message ("_MPPC_ defined.")

#endif

#pragma message ("END PLATTFORM DETECTION")

// function declarations //

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SettingsManager settingsManager;
	WNDCLASSEX windowClass;		// window class
	HWND hWND = NULL;
	int width = 1024;
	int height = 768;
	int refreshRate = 0;
	bool fullscreen = false;
	bool vsync = false;

	if(settingsManager.Init("settings.xml"))
	{
		settingsManager.LoadStartupSettings();

		width = settingsManager.m_nWidth;
		height = settingsManager.m_nHeight;
		fullscreen = settingsManager.m_bFullscreen;
		vsync = settingsManager.m_bVsync;

		if(fullscreen)
		{
			refreshRate = settingsManager.m_nRefreshRate;

		}
		else
		{
			refreshRate = 0;
		}
	}
	else
	{
		DEBUGPRINTF("Could not load settings. :-(\n");
		return (0);
	}


	// fill out the window class structure
	windowClass.cbSize		= sizeof(WNDCLASSEX);
	windowClass.style			= CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc		= WindowProc;
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance		= hInstance;
	windowClass.hIcon		= LoadIcon(NULL, IDI_APPLICATION);			
	windowClass.hCursor		= LoadCursor(NULL, IDC_ARROW);	
	windowClass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.lpszMenuName	= NULL;						
	windowClass.lpszClassName	= TEXT("window");
	windowClass.hIconSm		= LoadIcon(NULL, IDI_WINLOGO);	

	// register the windows class
	if (!RegisterClassEx(&windowClass))
		return 0;
		
#if 0
	hWND = CreateWindowEx(NULL, TEXT("window"), TEXT("TODO: NAME GAME"),
	WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, width, height,
 		NULL, NULL, hInstance,	 NULL);
#else
	HMENU hMenu = CreateMenu();
	InsertMenu(hMenu, 0, MF_BYPOSITION | MF_ENABLED, NULL, "File"); 
	hWND = CreateWindowEx(NULL, TEXT("window"), TEXT("TODO: NAME GAME"),
	WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, width, height,
 		NULL, hMenu, hInstance,	 NULL);	
#endif
	// check if window creation failed (hwnd would equal NULL)
	if (!hWND)
		return 0;
	

	Game game(hWND, width, height, fullscreen, vsync, refreshRate, settingsManager);
	bool paused = false;
	bool quit = false;
	if(game.Init())
	{

		MSG	msg;	
		double elapsedTime1 = (double)timeGetTime();
		double elapsedTime2;
		//tutils::dsfmt19937 rand(1234);
		Random rand = Random(1234);
		unsigned long frameInc = 0;
		while (1)
		{
			PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE);
	
	 		if(msg.message == WM_QUIT)
				break;
			else if(msg.message == WM_MOUSEACTIVATE)
			{
				msg.lParam = HIWORD(MA_ACTIVATE);
			}
			else if(msg.message == WM_SIZE)
			{
				if(msg.wParam == SIZE_MINIMIZED)
					paused = true;
				if(msg.wParam != SIZE_MINIMIZED)
					paused = false;
			}
			else if(msg.message == WM_ENABLE)
			{
				if(msg.wParam)
					paused = false;
				else
					paused = true;
			}
			else if(msg.message == WM_EXITSIZEMOVE)
			{
				paused = false;
			}
			else if(msg.message == WM_ENTERSIZEMOVE)
			{
				paused = true;
			}
			else
			{
				++frameInc %= FPS_UPDATE_NUM;
				if(frameInc == 0)
				{
					elapsedTime2 = (double)timeGetTime() - elapsedTime1;
					elapsedTime1 = timeGetTime();
					elapsedTime2 = FPS_UPDATE_NUM * 1000.0f / elapsedTime2;
					game.SetFPS(elapsedTime2);
				}
				if(!paused)
				{
					ShowCursor(!fullscreen);
					SetCapture(hWND);
					quit = !game.Process();
				}
				else
				{
					ShowCursor(paused);
					ReleaseCapture();
					Sleep(10);
				}
				if(quit)
				{
					ShowCursor(paused);
					ReleaseCapture();
					break;
				}
				/*if(elapsedTime2 < 400.0)
				{
					DEBUGPRINTF("Frame Rate: %g\n", elapsedTime2);
					;
				}*/
				Sleep(0);
				TranslateMessage(&msg);									
				DispatchMessage(&msg);

			}
		}
	}

	game.Release();
	
	UnregisterClass(TEXT("window"), hInstance);
	//return (msg.wParam); //should not use msg.wParam in this case as it would be 0x1b for escape key :-p
	return (0); //returns a normal error code since escape in fact causes the program to terminate properly.
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE:
			return 0;
			break;

		case WM_CLOSE:					// windows is closing
			PostQuitMessage(0);
			return 0;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
			break;
		
		default:
			break;
	}

	return (DefWindowProc(hwnd, message, wParam, lParam));
}
