#include "Common.h"
#ifdef __SOUND_ON__
#ifndef __SOUNDMANAGER_H_INCLUDED__
#define __SOUNDMANAGER_H_INCLUDED__
#include <al\al.h>
#include <al\alc.h>
#include <al\alut.h>
#include "SoundList.h"

class SoundManager
{

private:
	ALuint Buffer[NUM_SOUNDS];
	ALuint Source[NUM_SOUNDS];

	ALfloat SourcePos[NUM_SOUNDS][3];
	ALfloat SourceVel[NUM_SOUNDS][3];

	ALfloat ListenerPos[3];
	ALfloat ListenerVel[3];
	ALfloat ListenerOri[6];

public:
	SoundManager();
	~SoundManager();
	void Init();
	void Release();

	

};

#endif
#endif