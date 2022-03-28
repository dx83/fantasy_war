#include "RegimentAction.h"

#include "iStd.h"
#include "DrawCounters.h"
#include "LoadMap.h"
#include "RegimentMgr.h"
#include "ProcUI_RegimentInfo.h"
#include "ProcUI_Commander.h"

#define RATEX   9
#define MAXTEXT 17
#define MAXNUMB 56
#define MAXSTAT 17
#define MAXRATX 9//3
bool warscene;

RegimentAction* ra;
void drawActionRange(iPopup* pop, float dt, float rate);
void drawBattleField(iPopup* pop, float dt, float rate);
void drawBattleButton(float dt);

void battleAttackerDecide();
void battleRatioCalc();
void battleDiceRolling(float dt);
void battleRegimentStepReduce(RegimentInfo* rg, bool attack);
void regimentElimination();

void createRegimentAction()
{
    int i;
    warscene = false;

    ra = new RegimentAction;
    ra->pop = new iPopup*[2];

    // Tracking
    ra->tex = new Texture*[RATEX];
    ra->tex[0] = createImage("assets/battle/battle_icon.png");
    ra->tex[1] = createImage("assets/map/hex_shadow.png");

    iPopup* pop = new iPopup();
    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointZero;
    pop->methodTogether = drawActionRange;
    ra->pop[0] = pop;

    ra->tHex = new TRACKHEX[6];

    // Battle UI
    const char* charText[MAXSTAT] =
    {
        "0", "1", "2", "3", "4", "5",
        "S", "A", "B", "C", "D",//83, 65, 66, 67, 68
        "A", "B", "C", "0", "1", "2"
    };

    ra->uiText = new Texture*[MAXTEXT];
    ra->bigNum = new Texture*[MAXNUMB];
    ra->status = new Texture*[MAXSTAT];
    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(80, 80);
    // Battle UI : hero step
    setStringName("assets/font/Maplestory_Bold.ttf");
    setStringSize(28);
    setStringRGBA(0, 0, 0, 255);
    setStringBorder(5);
    setStringBorderRGBA(153, 112, 0, 255);

    g->init(size);
    setRGBA(255, 255, 255, 255);
    g->fillRect(0, 0, size.width, size.height, 20);
    g->drawString(size.width / 2, size.height / 2 - 20, VCENTER | HCENTER, "영웅");
    g->drawString(size.width / 2, size.height / 2 + 20, VCENTER | HCENTER, "체력");
    ra->uiText[0] = g->getTexture();

    size = iSizeMake(80, 640 - 180);
    g->init(size);
    g->fillRect(0, 0, size.width, size.height, 50);
    ra->uiText[1] = g->getTexture();

    size = iSizeMake(70, 50);
    setStringSize(50);
    setStringRGBA(255, 255, 255, 255);
    setStringBorder(3);
    setStringBorderRGBA(0, 0, 0, 255);
    const char* sign[5] = { ".", "(", ")", "+", "-" };
    for (i = 0; i < MAXNUMB; i++)	// big number
    {
        g->init(size);
        if ( i < 51 )
            g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%d", i);
        else
            g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, sign[i - 51]);
        ra->bigNum[i] = g->getTexture();
    }

    // Battle UI : hero name box
    size = iSizeMake(180, 50);
    for (i = 0; i < 2; i++)
    {
        g->init(size);
        i == 0 ? setRGBA(209, 178, 255, 200) : setRGBA(183, 240, 177, 200);// 적군 : 아군
        g->fillRect(0, 0, size.width, size.height, 50);
        setLineWidth(3);
        setRGBA(0, 0, 0, 200);
        g->drawRect(1, 1, size.width - 2, size.height - 2, 50);
        ra->uiText[2 + i] = g->getTexture();
    }

    // Battle UI : total power
    ra->uiText[4] = createImage("assets/battle/total_value_box.png");
    size = iSizeMake(200, 50);
    setStringSize(40);
    setStringBorder(3);
    setStringBorderRGBA(150, 150, 150, 255);
    for (i = 0; i < 2; i++)
    {
        if (i == 0)
            setStringRGBA(255, 20, 20, 255);
        else
            setStringRGBA(0, 0, 240, 255);

        g->init(size);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, i == 0 ? "총 공 격 력" : "총 방 어 력");
        ra->uiText[5 + i] = g->getTexture();
    }

    // Battle UI : center
    size = iSizeMake(120, 50);
    setStringName("assets/font/Roboto-Regular.ttf");
    setStringSize(55);
    setStringRGBA(255, 200, 0, 255);
    setStringBorder(3);
    setStringBorderRGBA(153, 112, 0, 255);

    const char* cText[3] = { "승 비", ":", "VS" };
    for (i = 0; i < 3; i++)
    {
        g->init(size);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, cText[i]);
        ra->uiText[7 + i] = g->getTexture();
    }

    // Battle UI : status text
    size = iSizeMake(50, 50);
    setStringName("assets/font/GmarketSansTTFBold.ttf");
    setStringSize(50);
    setStringBorder(3);
    setStringBorderRGBA(50,50, 50, 255);
    setLineWidth(5);
    for (i = 0; i < MAXSTAT; i++)
    {
        switch (i)
        {
            case 0:	case 1:	case 10: case 13: case 14:
                setStringRGBA(53, 53, 53, 255); setRGBA(53, 53, 53, 255); break;
            case 2:	case 9:
                setStringRGBA(18, 102, 255, 255); setRGBA(18, 102, 255, 255); break;
            case 3:	case 8:	case 12: case 15:
                setStringRGBA(0, 255, 0, 255); setRGBA(0, 255, 0, 255); break;
            case 4:	case 7:
                setStringRGBA(255, 36, 36, 255); setRGBA(255, 36, 36, 255); break;
            case 5:	case 6:	case 11: case 16:
                setStringRGBA(255, 235, 0, 255); setRGBA(255, 235, 0, 255); break;
        }
        g->init(size);
        g->drawRect(0, 0, size.width, size.height, 10);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%s", charText[i]);
        ra->status[i] = g->getTexture();
    }
    setLineWidth(1);

    // Battle UI : stat box
    size = iSizeMake(200, 50);
    g->init(size);
    setRGBA(255, 255, 255, 255);
    g->fillRect(0, 0, size.width, size.height, 10);
    g->drawRect(1, 1, size.width - 2, size.height - 2, 10);
    ra->uiText[10] = g->getTexture();

    // Battle UI : stat entry text
    size = iSizeMake(120, 80);
    setStringName("assets/font/Maplestory_Bold.ttf");
    setStringSize(40);
    setStringRGBA(0, 0, 0, 255);
    setStringBorder(5);
    setStringBorderRGBA(153, 112, 0, 255);

    const char* stat[2] = { "통 솔", "마 법" };
    for (i = 0; i < 2; i++)
    {
        g->init(size);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, stat[i]);
        ra->uiText[11 + i] = g->getTexture();
    }

    // Battle UI : DICE
    ra->dice = new Texture*[6];
    for (i = 0; i < 6; i++)
        ra->dice[i] = createImage("assets/battle/dice_%d.png", i);
    // Battle UI : DICE combat result box
    size = iSizeMake(65, 85);
    resetRGBA();
    setLineWidth(10);
    g->init(size);
    g->drawRect(0, 0, size.width, size.height, 10);
    ra->uiText[13] = g->getTexture();
    setLineWidth(1);
    // Battle UI : DICE magic text
    size = iSizeMake(80, 80);
    setStringName("assets/font/Maplestory_Bold.ttf");
    setStringSize(30);
    setStringBorder(5);

    g->init(size);
    setStringRGBA(0, 0, 0, 255);
    setStringBorderRGBA(153, 112, 0, 255);
    g->drawString(size.width / 2, size.height / 2 - 20, VCENTER | HCENTER, "이 하");
    g->drawString(size.width / 2, size.height / 2 + 20, VCENTER | HCENTER, "성 공");
    ra->uiText[14] = g->getTexture();

    g->init(size);
    g->drawString(size.width / 2, size.height / 2 - 20, VCENTER | HCENTER, "마 법");
    g->drawString(size.width / 2, size.height / 2 + 20, VCENTER | HCENTER, "실 패");
    ra->uiText[15] = g->getTexture();

    g->init(size);
    setStringRGBA(255, 0, 0, 255);
    setStringBorderRGBA(255, 200, 0, 255);
    g->drawString(size.width / 2, size.height / 2 - 20, VCENTER | HCENTER, "마 법");
    g->drawString(size.width / 2, size.height / 2 + 20, VCENTER | HCENTER, "타 격");
    ra->uiText[16] = g->getTexture();

    // Battle UI : pop
    ra->tex[2] = createImage("assets/battle/battle_field.png");

    pop = new iPopup(1);
    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointZero;
    pop->methodTogether = drawBattleField;
    pop->methodFigure[0] = drawBattleButton;
    ra->pop[1] = pop;

    // Battle UI : button
    for (i = 0; i < 2; i++)
        ra->tex[3 + i] = createImage("assets/battle/btn_battle%d.png", i);

    resetRGBA();
    size = iSizeMake(77, 207);
    setStringName("assets/font/RixYeoljeongdo_Regular.ttf");
    setStringSize(30);
    setStringRGBA(229, 216, 92, 255);
    setStringBorder(3);
    setStringBorderRGBA(50, 50, 50, 255);

    g->init(size);
    g->drawString(size.width / 2, size.height / 2 - 60, TOP | HCENTER, "전");
    g->drawString(size.width / 2, size.height / 2 - 20, TOP | HCENTER, "투");
    g->drawString(size.width / 2, size.height / 2 + 20, TOP | HCENTER, "개");
    g->drawString(size.width / 2, size.height / 2 + 60, TOP | HCENTER, "시");
    ra->tex[5] = g->getTexture();

    g->init(size);
    g->drawString(size.width / 2, size.height / 2 - 60, TOP | HCENTER, "군");
    g->drawString(size.width / 2, size.height / 2 - 20, TOP | HCENTER, "단");
    g->drawString(size.width / 2, size.height / 2 + 20, TOP | HCENTER, "후");
    g->drawString(size.width / 2, size.height / 2 + 60, TOP | HCENTER, "퇴");
    ra->tex[6] = g->getTexture();

    g->init(size);
    g->drawString(size.width / 2, size.height / 2 - 60, TOP | HCENTER, "전");
    g->drawString(size.width / 2, size.height / 2 - 20, TOP | HCENTER, "투");
    g->drawString(size.width / 2, size.height / 2 + 20, TOP | HCENTER, "종");
    g->drawString(size.width / 2, size.height / 2 + 60, TOP | HCENTER, "료");
    ra->tex[7] = g->getTexture();

    // 선택 아이콘 설정 폐기
    //ra->tex[7] = createImage("assets/battle/battle_select_icon.png");

    // 각종 변수 초기화
    ra->bStart = ra->bRetreat = false;
    ra->bFirst = ra->bRolling = false;

    //memset(ra->resDice, 0, sizeof(int)*4);
    for (i = 0; i < 4; i++)
        ra->resDice[i] = i;
    ra->battleRes = ra->battleHit = ra->magicHit = 0;
    ra->magicA = ra->magicD = false;

    ra->ratioA = ra->ratioD =
    ra->resultA = ra->resultD = 0;
    memset(ra->numA, 0, sizeof(int)*3);
    memset(ra->numD, 0, sizeof(int)*3);

    // 전투 종료
    ra->THEEND = false;

    // 설명문 텍스트
    ra->descText = new Texture*[MAXRATX];
    resetRGBA();
    size = iSizeMake(495, 135);
    setStringName("assets/font/RixYeoljeongdo_Regular.ttf");
    setStringSize(100);
    setStringBorder(15);
    setStringBorderRGBA(50, 50, 50, 255);

    g->init(size);
    setStringRGBA(255, 0, 0, 255);
    g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "양군 전멸");
    ra->descText[0] = g->getTexture();

    g->init(size);
    setStringRGBA(204, 61, 61, 255);
    g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "아군 전멸");
    ra->descText[1] = g->getTexture();

    g->init(size);
    setStringRGBA(229, 216, 92, 255);
    g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "적군 섬멸");
    ra->descText[2] = g->getTexture();

    ra->resultTextIdx = -1;// 전투 결과 텍스트 초기화

    setStringSize(25);
    setStringRGBA(50, 50, 50, 255);
    setStringBorder(0);
    const char* descBattle[18] =
    {
        "승비 : 공방 비교해서 방어측은 소수점 올림",
        "승비+통솔 수치+주사위 눈 => 높은쪽 승리",
        "패배한 쪽은 영웅의 체력을 한칸 줄인다.",

        "통솔 등급 : A > B > C",
        "통솔 등급이 높은 쪽은 승리시",
        "추가로 상대 유닛에 즉시 피해를 준다.",

        "비길시 : 통솔 등급이 높은 쪽이 승리",
        "통솔 등급이 같은면 주사위 눈 높은쪽 승리",
        "영웅 체력이 0인 경우 : 유닛이 즉시 피해",

        "마법 공격 : 각 영웅은 주사위를 굴려",
        "주사위 눈이 마법 수치 이하라면",
        "상대 영웅의 체력을 한칸 줄인다.",

        "마법 등급 : S > A > B > C > D",
        "마법 등급이 높은 쪽은 승리시",
        "추가로 상대 유닛에 즉시 피해를 준다.",

        "영웅의 체력이 남아있더라도",
        "유닛이 남아있지 않다면, 완전히 패패하고",
        "해당 군단은 게임에서 제거 됩니다.",
    };

    for (i = 0; i < MAXRATX - 3; i++)//0~5
    {
        g->init(size);
        g->drawString(15, size.height / 2 - 40, VCENTER | LEFT, descBattle[0+(i*3)]);
        g->drawString(15, size.height / 2, VCENTER | LEFT, descBattle[1+(i*3)]);
        g->drawString(15, size.height / 2 + 40, VCENTER | LEFT, descBattle[2+(i*3)]);
        ra->descText[i + 3] = g->getTexture();
    }

    ra->tex[8] = createImage("assets/battle/battle_text_arrow.png");

    ra->descTextIdx = 0;
    ra->bDescL = ra->bDescR = false;
}

