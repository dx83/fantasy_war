#include "Game.h"

#include "Loading.h"
#include "Intro.h"
#include "Menu.h"
#include "Proc.h"
#include "Sound.h"

#include "Test.h"

//#define LOWDISK
void loadGame()
{
#if 0
    AudioInfo ai[5] =
	{
#ifndef LOWDISK
		{"assets/wav/00maintitle.wav", true, 1.0f},
		{"assets/wav/01ingame.wav", true, 1.0f},
		{"assets/wav/02menu_btn.wav", false, 1.0f},
		{"assets/wav/03regimentClick.wav", false, 1.0f},
		{"assets/wav/04foot.wav", false, 1.0f},
#else
		{"assets/wav/02menu_btn.wav", false, 1.0f},
		{"assets/wav/03regimentClick.wav", false, 1.0f},
		{"assets/wav/02menu_btn.wav", false, 1.0f},
		{"assets/wav/03regimentClick.wav", false, 1.0f},
		{"assets/wav/04foot.wav", false, 1.0f},
#endif
	};
	loadAudio(ai, 5);
	audioVolume(1.0f, 1.0f, 0);
	audioPlay(0);
#endif
    //loadSound();
	setStringName("assets/font/BMJUA_ttf.ttf");

#if 1// 스트링 로딩
	gameState = gs_delay;
    //setLoading(gs_intro, NULL, loadIntro, 1);
	setLoading(gs_menu, NULL, loadMenu, 1);
#else
    gameState = gs_proc;
	switch (gameState)
	{
	case gs_intro: loadIntro(); gameState = gs_intro; break;
	case gs_menu: loadMenu(); gameState = gs_menu; break;
	case gs_proc: loadProc(); gameState = gs_proc; break;

	case gs_test: loadTest(); gameState = gs_test; break;
	}
#endif
}

void freeGame()
{
	switch (gameState)
	{
	case gs_intro: freeIntro(); break;
	case gs_menu: freeMenu(); break;
	case gs_proc: freeProc(); break;

	case gs_test: freeTest();
	}
	//freeAudio();
}

void drawGame(float dt)
{
	switch (gameState)
	{
	case gs_intro:drawIntro(dt); break;
	case gs_menu: drawMenu(dt); break;
	case gs_proc: drawProc(dt); break;

	case gs_test: drawTest(dt); break;
	}
	drawLoading(dt);
}

void keyGame(iKeyStat stat, iPoint point)
{
	if (keyPopYesNo(stat, point))
		return;
	if (keyLoading(stat, point))
		return;

	switch (gameState) 
	{
	case gs_intro: keyIntro(stat, point); break;
	case gs_menu: keyMenu(stat, point); break;
	case gs_proc: keyProc(stat, point); break;

	case gs_test: keyTest(stat, point); break;
	}
}

