#include "Sound.h"

void loadSound()
{
	const char* bgm[2] =
	{
			"wav/00maintitle.wav",  //0
			"wav/01ingame.wav"      //1
	};

	const char* sfx[3] =
	{
			"wav/02menu_btn.wav",       //0
			"wav/03regimentClick.wav",  //1
			"wav/04foot.wav"            //2
	};

	JNIEnv* env = getEnv();

	//bgm
	for(int i = 0; i < 2; i++)
	{
		jstring path = javaNewStringChar(env, bgm[i]);
		ndkLoadBgmSound(path);
		env->DeleteLocalRef(path);
	}

	//sfx
	for(int i = 0; i < 3; i++)
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
	ndkPlaySfxSound(index);
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