#pragma once

#include "iType.h"

#include "iPoint.h"
#include "iColor.h"
#include "iOpenGL.h"

struct Vertex
{
	iPoint coord;	// float coord[2];
	iColor4f color;	//float color[4];
	iPoint st;		// float st[2];	texture coordinate
};

struct Quad
{
	Vertex tl;
	Vertex tr;
	Vertex bl;
	Vertex br;
};

class iVBO
{
public:
	iVBO();
	virtual ~iVBO();

	void clean();
	void set(Texture* tex, int max);
	void paint();

public:
	Quad* quads;
	int quadsNum, quadsMax;

	GLuint quadsID;
	//GLushort* indices;
	GLuint elementsID;

	Texture* tex;
	iPoint position;
	//GLuint blendSrc, blendDst;
	GLBlend blend;
};

extern iVBO* vbo;
extern Texture* texEgg;

struct Egg
{
	iPoint p;
	iPoint v;
	float life, _life;
	iColor4f cStart, cEnd;
	float sStart, sEnd;

	bool paint(float dt);
};

#define _quadMax 100000

class iParticleSystem
{
public:
	iParticleSystem(int quadMax = _quadMax);
	virtual ~iParticleSystem();

	void paint(float dt);

public:
	Egg* _egg;
	Egg** egg;
	int eggNum;

	float createDt, _createDt;
	float life, lifeVar;

	iPoint position, positionVar;
	float move, moveVar;
	float degree, degreeVar;
	iColor4b cStart, cStartVar;
	iColor4b cEnd, cEndVar;
	float sStart, sStartVar;
	float sEnd, sEndVar;
};

Texture* createParticle();