void freeRegimentAction()
{
    int i;
    delete ra->tHex;

    for (i = 0; i < RATEX; i++)
        freeImage(ra->tex[i]);
    delete ra->tex;

    for (i = 0; i < MAXTEXT; i++)
        freeImage(ra->uiText[i]);
    delete ra->uiText;

    for (i = 0; i < MAXNUMB; i++)
        freeImage(ra->bigNum[i]);
    delete ra->bigNum;

    for (i = 0; i < MAXSTAT; i++)
        freeImage(ra->status[i]);
    delete ra->status;

    for (i = 0; i < 6; i++)
        freeImage(ra->dice[i]);
    delete ra->dice;

    for (i = 0; i < MAXRATX; i++)
        freeImage(ra->descText[i]);
    delete ra->descText;

    for (i = 0; i < 2; i++)
        delete ra->pop[i];
    delete ra->pop;

    delete ra;
}

void drawActionRange(iPopup* pop, float dt, float rate)
{
    //loge("@drawActionRange");
    resetRGBA();
    int i, set;
    mapOffSet* off = getMapOffSet();
    Hexagon* hexs = getHexInfo();

    for ( i = 0; i < 6; i++ )
    {
        TRACKHEX* th = &ra->tHex[i];
        set = th->setTex;

        if (set == -1)      // 이동 불가
            continue;
        else if (set == 1)  // 아군 (이동 불가)
        {
            setRGBA(200, 0, 0, 200);
            set = 1;
        }
        else if (set == 2)  // 이동 가능
        {
            setRGBA(220, 220, 220, 200);
            set = 1;
        }

        drawImage(ra->tex[set], hexs[th->index].point.x + off->curr.x, hexs[th->index].point.y + off->curr.y, VCENTER | HCENTER);
        resetRGBA();
    }
}

