#include "RegimentMgr.h"

#include "iStd.h"

#include "LoadMap.h"

static ArmyCounter** armyCounters;
static GeneralInfo** generals;

static iArray* regiments;
void freeArrayRegiment(void* data);

RegimentText* regimentText;

void initRegimentMgr()
{
	int i, j, k;
	armyCounters = new ArmyCounter * [TOTALCOUNTER];
	generals = new GeneralInfo * [TOTALGENERAL];
	for (i = 0; i < TOTALGENERAL; i++)
		generals[i] = new GeneralInfo;
	regiments = new iArray(freeArrayRegiment);

	Hexagon* hexs = getHexInfo();

	bool exist = false;
	for (i = 0; i < TOTALCOUNTER; i++)
	{
		armyCounters[i] = new ArmyCounter;
		ArmyCounter* a = armyCounters[i];
		BaseCounter* b = &scen_starter_counter[i];
		//
		a->idx = i;
		a->nation = b->nation;
		a->pattern = b->pattern;
		strcpy(a->title, b->title);

		a->stepNum = b->stepNum;
		a->stepMod = b->stepMod;
		b->stepMod ? a->stepCurr = b->stepNum - b->stepMod : a->stepCurr = b->stepNum;
		for (j = 0; j < a->stepNum * 2; j++)
			a->strength[j] = steps[b->stepNum - 1][b->stepIdx][j];

		a->special = b->special;

		int reduce;
		b->stepMod ? reduce = 1 : reduce = 0;
		switch (a->stepNum)
		{
		case 3:	a->nation < 5 ? a->type = 0 + reduce : a->type = 6 + reduce; break;
		case 2:	a->nation < 5 ? a->type = 3 + reduce : a->type = 9 + reduce; break;
		case 1:	a->nation < 5 ? a->type = 5 : a->type = 11; break;
		}

		b->stepMod ? a->ap = a->strength[2] : a->ap = a->strength[0];
		b->stepMod ? a->dp = a->strength[3] : a->dp = a->strength[1];
		//
		a->aup = 0;
		int bonus = hexs[b->hexIndex].defBonus;
		bonus ? a->dup = a->dp + bonus : a->dup = 0;
		//
		for (j = 0; j < regiments->count; j++)// 같은 연대에 부대 추가
		{
			RegimentInfo* rg = (RegimentInfo*)regiments->objectAtIndex(j);
			if (rg->hexIdx == b->hexIndex)
			{
				exist = true;
				rg->count++;
				rg->army[rg->count - 1] = a;
				rg->ar += a->ap;
				bonus ? rg->dr += a->dup : rg->dr += a->dp;
				break;
			}
			exist = false;
		}
		if (exist == false)
		{
			RegimentInfo* rg = new RegimentInfo;
			rg->hexIdx = b->hexIndex;
			rg->ps = hexs[b->hexIndex].point;
			rg->count = 1;
			rg->army[0] = a;
			for (int i = 0; i < 3; i++)
				rg->army[i + 1] = NULL;

			rg->army[0]->nation < 5 ? rg->ally = true : rg->ally = false;

			rg->ar = a->ap;
			bonus ? rg->dr = a->dup : rg->dr = a->dp;
			rg->mv = 4;

			regiments->addObject(rg);
			int currIdx = regiments->count - 1;

			setRegimentIndex(b->hexIndex, currIdx);//regimentInfo index in hex

			for (j = 0; j < TOTALGENERAL; j++)
			{
				BaseGeneral* bg = &scen_starter_general[j];
				if (b->hexIndex == bg->hexIndex)
				{
					GeneralInfo* gi = generals[currIdx];
					gi->idx = currIdx;
					gi->nation = bg->nation;
					strcpy(gi->name, bg->name);
					gi->race = bg->race;
					gi->mark = bg->mark;
					strcpy(gi->note, bg->note);
					gi->leaderRank = bg->leaderRank;
					gi->leaderNum = bg->leaderNum;
					gi->magicRank = bg->magicRank;
					gi->magicNum = bg->magicNum;
					gi->maxSteps = gi->currStep = bg->steps;
					for (k = 0; k < 8; k++)
					{
						gi->patterns[k][0] = bg->patterns[k][0];
						gi->patterns[k][1] = bg->patterns[k][1];
					}
					gi->controlRange = bg->controlRange;
					gi->regimentIdx = currIdx;
					gi->appoint = 0;
					rg->gi = gi;
					break;
				}
			}
		}
	}

	int addIndex = 0;
	for (i = 0; i < TOTALGENERAL; i++)// 부대가 없는 영웅들 삽입
	{
		BaseGeneral* bg = &scen_starter_general[i];
		if (bg->hexIndex == -1)
		{
			GeneralInfo* gi = generals[regiments->count + addIndex];
			gi->idx = regiments->count + addIndex;
			gi->nation = bg->nation;
			memcpy(gi->name, bg->name, sizeof(bg->name));
			gi->race = bg->race;
			gi->mark = bg->mark;
			memcpy(gi->note, bg->note, sizeof(bg->note));
			gi->leaderRank = bg->leaderRank;
			gi->leaderNum = bg->leaderNum;
			gi->magicRank = bg->magicRank;
			gi->magicNum = bg->magicNum;
			gi->maxSteps = gi->currStep = bg->steps;
			gi->controlRange = bg->controlRange;
			gi->appoint = 0;
			addIndex++;
		}
	}

	// 정렬
	for (i = 0; i < regiments->count; i++)
	{
		RegimentInfo* rg = (RegimentInfo*)regiments->objectAtIndex(i);

		for (j = 0; j < rg->count - 1; j++)
		{
			for (k = j; k < rg->count; k++)
			{
				if (rg->army[j]->ap < rg->army[k]->ap)					// 공격력 높은쪽이 앞에
				{
					ArmyCounter* t = rg->army[j];
					rg->army[j] = rg->army[k];
					rg->army[k] = t;
				}
				else if (rg->army[j]->ap == rg->army[k]->ap)			// 방어력이 높은쪽이 앞에
				{
					if (rg->army[k]->dp < rg->army[k]->dp)
					{
						ArmyCounter* t = rg->army[j];
						rg->army[j] = rg->army[k];
						rg->army[k] = t;
					}
					else if (rg->army[j]->dp == rg->army[k]->dp)
					{
						if (rg->army[j]->nation > rg->army[k]->nation)	// 국가 번호 높은쪽이 앞에
						{
							ArmyCounter* t = rg->army[j];
							rg->army[j] = rg->army[k];
							rg->army[k] = t;
						}
					}
				}
			}
		}
	}

	regimentText = new RegimentText;
	regimentText->heroNameTitle = new Texture*[TOTALGENERAL];
	regimentText->counterID = new Texture*[TOTALCOUNTER];
	regimentText->nation = new Texture*[8];
	regimentText->heroName = new Texture*[TOTALGENERAL];
	regimentText->heroRace = new Texture**[8];
	for (i = 0; i < 8; i++)
		regimentText->heroRace[i] = new Texture*[3];
	regimentText->heroNote = new Texture*[TOTALGENERAL];

	iGraphics* g = iGraphics::share();
	iSize size = iSizeMake(200, 25);

	setStringName("assets/font/Maplestory_Light.ttf");
	setStringSize(17);
	setStringRGBA(0, 0, 0, 255);
	setStringBorder(2);
	setStringStyle(0);

	for (i = 0; i < TOTALGENERAL; i++)
	{
		generals[i]->nation < 5 ? setStringBorderRGBA(134, 229, 127, 255) : setStringBorderRGBA(201, 138, 255, 255);
		generals[i]->nation < 5 ? setStringStyle(1) : setStringStyle(0);
		g->init(size);
		g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%s", generals[i]->name);
		regimentText->heroNameTitle[i] = g->getTexture();
	}

	setStringSize(14);
	setStringRGBA(0, 0, 0, 255);
	setStringBorder(0);
	setStringBorderRGBA(0, 0, 0, 255);
	setStringStyle(0);

	size = iSizeMake(134, 25);
	for (i = 0; i < TOTALCOUNTER; i++)
	{
		g->init(size);
		g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%s", armyCounters[i]->title);
		regimentText->counterID[i] = g->getTexture();
	}

	setStringName("assets/font/Maplestory_Bold.ttf");
	setStringSize(20);
	setStringRGBA(50, 50, 50, 255);
	setStringBorder(2);
	setStringBorderRGBA(196, 183, 59, 255);
	setStringStyle(0);

	for (i = 0; i < 8; i++)
	{
		g->init(size);
		g->drawString(0, size.height / 2, VCENTER | LEFT, "%s", nationName[i]);
		regimentText->nation[i] = g->getTexture();
	}

	for (i = 0; i < TOTALGENERAL; i++)
	{
		g->init(size);
		g->drawString(0, size.height / 2, VCENTER | LEFT, "%s", generals[i]->name);
		regimentText->heroName[i] = g->getTexture();
	}

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 3; j++)
		{
			g->init(size);
			g->drawString(0, size.height / 2, VCENTER | LEFT, "%s", generaRace[i][j]);
			regimentText->heroRace[i][j] = g->getTexture();
		}
	}

	setStringSize(18);
	for (i = 0; i < TOTALGENERAL; i++)
	{
		g->init(size);
		g->drawString(0, size.height / 2, VCENTER | LEFT, "%s", generals[i]->note);
		regimentText->heroNote[i] = g->getTexture();
	}

	setStringSize(20);

}

