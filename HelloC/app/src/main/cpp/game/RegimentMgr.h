#pragma once

#include "iType.h"
#include "iPoint.h"
#include "iArray.h"
#include "TextGroup.h"

struct ArmyCounter
{
	int idx;//자신의 인데스
	int nation, pattern;
	char title[16];

	int stepNum, stepMod, stepCurr;
	int strength[6] = {};	// 짝:공 홀:방
	int special;

	int type, ap, dp, aup, dup;//current
};

struct GeneralInfo
{
	int idx;//자신의 인덱스
	int nation;
	char name[30] = "";
	int race;
	int mark;
	char note[30] = "";

	char leaderRank;	int leaderNum;
	char magicRank;		int magicNum;
	int maxSteps, currStep;
	int patterns[8][2];

	int controlRange;

	int regimentIdx = -1;
	int appoint = 0;
};

struct RegimentInfo
{
	int hexIdx;
	iPoint ps;
	bool ally;
	int count;
	ArmyCounter* army[4];
	int ar, dr, mv;
	GeneralInfo* gi;
};

struct RegimentText
{
	Texture** heroNameTitle;
	Texture** counterID;
	Texture** nation;
	Texture** heroName;
	Texture*** heroRace;
	Texture** heroNote;
};

extern RegimentText* regimentText;

void initRegimentMgr();
void freeRegimentMgr();
void addRegimentMgr();
void delRegimentMgr();
int regimentsTotalCount();
RegimentInfo* regimentAtIndex(int index);

