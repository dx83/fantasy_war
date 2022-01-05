#include "iPoint.h"

#include "iStd.h"

bool iPoint::operator == (const iPoint& p)
{
	return x == p.x && y == p.y;	// 0 또는 1 반환
}

bool iPoint::operator != (const iPoint& p)
{
	return x != p.x || y != p.y;	// 0 또는 1 반환
}

iPoint iPoint::operator + (const iPoint& p)
{
	iPoint r;
	r.x = x + p.x;
	r.y = y + p.y;
	return r;
}

iPoint iPoint::operator - (const iPoint& p)
{
	iPoint r;
	r.x = x - p.x;
	r.y = y - p.y;
	return r;
}

iPoint iPoint::operator * (const iPoint& p)
{
	iPoint r;
	r.x = x * p.x;
	r.y = y * p.y;
	return r;
}

iPoint iPoint::operator / (const iPoint& p)
{
	iPoint r;
	r.x = x / p.x;
	r.y = y / p.y;
	return r;
}

iPoint& iPoint::operator += (const iPoint& p)
{
	x += p.x;
	y += p.y;
	return *this;
}

iPoint& iPoint::operator -= (const iPoint& p)
{
	x -= p.x;
	y -= p.y;
	return *this;
}

iPoint& iPoint::operator *= (const iPoint& p)
{
	x *= p.x;
	y *= p.y;
	return *this;
}

iPoint& iPoint::operator /= (const iPoint& p)
{
	x /= p.x;
	y /= p.y;
	return *this;
}


iPoint iPoint::operator + (const float s)
{
	iPoint r;
	r.x = x + s;
	r.y = y + s;
	return r;
}

iPoint iPoint::operator - (const float s)
{
	iPoint r;
	r.x = x - s;
	r.y = y - s;
	return r;
}

iPoint iPoint::operator * (const float s)
{
	iPoint r;
	r.x = x * s;
	r.y = y * s;
	return r;
}

iPoint iPoint::operator / (const float s)
{
	iPoint r;
	r.x = x / s;
	r.y = y / s;
	return r;
}


iPoint& iPoint::operator += (const float s)
{
	x += s;
	y += s;
	return *this;
}

iPoint& iPoint::operator -= (const float s)
{
	x -= s;
	y -= s;
	return *this;
}

iPoint& iPoint::operator *= (const float s)
{
	x *= s;
	y *= s;
	return *this;
}

iPoint& iPoint::operator /= (const float s)
{
	x /= s;
	y /= s;
	return *this;
}

iPoint iPointMake(float x, float y)
{
	iPoint p;
	p.x = x;
	p.y = y;
	return p;
}

float _cos(float degree)
{	// degree * M_PI / 180 => 연산속도
	return cos(degree / 180 * M_PI);	// 내장함수 cos(radian)
}

float _sin(float degree)
{	// degree * M_PI / 180 => 연산속도
	return sin(degree / 180 * M_PI);	// 내장함수 sin(radian)
}

float iPointAngle(const iPoint& v1, const iPoint& v2, const iPoint& v3)
{
#if 1// 제 2 코사인법칙(a2 = b2 + c2 - 2bc cosA)
	// v13^2 = v12^2 + v23^2 - 2 * v12 * v23 * cosA
	// A = acos( (b^2 + c^2 - a^2) / (2ab) );
	float a2 = (v1.x - v3.x) * (v1.x - v3.x) + (v1.y - v3.y) * (v1.y - v3.y);
	float b2 = (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y);
	float c2 = (v2.x - v3.x) * (v2.x - v3.x) + (v2.y - v3.y) * (v2.y - v3.y);
	float degree = acos((b2 + c2 - a2) / sqrtf(4 * b2 * c2)) * 180 / M_PI;
	if (v3.y > v1.y)
		degree = 360 - degree;
	return degree;
#else// 내적(aXb = |a||b|cosA)
	iPoint a = iPointMake(v1.x - v2.x, v1.y - v2.y);
	iPoint b = iPointMake(v3.x - v2.x, v3.y - v2.y);
	float degree = acos((a.x * b.x + a.y * b.y) / sqrtf((a.x * a.x + a.y * a.y) * (b.x * b.x + b.y * b.y)))
		* 180 / M_PI;
	if (v3.y > v1.y)
		degree = 360 - degree;
	return degree;
#endif
}