void drawRegimentAction(float dt)
{
    resetRGBA();
    ra->pop[0]->paint(dt);
}

void showRegimentAction(bool show)
{
    ra->pop[0]->show(show);
}

bool keyRegimentAction(iKeyStat stat, iPoint point)
{
    switch (stat)
    {
        case iKeyStatBegan:
            break;

        case iKeyStatMoved:
            break;

        case iKeyStatEnded:
        {
            if ( ra->pop[0]->bShow ) // 이동 및 전투
            {
                Hexagon* hexs = getHexInfo();
                for ( int i = 0; i < 6; i++ )
                {
                    TRACKHEX* th = &ra->tHex[i];
                    if ( th->index != currentIndex ) continue;

                    if ( th->setTex == 2 ) // 이동 구현
                    {
#if 0//issue sound
                        playSfxSound(2);
                        JNIEnv* env = getEnv();
                        jclass cls = env->FindClass("java/lang/Thread");
                        jmethodID mid = env->GetStaticMethodID(cls, "sleep", "(J)V");
                        jlong sleepTime = 1000;
                        env->CallStaticVoidMethod(cls, mid, sleepTime);
                        env->DeleteLocalRef(cls);
#endif
                        setRegimentIndex( currentIndex, getRegimentIndex(_dc->focusIdx) );
                        setRegimentIndex( _dc->focusIdx, -1 );

                        RegimentInfo* rg = regimentAtIndex( getRegimentIndex(currentIndex) );
                        rg->ps = hexs[currentIndex].point;

                        if ( statVictory() )
                            rg->mv = 4;
                        else
                            rg->mv -= th->value;

                        rg->dr = 0;
                        for (int j = 0; j < rg->count; j++)
                        {
                            ArmyCounter* a = rg->army[j];

                            int bonus = hexs[currentIndex].defBonus;
                            if ( bonus )
                                a->dup = a->dp + bonus;
                            else
                                a->dup = 0;

                            rg->dr += (a->dup ? a->dup : a->dp);
                        }

                        _dc->focusIdx = currentIndex;
                        victoryCheck();
                        actionRangeTracking();
                    }
                    else if ( th->setTex == 0 ) // 전투시
                    {
                        _dc->focusRgIdx = getRegimentIndex( _dc->focusIdx );
                        _dc->currRgIdx = getRegimentIndex( currentIndex );
                        battleAttackerDecide();
                        battleRatioCalc();
                        ra->bFirst = true;

                        showBattleAction(true);
                        warscene = true;
                        RegimentInfo* rg = regimentAtIndex( _dc->focusRgIdx );

                        if ( statVictory() )
                            rg->mv = 4;
                        else
                            rg->mv -= th->value;
                        actionRangeTracking();
                    }
                }
            }
            break;
        }//case iKeyStatEnded:
    }

    return false;
}

