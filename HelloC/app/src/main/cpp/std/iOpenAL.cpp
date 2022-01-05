#include "iOpenAL.h"

#include "iStd.h"

iOpenAL::iOpenAL(int audioNum)
{
}

iOpenAL::~iOpenAL()
{
}

void iOpenAL::initBuffer(int idx, const char* szFormat, ...)
{
}

void iOpenAL::initSource(int idx, bool repeat)
{
}

void iOpenAL::play(int idx, bool repeat)
{
}

void iOpenAL::pause(int idx)
{
}

void iOpenAL::stop(int idx)
{
}

void iOpenAL::volume(int idx, float vol)
{
}

void loadAudio(AudioInfo* ai, int aiNum)
{
}

void freeAudio()
{
}

void pauseAudio()
{
}

void resumeAudio()
{
}

void audioPlay(int list)
{
}

void audioStop(int index)
{
}

void audioStop()
{
}

void audioVolume(float bgm, float eff, int effNum)
{
}

void audioVolume(float volume, int soundNum)
{
}

float getAudioVol(int soundNum)
{
    return soundNum;
}

