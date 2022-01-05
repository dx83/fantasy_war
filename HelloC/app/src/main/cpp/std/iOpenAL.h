#pragma once

#include "iType.h"

#if (OS==OS_WINDOWS)
#include <al.h>
#include <alc.h>
#pragma comment(lib, "OpenAL32.lib")

#elif (OS==OS_ANDROID)

#endif

class iOpenAL
{
public:
	iOpenAL(int audioNum);
	virtual ~iOpenAL();

	void initBuffer(int idx, const char* szFormat, ...);
	void initSource(int idx, bool repeat);

	void play(int idx, bool repeat);
	void pause(int idx);
	void stop(int idx);
	void volume(int idx, float vol);

public:
#if (OS==OS_WINDOWS)
	ALuint* gBuffer;
	ALuint* gSource;
	int bufCount;
#elif (OS==OS_ANDROID)

#endif
};

struct AudioInfo
{
	const char* fileName;
	bool repeat;
	float volume;
};

void loadAudio(AudioInfo* ai, int aiNum);
void freeAudio();

void pauseAudio();
void resumeAudio();

void audioPlay(int idx);
void audioStop();
void audioVolume(float bgm, float eff, int effNum);
//temp
void audioVolume(float volume, int soundNum);
float getAudioVol(int soundNum);