void actionRangeTracking()
{
    //loge("@actionRangeTracking");
    int i, focus = _dc->focusIdx;
    for (i = 0; i < 6; i++)
    {
        TRACKHEX* th = &ra->tHex[i];
        th->index = -1;
        th->value = 0;
        th->setTex = -1;
    }

    Hexagon* h = getHexInfo();
    RegimentInfo* focusRg = regimentAtIndex( h[focus].regimentIdx );

    if ( focusRg->ally == false )
    {
        cmBmoveBtnOff();
        _dc->focusRgIdx = -1;
        _dc->focusIdx = -1;
        return;
    }
    for (i = 0; i < 6; i++)
    {
        int idx = h[focus].way[i];

        if ( fogStatus(idx) || h[idx].move == 0 )//전쟁안개 || 이동불가
            continue;

        TRACKHEX* th = &ra->tHex[i];
        th->index = idx;
        th->value = h[idx].move;

        if ( h[idx].regimentIdx == -1 )
        {
            if ( focusRg->mv < th->value )
                th->setTex = 1;
            else
                th->setTex = 2;
        }
        else
        {
            RegimentInfo* targetRg = regimentAtIndex( h[idx].regimentIdx );
            if ( focusRg->ally == targetRg->ally )
                th->setTex = 1;
            else //if ( focusRg->ally != targetRg->ally )
            {
                if ( focusRg->mv < th->value )
                    th->setTex = 1;
                else
                    th->setTex = 0;
            }
        }
    }
    //loge("tracking End");
}

void battleRatioCalc()
{
    //loge("@battleRatioCalc");
    RegimentInfo* att = _dc->rg[0];
    RegimentInfo* def = _dc->rg[1];
    if ( att->ar == 0 || def->dr == 0 )
    {
        ra->resultA = ra->resultD = 55;
        memset(ra->numA, 0, sizeof(int)*3);
        memset(ra->numD, 0, sizeof(int)*3);
        return;
    }

    ra->ratioA = (float)att->ar / (float)def->dr;
    ra->ratioD = (float)def->dr / (float)att->ar;
    //loge("%f %f", ra->ratioA, ra->ratioD);
    ra->ratioA > ra->ratioD ? ra->ratioD = 1 : ra->ratioA = 1;
    if ( ra->ratioA > ra->ratioD )
    {
        float divPos = ra->ratioA;
        ra->numA[0] = divPos;
        divPos = (divPos - ra->numA[0]) * 10;
        ra->numA[1] = divPos;
        divPos = (divPos - ra->numA[1]) * 10;
        ra->numA[2] = divPos;

        ra->numD[0] = 1;
        ra->numD[1] = 0;
        ra->numD[2] = 0;

        ra->ratioA = round(ra->ratioA * 100) / 100;
        ra->ratioD = 1;

        float temp = ceil(ra->ratioA);
        //loge("%f", temp);
        ra->resultA = (int)temp;
        ra->resultD = 1;
    }
    else if ( ra->ratioA < ra->ratioD )
    {
        ra->numA[0] = 1;
        ra->numA[1] = 0;
        ra->numA[2] = 0;

        float divPos = ra->ratioD;
        ra->numD[0] = divPos;
        divPos = (divPos - ra->numD[0]) * 10;
        ra->numD[1] = divPos;
        divPos = (divPos - ra->numD[1]) * 10;
        ra->numD[2] = divPos;

        ra->ratioA = 1;
        ra->ratioD = round(ra->ratioD * 100) / 100;

        float temp = ceil(ra->ratioD);
        //loge("%f", temp);
        ra->resultA = 1;
        ra->resultD = (int)temp;
    }
    else// if ( ra->ratioA == ra->ratioD )
    {
        ra->numA[0] = 1;
        ra->numA[1] = 0;
        ra->numA[2] = 0;

        ra->numD[0] = 1;
        ra->numD[1] = 0;
        ra->numD[2] = 0;

        ra->ratioA = 1;
        ra->ratioD = 1;

        ra->resultA = 1;
        ra->resultD = 1;
    }
    //loge("%f %f", ra->ratioA, ra->ratioD);
    //loge("%d %d", ra->resultA, ra->resultD);
}

