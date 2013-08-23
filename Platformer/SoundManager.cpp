#include "SoundManager.h"
#ifdef __SOUND_ON__

SoundManager::SoundManager()
{
	for(int i = 0; i < NUM_SOUNDS; i++)
	{
		Buffer[i] = 0;
		Source[i] = 0;
		for(int j = 0; j < 3; j++)
		{
			SourcePos[i][j] = 0.0;
			SourceVel[i][j] = 0.0;

		}
	}
	for(int i = 0; i < 3; i++)
	{
		ListenerPos[i] = 0.0;
		ListenerVel[i] = 0.0;
	}
	ListenerOri[0] = 0.0;
	ListenerOri[1] = 0.0;
	ListenerOri[2] = -1.0;
	ListenerOri[3] = 0.0;
	ListenerOri[4] = 1.0;
	ListenerOri[5] = 0.0;
}
#endif