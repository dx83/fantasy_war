#pragma once

extern const char* mapName;

struct TileType
{
	int hexIdx;
	char name[30];
	int move, defBonus = 0;
	int index = 9999;
};

extern TileType terrain[54];
extern TileType landmark[68];
extern int tileTerrain[1193][2];

struct BaseCounter
{
	int nation, pattern;
	char title[16];
	int stepNum, stepIdx, stepMod;
	int special;
	int hexIndex;
};

extern int steps[3][11][6];
#define TOTALCOUNTER 84
extern BaseCounter scen_starter_counter[TOTALCOUNTER];

struct BaseGeneral
{
	int nation;
	char name[30];
	int race;
	int mark;
	char note[30];
	
	char leaderRank;	int leaderNum;
	char magicRank;		int magicNum;
	int steps;
	int patterns[8][2];

	int controlRange;
	
	int hexIndex;
	// int appoint;
};

extern char nationName[8][30];
extern char generaRace[8][3][30];
#define TOTALGENERAL 47
extern BaseGeneral scen_starter_general[TOTALGENERAL];