void drawBattleInfo(float dt)
{
    //loge("@drawBattleInfo");
    int i, j;
    float offX[22];
    float battleLeft = _dc->battleW - 10;

    Texture** t = ra->uiText;
    for (i = 0; i < 2; i++)
    {
        if ( i == 0 )
        {
            offX[0] = 10;
            offX[1] = 15;
            offX[2] = 30;
            offX[3] = 80 + 150 + 30;
            offX[4] = offX[3] + t[2]->width / 2;
            offX[5] = offX[3];
            offX[6] = offX[3] - 10;
            offX[7] = offX[3] + 5;// total combat value
            offX[8] = _dc->hcenter - 30 - 70;
            offX[9] = offX[7] + 60;
            offX[10] = offX[9] + 80;
            offX[11] = offX[9] + 15;
            offX[12] = _dc->hcenter - t[10]->width - 50;//leadership box
            offX[13] = offX[12] - 10;//leadership text
            offX[14] = _dc->hcenter - 50 - 50;//leadership num
            offX[15] = offX[14] - 50 - 5;//leadership rank
            offX[16] = offX[7];// war dice
            offX[17] = offX[7] + 70;// +
            offX[18] = offX[17] + 40;// war bonus
            offX[19] = _dc->hcenter - t[13]->width;// war result
            offX[20] = _dc->hcenter - 70;
            offX[21] = _dc->hcenter - 150;
        }
        else
        {
            offX[0] = battleLeft - t[0]->width;
            offX[1] = battleLeft - t[1]->width + 5;
            offX[2] = battleLeft - t[1]->width + 20;
            offX[3] = battleLeft - t[0]->width - 150 - 20 - t[2]->width;
            offX[4] = battleLeft - t[0]->width - 150 - 20 - t[2]->width / 2;
            offX[5] = battleLeft - t[0]->width - 150 - 20 - t[4]->width * 0.4;
            offX[6] = offX[5] - 10;
            offX[7] = battleLeft - t[0]->width - 150 - 20 - 60 - 15;// total combat value
            offX[8] = _dc->hcenter;
            offX[9] = offX[7] - 112;
            offX[10] = offX[9] + 80;
            offX[11] = offX[9] + 15;
            offX[12] = _dc->hcenter + 50;//leadership box
            offX[13] = offX[12] + 90;//leadership text
            offX[14] = _dc->hcenter + 50;//leadership num
            offX[15] = offX[14] + 50 + 5;//leadership rank
            offX[16] = battleLeft - t[0]->width - 150 - 85 - 25;//war dice
            offX[17] = offX[16] - 55;// +
            offX[18] = offX[17] - 40;// war bonus
            offX[19] = _dc->hcenter;// war result
            offX[20] = offX[19];
            offX[21] = _dc->hcenter + 65 + 5;
        }

        // hero step title
        i == 0 ? setRGBA(255, 222, 222, 230) : setRGBA(223, 235, 255, 230);
        drawImage(t[0], offX[0], 50, TOP | LEFT);   // step title background
        drawImage(t[1], offX[0], 140, TOP | LEFT);  // stepbar background

        // hero current steps
        RegimentInfo* rg = _dc->rg[i];
        Texture* tNum = ra->bigNum[rg->gi->currStep];
        rg->gi->currStep == rg->gi->maxSteps ? setRGBA(0, 255, 0, 255) : setRGBA(255, 0, 0, 255);
        drawImage(tNum, offX[1], 150, TOP | LEFT);    // curr step value

        setLineWidth(20);
        setRGBA(219, 0, 0, 255);
        if ( rg->gi->currStep )
        {
            for (j = 0; j < rg->gi->maxSteps; j++)
            {
                if (j > rg->gi->currStep - 1)
                    setRGBA(50, 50, 50, 255);
                drawLine(offX[2], 210 + j * 24, offX[2] + 40, 220 + j * 24); // step bar
            }
        }
        setLineWidth(1);

        // hero name
        resetRGBA();
        drawImage(t[2 + rg->ally], offX[3], 10, TOP | LEFT);    // title background
        Texture* name = regimentText->heroNameTitle[rg->gi->idx];
        drawImage(name, offX[4], 10 + t[2]->height / 2, VCENTER | HCENTER,
                  0, 0, name->width, name->height, 2, 2, 2, 0);// hero name

        // total combat value
        i == 0 ? setRGBA(255, 216, 216, 230) : setRGBA(217, 229, 255, 230);
        drawImage(t[4], offX[5], 65, TOP | LEFT,
                  0, 0, t[4]->width, t[4]->height, 0.4, 0.4, 2, 0, i);
        drawImage(t[5 + i], offX[6], 70, TOP | LEFT);

        tNum = ra->bigNum[i == 0 ? rg->ar : rg->dr];
        setRGBA(255, 235, 0, 255);
        drawImage(tNum, offX[7], 130, TOP | LEFT);

        // battle ratio
        tNum = ra->bigNum[i == 0 ? ra->resultA : ra->resultD];
        int space = 0;
        if ( i == 0  && ra->resultA == 1 )
            space = 5;
        else if ( i == 1 && ra->resultD == 1)
            space = -5;

        setRGBA(255, 200, 0, 255);
        drawImage(tNum, offX[8] + space, 125, TOP | LEFT,
                  0, 0, tNum->width, tNum->height, 1.4, 1.4, 2, 0);

        tNum = ra->bigNum[52];//(
        drawImage(tNum, offX[9], 150, TOP | LEFT,
                  0, 0, tNum->width, tNum->height, 0.6, 0.6, 2, 0);
        tNum = ra->bigNum[53];//)
        drawImage(tNum, offX[10], 150, TOP | LEFT,
                  0, 0, tNum->width, tNum->height, 0.6, 0.6, 2, 0);

        for (j = 0; j < 3; j++)
        {
            int roundNumber = (i == 0 ? ra->numA[j] : ra->numD[j]);
            tNum = ra->bigNum[ra->THEEND ? 0 : roundNumber];
            int off = 0;
            if (j == 1) off = 2;
            drawImage(tNum, offX[11] + (j * 25) + off, 150, TOP | LEFT,
                      0, 0, tNum->width, tNum->height, 0.6, 0.6, 2, 0);
        }

        tNum = ra->bigNum[51];
        drawImage(tNum, offX[11] + 13, 164, TOP | LEFT,
                  0, 0, tNum->width, tNum->height, 0.6, 0.6, 2, 0);

        // @leadership box
        i == 0 ? setRGBA(255, 216, 216, 230) : setRGBA(217, 229, 255, 230);
        drawImage(t[10], offX[12], 200, TOP | LEFT);
        resetRGBA();
        drawImage(t[11], offX[13], 185, TOP | LEFT);
        Texture* stat = ra->status[rg->gi->leaderNum + 14];
        drawImage(stat, offX[14], 200, TOP | LEFT);
        stat = ra->status[rg->gi->leaderRank - 54];
        drawImage(stat, offX[15], 200, TOP | LEFT);

        // @war dice
        int rDiceNum = ra->resDice[i];
        Texture* d = ra->dice[rDiceNum];
        drawImage(d, offX[16], 260, TOP | LEFT);
        tNum = ra->bigNum[54];
        setRGBA(50, 50, 50, 255);
        drawImage(tNum, offX[17], 280, TOP | LEFT);//+

        int warBonus = (i == 0 ? ra->resultA : ra->resultD) + rg->gi->leaderNum;
        tNum = ra->bigNum[ra->THEEND ? 55 : warBonus];
        setRGBA(255, 200, 0, 255);
        drawImage(tNum, offX[18], 280, TOP | LEFT);//war bonus

        i == 0 ? setRGBA(255, 216, 216, 230) : setRGBA(217, 229, 255, 230);
        drawImage(t[13], offX[19], 260, TOP | LEFT);// war result box

        int diceNumber = ra->THEEND || ra->bFirst ? 0 : rDiceNum + 1 + warBonus;
        tNum = ra->bigNum[diceNumber];
        if ( diceNumber == 0 )           setRGBA(50, 50, 50, 255);
        else
        {
            if ( ra->battleRes == 0 )    setRGBA(255, 0, 127, 255);//비김
            //if ( ra->battleRes == 0 )    setRGBA(50, 50, 50, 255);//비김, 둘다 피해없음
            else
            {
                if ( i == 0 )
                    ra->battleRes == 1 ? setRGBA(255, 0, 127, 255) : setRGBA(50, 50, 50, 255);
                else
                    ra->battleRes == 2 ? setRGBA(255, 0, 127, 255) : setRGBA(50, 50, 50, 255);
            }
        }
        drawImage(tNum, offX[20], 270, TOP | LEFT,
                  0, 0, tNum->width, tNum->height, 1, 1.4, 2, 0);//result value

        // #magic box
        i == 0 ? setRGBA(255, 216, 216, 230) : setRGBA(217, 229, 255, 230);
        drawImage(t[10], offX[12], 355, TOP | LEFT);
        resetRGBA();
        drawImage(t[12], offX[13], 340, TOP | LEFT);
        stat = ra->status[rg->gi->magicNum];
        drawImage(stat, offX[14], 355, TOP | LEFT);
        int idx = rg->gi->magicRank == 83 ? 6 : rg->gi->magicRank - 58;
        stat = ra->status[idx];
        drawImage(stat, offX[15], 355, TOP | LEFT);

        // #magic dice
        rDiceNum = ra->resDice[i + 2];
        d = ra->dice[rDiceNum];
        drawImage(d, offX[16], 415, TOP | LEFT);

        i == 0 ? setRGBA(255, 216, 216, 230) : setRGBA(217, 229, 255, 230);
        drawImage(t[13], offX[19], 415, TOP | LEFT);// magic result box

        if ( ra->bFirst ) setRGBA(255, 200, 0, 255);// 0라운드
        else
        {
            if (i == 0)
                ra->magicA ? setRGBA(255, 0, 127, 255) : setRGBA(50, 50, 50, 255);
            else
                ra->magicD ? setRGBA(255, 0, 127, 255) : setRGBA(50, 50, 50, 255);
        }
        tNum = ra->bigNum[ra->bFirst ? rg->gi->magicNum : rDiceNum + 1];
        drawImage(tNum, offX[20], 425, TOP | LEFT,
                  0, 0, tNum->width, tNum->height, 1, 1.4, 2, 0);//result value

        resetRGBA();
        if ( ra->bFirst )
            drawImage(t[14], offX[21], 415, TOP | LEFT);
        else
        {
            if (i == 0)
                drawImage(t[ra->magicA ? 16 : 15], offX[21], 415, TOP | LEFT);
            else
                drawImage(t[ra->magicD ? 16 : 15], offX[21], 415, TOP | LEFT);
        }
    }// for (i = 0; i < 2; i++)
    //loge("battleInfo End");
    resetRGBA();
}