void freeArrayRegiment(void* data)
{
	RegimentInfo* rg = (RegimentInfo*)data;
	delete rg;
}

void freeRegimentMgr()
{
	int i;
	for (i = 0; i < TOTALCOUNTER; i++)
		delete armyCounters[i];
	delete armyCounters;

	for (i = 0; i < TOTALGENERAL; i++)
		delete generals[i];
	delete generals;

	delete regiments;
	
	for (i = 0; i < TOTALGENERAL; i++)
	{
		freeImage(regimentText->heroNameTitle[i]);
		freeImage(regimentText->heroName[i]);
		freeImage(regimentText->heroNote[i]);
	}
	delete regimentText->heroNameTitle;
	delete regimentText->heroName;
	delete regimentText->heroNote;

	for (i = 0; i < TOTALCOUNTER; i++)
		freeImage(regimentText->counterID[i]);
	delete regimentText->counterID;

	for (i = 0; i < 8; i++)
	{
		freeImage(regimentText->nation[i]);
		for (int j = 0; j < 3; j++)
			freeImage(regimentText->heroRace[i][j]);
		delete regimentText->heroRace[i];
	}
	delete regimentText->nation;
	delete regimentText->heroRace;

	delete regimentText;
}

void addRegimentMgr()
{

}

void delRegimentMgr()
{

}

int regimentsTotalCount()
{
	return regiments->count;
}

RegimentInfo* regimentAtIndex(int index)
{
	return (RegimentInfo*)regiments->objectAtIndex(index);
}