iPoint iPointRotate(iPoint p, iPoint t, float degree)
{
	while (degree < 0)	degree += 360;
	while (degree > 0)	degree -= 360;
	float sinr = _sin(degree);
	float cosr = _cos(degree);
	/* 회전 공식
	iPoint r;
	// 시계 방향, 기본 공식
	r.x = t.x + (p.x - t.x) * cosr - (p.y - t.y) * sinr;
	r.y = t.y + (p.x - t.x) * sinr + (p.y - t.y) * cosr;
	// 반시계 방향, sinr -p.x/-p.y
	r.x = t.x + (p.x - t.x) * cosr - (-p.y + t.y) * sinr;
	r.y = t.y + (- p.x + t.x) * sinr + (p.y - t.y) * cosr;
	*/
	//#define CLOCKWISE
#ifdef CLOCKWISE	// 시계방향 회전
	iPoint r = iPointMake(p.x - t.x, p.y - t.y);
#else				// 반시계방향 회전, -p.y
	iPoint r = iPointMake(p.x - t.x, t.y - p.y);
#endif
	iPoint _p;
	_p.x = r.x * cosr - r.y * sinr;
	_p.y = r.x * sinr + r.y * cosr;

	_p.x = t.x + _p.x;
#ifdef CLOCKWISE	// 시계방향 회전
	_p.y = t.y + _p.y;
#else				// 반시계방향 회전, -_p.y
	_p.y = t.y - _p.y;
#endif
	return _p;
}
// 벡터 크기(두 점 사이의 길이), 한쪽을 (0, 0) 기준으로 봄
float  iPointLength(iPoint p)
{	// 수학 표기법 |p|
	return sqrt(p.x * p.x + p.y * p.y);
}
// 단위 벡터
iPoint iPointVector(iPoint p)
{
	float len = iPointLength(p);	// p의 길이
	if (len == 0.0f)				// 길이가 0이면
		return iPointZero;			// 원점 반환
	return p / len;	// 각 속성을 길이로 나누면 단위 벡터가 됨
}
// 벡터 내적
float iPointDot(iPoint p0, iPoint p1)
{
	/*벡터 a = (a1, a2), b = (b1, b2)
		a·b = |a||b|cosθ = a1b1 + a2b2
	*/
	return p0.x * p1.x + p0.y * p1.y;	// a·b
}
// 선분(sp<->ep)과 한점(p)의 최단거리
float distanceFromLine(iPoint p, iPoint sp, iPoint ep)
{
	iPoint b = ep - sp;				// 선분(sp<->ep) b 의 벡터
	float len = iPointLength(b);	// b의 길이(벡터 크기)
	b /= len;						// b의 단위 벡터

	iPoint a = p - sp;				// sp<->p의 가상의 선분 a 의 벡터
	/*
		|a|cosθ = 선분 b에서 최단거리에 해당하는 지점까지의 길이
		a·b = |a||b|cosθ, |a|cosθ = a·b / |b|
		현재 b를 단위 벡터 n 이라고 했을때, |n|은 1
		a·n = |a||n|cosθ, a·n = |a|cosθ
	*/
	float d = iPointDot(a, b);// a와 b(단위벡터)를 내적하면 = |a|cosθ
	/*
		d < 0.0f : p점은 sp 보다 뒤쪽에 있음
		d > len  : p점이 ep 보다 앞쪽에 있음
	*/
	iPoint proj = b * (d < 0.0f ? 0.0f : d > len ? len : d);

	return iPointLength(a - proj);	// 한점 p에서 선분 b까지의 최단거리
}


float iPointDistance(iPoint p0, iPoint p1)
{
	iPoint p;
	p.x = p0.x - p1.x;
	p.y = p0.y - p1.y;
	return sqrt(p.x * p.x + p.y * p.y);
}

float iPointDistance2(iPoint p0, iPoint p1)
{
	iPoint p;
	p.x = p0.x - p1.x;
	p.y = p0.y - p1.y;
	return p.x * p.x + p.y * p.y;
}

float iPointCross(iPoint p0, iPoint p1, iPoint p2)
{
	return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

float linear(float rate, float start, float end)
{
	return start + (end - start) * rate;
}

iPoint linear(float rate, iPoint start, iPoint end)
{
	return start + (end - start) * rate;
}

float easeIn(float rate, float start, float end)
{	//return start + (end-start) * _sin(90*_sin(90*rate))
	return start + (end - start) * _sin(90 * rate);
}

iPoint easeIn(float rate, iPoint start, iPoint end)
{
	return start + (end - start) * _sin(90 * rate);
}

float easeOut(float rate, float start, float end)
{	//return start + (end-start) * rate * rate * rate;
	return start + (end - start) * rate * rate;
}

iPoint easeOut(float rate, iPoint start, iPoint end)
{	// iPoint의 경우 소수점들끼리 먼저 계산하는 것이 속도가 빠름
	return start + (end - start) * (rate * rate);
}