void drawBattleButton(float dt)
{
    //loge("@drawBattleButton");
    resetRGBA();
    Texture** t = ra->tex;

    if ( ra->bRolling )
        setRGBA(50, 50, 50, 255);

    drawImage(t[ra->bRetreat ? 4 : 3], devSize.width - t[3]->width - 18, 385, TOP | LEFT);
    drawImage(t[ra->THEEND ? 7 : 6], devSize.width - t[3]->width - 12, 390, TOP | LEFT);//7종료6후퇴

    if ( ra->THEEND )
        setRGBA(50, 50, 50, 255);
    drawImage(t[ra->bStart ? 4 : 3], devSize.width - t[3]->width - 18, 130, TOP | LEFT);
    drawImage(t[5], devSize.width - t[3]->width - 12, 135, TOP | LEFT);

    resetRGBA();
}

void drawBattleField(iPopup* pop, float dt, float rate)
{
    //loge("@drawBattle");
    drawImage(ra->tex[2], 0, 0, TOP | LEFT);//battle field

    for (int i = 0; i < 3; i++)
        drawImage(ra->uiText[7 + i], _dc->hcenter, 70 + (i * 65), TOP | HCENTER);

    drawWarCounters(dt);//DrawCounters.cpp
    drawBattleInfo(dt);

    drawBattleButton(dt);

    // 설명 박스
    setRGBA(178, 235, 244, 220);
    fillRect(260, 505, 495, 135, 20);
    if ( ra->resultTextIdx == -1 )
    {
        ra->bDescL ? setRGBA(250, 237, 125, 220) : setRGBA(92, 209, 229, 220);
        drawImage(ra->tex[8], 280, 505, TOP | LEFT);
        ra->bDescR ? setRGBA(250, 237, 125, 220) : setRGBA(92, 209, 229, 220);
        drawImage(ra->tex[8], 735, 505, TOP | RIGHT,
                  0, 0, ra->tex[8]->width, ra->tex[8]->height, 1, 1, 2, 0, REVERSE_WIDTH);
    }
    resetRGBA();
    setLineWidth(5);
    drawRect(260, 505, 495, 135, 20);
    // 설명 박스 내용
    if ( ra->resultTextIdx != -1 )
        drawImage(ra->descText[ra->resultTextIdx], 260, 505, TOP | LEFT);
    else
        drawImage(ra->descText[3 + ra->descTextIdx], 260, 505, TOP | LEFT);

    // 전투 계산
    battleDiceRolling(dt);

    setLineWidth(1);
}

void drawBattleAction(float dt)
{
    ra->pop[1]->paint(dt);
    if ( ra->pop[1]->bShow == false )
    {
        showPopVic( victory );
        ra->THEEND = false;// 배틀UI가 완전히 꺼질때 까지 배틀 데이터 유지
        ra->resultTextIdx = -1;
        ra->descTextIdx = 0;
    }
}

void showBattleAction(bool show)
{
    ra->pop[1]->show(show);
}

void battleAttackerDecide() // RegimentAction.cpp
{
    //loge("@battleAttackerDecide");
    for (int i = 0; i < 2; i++)
    {
        RegimentInfo* rg = regimentAtIndex( i == 0 ? _dc->focusRgIdx : _dc->currRgIdx );
        _dc->rg[i] = rg;
    }
}

