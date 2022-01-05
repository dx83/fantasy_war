#pragma once

struct iPoint
{
	float x, y;

	bool operator == (const iPoint& p);
	bool operator != (const iPoint& p);

	// 매개 변수	: iPoint 구조체의 연산을 위해 iPoint 주소를 매개변수로 사용
	// 반환형	: 계산 결과 값만 반환하면 되므로 &(주소연산자) 사용안함
	iPoint operator + (const iPoint& p);
	iPoint operator - (const iPoint& p);
	iPoint operator * (const iPoint& p);
	iPoint operator / (const iPoint& p);
	// 반환형	: 계산 결과값을 대입해야 하므로 &(주소연산자) 사용
	iPoint& operator += (const iPoint& p);
	iPoint& operator -= (const iPoint& p);
	iPoint& operator *= (const iPoint& p);
	iPoint& operator /= (const iPoint& p);

	// 매개 변수	: float 값을 받아서 연산을 진행하기 위해 float 값을 매개변수로 사용
	iPoint operator + (const float s);
	iPoint operator - (const float s);
	iPoint operator * (const float s);
	iPoint operator / (const float s);

	iPoint& operator += (const float s);
	iPoint& operator -= (const float s);
	iPoint& operator *= (const float s);
	iPoint& operator /= (const float s);
};


#define iPointZero iPointMake(0, 0)
#define iPointOne iPointMake(1, 1)
iPoint iPointMake(float x, float y);

float _cos(float degree);
float _sin(float degree);

float iPointAngle(const iPoint& v1, const iPoint& v2, const iPoint& v3);
iPoint iPointRotate(iPoint p, iPoint t, float degree);

float iPointLength(iPoint p);
iPoint iPointVector(iPoint p);
float iPointDot(iPoint p0, iPoint p1);
float iPointCross(iPoint p0, iPoint p1, iPoint p2);

float distanceFromLine(iPoint p, iPoint sp, iPoint ep);

float iPointDistance(iPoint p0, iPoint p1);
float iPointDistance2(iPoint p0, iPoint p1);

// 50, 100		rate == 0, 50		rate == 1, 100
// 100, 50		rate == 0, 100		rate == 1, 50
// -100, 50		rate == 0, -100		rate == 1, 50
// 게임에 매우 자주 쓰임 (선형 보간)	ex) 게이지바
float linear(float rate, float start, float end);
iPoint linear(float rate, iPoint start, iPoint end);

float easeIn(float rate, float start, float end);
iPoint easeIn(float rate, iPoint start, iPoint end);

float easeOut(float rate, float start, float end);
iPoint easeOut(float rate, iPoint start, iPoint end);

