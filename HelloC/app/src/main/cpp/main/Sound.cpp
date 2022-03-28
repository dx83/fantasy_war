#include "Sound.h"

#define _bgmMAX 2
#define _sfxMAX 3
//#define SOUNDON

void loadSound()
{
#if 0
	const char* bgm[_bgmMAX] =
	{
			"wav/00maintitle.wav",  //0
			"wav/01ingame.wav"      //1
	};
#endif
	const char* sfx[_sfxMAX] =
	{
			"wav/02menu_btn.wav",       //0
			"wav/03regimentClick.wav",  //1
			"wav/04foot.wav"            //2
	};

	JNIEnv* env = getEnv();

	//bgm
#if 0
	for(int i = 0; i < _bgmMAX; i++)
	{
		jstring path = javaNewStringChar(env, bgm[i]);
		ndkLoadBgmSound(path);
		env->DeleteLocalRef(path);
	}
#endif
	//sfx
	for(int i = 0; i < _sfxMAX; i++)
	{
		jstring path = javaNewStringChar(env, sfx[i]);
		ndkLoadSfxSound(path);
		env->DeleteLocalRef(path);
	}
}

void playBgmSound(int index)
{
	ndkPlayBgmSound(index);
}

void playSfxSound(int index)
{
#ifdef SOUNDON
	ndkPlaySfxSound(index);
#endif
}

void stopBgmSound(int index)
{
	ndkStopBgmSound(index);
}

void stopSfxSound(int index)
{
	ndkStopSfxSound(index);
}

void volumeBgmSound(float volume)
{
	ndkVolumeBgmSound(volume);
}

void volumeSfxSound(float volume)
{
	ndkVolumeSfxSound(volume);
}

void pauseBgmSound(bool pause)
{
	ndkPauseBgmSound(pause);
}