static float diceDt = 0.0f;
#define _diceDt 0.3f
void battleDiceRolling(float dt) // 출품 : 라운드마다 모든 전투 자동
{
    if ( ra->bRolling == false )
        return;
    //loge("@battleDiceRolling");
    for (int i = 0; i < 4; i++)
        ra->resDice[i] = rand() % 6;

    diceDt += dt;
    if ( diceDt > _diceDt)
    {
        RegimentInfo* atk = _dc->rg[0];
        RegimentInfo* def = _dc->rg[1];
        int atkBonus = ra->resultA + atk->gi->leaderNum;
        int defBonus = ra->resultD + def->gi->leaderNum;

        int atkRes = ra->resDice[0] + 1 + atkBonus;
        int defRes = ra->resDice[1] + 1 + defBonus;

        // 통솔 등급이 높은 쪽이 있다면 높은 쪽은 즉시 유닛에 데미지를 줄 수 있다.
        if ( atk->gi->leaderRank != def->gi->leaderRank )// 같은 등급이 아닐때 적용
        {
            if ( atk->gi->leaderRank < def->gi->leaderRank)// A > B > C char가 크면 패배
                ra->battleHit = 1;   // 공격자 승
            else if ( atk->gi->leaderRank > def->gi->leaderRank)
                ra->battleHit = 2;   // 방어자 승
        }
        else
            ra->battleHit = 0;

        // 전투 비교
        if ( atkRes == defRes )// 비기는 경우
        {   // 통솔 등급 비교
            if ( atk->gi->leaderRank == def->gi->leaderRank )
            {   // 통솔 등급도 같으면 주사위 높은 쪽이 승리
                if ( ra->resDice[0] == ra->resDice[1] ) // 완전히 비기면 둘 다 피해
                {
                    ra->battleRes = 0;
                    atk->gi->currStep -= 1;
                    def->gi->currStep -= 1;
                }
                else if ( ra->resDice[0] > ra->resDice[1] )
                {
                    ra->battleRes = 1;  // 공격자 승
                    def->gi->currStep -= 1;
                }
                else// if ( ra->resDice[0] < ra->resDice[1] )
                {
                    ra->battleRes = 2;  // 방어자 승
                    atk->gi->currStep -= 1;
                }
            }
            else if ( atk->gi->leaderRank < def->gi->leaderRank )// A > B > c char가 크면 패배
            {
                ra->battleRes = 1;  // 공격자 승
                def->gi->currStep -= 1;
            }
            else// if ( atk->gi->leaderRank > def->gi->leaderRank )
            {
                ra->battleRes = 2;  // 방어자 승
                atk->gi->currStep -= 1;
            }
        }
        else if ( atkRes > defRes )
        {
            ra->battleRes = 1;      // 공격자 승
            def->gi->currStep -= 1;
        }
        else// if ( atkRes < defRes )
        {
            ra->battleRes = 2;      // 방어자 승
            atk->gi->currStep -= 1;
        }

        if ( ra->battleRes == 0 )       // 무승부 둘다 피해
        {
            if (atk->gi->currStep < 0)
            {
                atk->gi->currStep = 0;
                battleRegimentStepReduce(atk, true);
            }
            if (def->gi->currStep < 0)
            {
                def->gi->currStep = 0;
                battleRegimentStepReduce(def, false);
            }
        }
        else if ( ra->battleRes == 1)   // 공격자 승
        {
            if (def->gi->currStep < 0)
            {
                def->gi->currStep = 0;
                battleRegimentStepReduce(def, false);
            }
            if ( ra->battleHit == 1 )//등급 승
                battleRegimentStepReduce(def, false);
        }
        else if ( ra->battleRes == 2 )  // 방어자 승
        {
            if (atk->gi->currStep < 0)
            {
                atk->gi->currStep = 0;
                battleRegimentStepReduce(atk, false);
            }
            if ( ra->battleHit == 2 )//등급 승
                battleRegimentStepReduce(atk, false);
        }

        // 마법 등급이 높은 쪽이 있다면 높은 쪽은 즉시 유닛에 데미지를 줄 수 있다.
        if ( atk->gi->magicRank != def->gi->magicRank )// 같은 등급이 아닐때 적용
        {   // S 등급이 최고 등급
            if ( atk->gi->magicRank == 's')
                ra->magicHit = 1;   // 공격자 승
            else if ( def->gi->magicRank == 's')
                ra->magicHit = 2;   // 방어자 승

            if ( atk->gi->magicRank < def->gi->magicRank)// A > B > C > D char가 크면 패배
                ra->magicHit = 1;   // 공격자 승
            else if ( atk->gi->magicRank > def->gi->magicRank)
                ra->magicHit = 2;   // 방어자 승
        }
        else
            ra->magicHit = 0;

        // 마법 굴림
        if ( atk->gi->magicNum < ra->resDice[2] + 1 )
            ra->magicA = false;
        else // 공격측 마법 성공
        {
            ra->magicA = true;
            def->gi->currStep -= 1;
            if ( def->gi->currStep < 0 )
            {
                def->gi->currStep = 0;
                battleRegimentStepReduce(def, false);
            }
            if ( ra->magicHit == 1 )//등급 승
                battleRegimentStepReduce(def, false);
        }

        if ( def->gi->magicNum < ra->resDice[3] + 1 )
            ra->magicD = false;
        else // 방어측 마법 성공
        {
            ra->magicD = true;
            atk->gi->currStep -= 1;
            if ( atk->gi->currStep < 0 )
            {
                atk->gi->currStep = 0;
                battleRegimentStepReduce(atk, true);
            }
            if ( ra->magicHit == 2 )
                battleRegimentStepReduce(atk, true);
        }

        ra->bFirst = false;
        ra->bRolling = false;
        diceDt = 0.0f;
        regimentElimination();
    }
    //loge("@battleDiceRolling End");
}

void battleRegimentStepReduce(RegimentInfo* rg, bool attack)// 0방어자 1공격자
{
    //loge("@battleRegimentStepReduce %d", attack);
    if (rg->count == 0)
        return;

    rg->ar = 0; rg->dr = 0;
    int bonus;
    Hexagon* h = getHexInfo();
    if ( attack )
        bonus = h[_dc->focusIdx].defBonus;
    else
        bonus = h[currentIndex].defBonus;

    ArmyCounter* unit = rg->army[rg->count - 1];

    unit->stepCurr -= 1;
    unit->type += 1;

    if ( unit->stepCurr <= 0)
    {
        unit->stepCurr = 0;
        unit->type =99;
        unit->ap = 0;
        unit->dp = 0;
        unit->aup = 0;
        unit->dup = 0;
    }
    else
    {
        int strArrIdx[3] = {};
        switch (unit->stepNum)//step에 따른 사용 이미지/공방 결정
        {
            case 3: strArrIdx[0] = 4; strArrIdx[1] = 2;  break;
            case 2: strArrIdx[0] = 2;  break;
            case 1: strArrIdx[0] = 0;  break;
        }
        unit->ap = unit->strength[strArrIdx[unit->stepCurr - 1]];
        unit->dp = unit->strength[strArrIdx[unit->stepCurr - 1] + 1];
        if (bonus)
            unit->dup = unit->dp + bonus;
    }

    for (int i = 0; i < rg->count; i++)
    {
        ArmyCounter* unit = rg->army[i];
        int aa, dd;
        unit->aup == 0 ? aa = unit->ap : aa = unit->aup;
        rg->ar += aa;
        unit->dup == 0 ? dd = unit->dp : dd = unit->dup;
        rg->dr += dd;
    }

    int destroy = 0;
    for (int i = 0; i < rg->count; i++)
    {
        if ( rg->army[i]->stepCurr == 0)
            destroy++;
    }
    rg->count -= destroy;
    if ( rg->count < 0)
        rg->count = 0;

    battleRatioCalc();
    //loge("battleRegimentStepReduce End %d", attack);
}

void regimentElimination()
{
    //loge("@regimentElimination");
    RegimentInfo* atk = _dc->rg[0];
    RegimentInfo* def = _dc->rg[1];

    bool deathAtk = false;
    bool deathDef = false;

    if ( atk->count == 0) // 현재는 플레이어뿐
    {
        deathAtk = true;
        setRegimentIndex(_dc->focusIdx, -1);
        cmBmoveBtnOff();
        showCommander(false);
        _dc->focusRgIdx = -1;
    }

    if ( def->count == 0 )
    {
        deathDef = true;
        setRegimentIndex(currentIndex, -1);
        _dc->currRgIdx = -1;

        if ( deathAtk == false )
        {
            Hexagon* hexs = getHexInfo();
            setRegimentIndex( currentIndex, getRegimentIndex(_dc->focusIdx) );
            setRegimentIndex( _dc->focusIdx, -1 );

            RegimentInfo* rg = regimentAtIndex( getRegimentIndex(currentIndex) );
            rg->ps = hexs[currentIndex].point;

            rg->dr = 0;
            for (int j = 0; j < rg->count; j++)
            {
                ArmyCounter* a = rg->army[j];

                int bonus = hexs[currentIndex].defBonus;
                if ( bonus )
                    a->dup = a->dp + bonus;
                else
                    a->dup = 0;

                rg->dr += (a->dup ? a->dup : a->dp);
            }

            _dc->focusIdx = currentIndex;
            victoryCheck();
            actionRangeTracking();
        }
    }
    if ( deathAtk )
    {
        currentIndex = _dc->focusIdx;
        _dc->focusIdx = -1;
    }

    if ( deathAtk || deathDef )
        ra->THEEND = true;

    if ( deathAtk && deathDef)  ra->resultTextIdx = 0;
    else if ( deathAtk )        ra->resultTextIdx = 1;
    else if ( deathDef )        ra->resultTextIdx = 2;
    else                        ra->resultTextIdx = -1;
}

bool keyBattleAction(iKeyStat stat, iPoint point)
{
    if ( ra->pop[1]->bShow == false )
        return false;

    if ( ra->bRolling )
        return true;

    Texture** t = ra->tex;
    iRect rt;
    switch (stat)
    {
        case iKeyStatBegan:
        {
            rt = iRectMake(devSize.width - t[3]->width - 18, 385, t[3]->width, t[3]->height);
            if ( containPoint(point , rt) )
                ra->bRetreat = true;

            if ( ra->THEEND ) break;
            rt = iRectMake(devSize.width - t[3]->width - 18, 130, t[3]->width, t[3]->height);
            if ( containPoint(point , rt) )
                ra->bStart = true;

            //iRectMake(260, 505, 495, 135);
            rt = iRectMake(260, 505, 247, 135);
            if ( containPoint(point, rt) )
                ra->bDescL = true;
            rt = iRectMake(507, 505, 248, 135);
            if ( containPoint(point, rt) )
                ra->bDescR = true;
            break;
        }

        case iKeyStatMoved:
        {
            rt = iRectMake(devSize.width - t[3]->width - 18, 385, t[3]->width, t[3]->height);
            if ( !containPoint(point , rt) )
                ra->bRetreat = false;

            if ( ra->THEEND ) break;
            rt = iRectMake(devSize.width - t[3]->width - 18, 130, t[3]->width, t[3]->height);
            if ( !containPoint(point , rt) )
                ra->bStart = false;

            rt = iRectMake(260, 505, 247, 135);
            if ( !containPoint(point, rt) )
                ra->bDescL = false;
            rt = iRectMake(507, 505, 248, 135);
            if ( !containPoint(point, rt) )
                ra->bDescR = false;
            break;
        }

        case iKeyStatEnded:
        {
            if ( ra->bStart )
                ra->bRolling = true;

            if ( ra->bRetreat )
            {
                warscene = false;
                showBattleAction(false);
                //showRegimentAction(true);
            }

            if ( ra->bDescL )
            {
                if ( --ra->descTextIdx < 0 ) ra->descTextIdx = 5;
                ra->bDescL = false;
            }
            else if ( ra->bDescR )
            {
                if ( ++ra->descTextIdx > 5 ) ra->descTextIdx = 0;
                ra->bDescR = false;
            }

            ra->bStart = false;
            ra->bRetreat = false;
            break;
        }
    }

    return true;
}

bool battleUIAction()
{
    return ra->pop[1]->bShow;
}

bool actRegimentAction()
{
    return ra->pop[0]->bShow;
}

