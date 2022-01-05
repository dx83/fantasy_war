#pragma once

#include "iOpenGL.h"

class iFBO
{
public:
	iFBO(int width, int height);
	virtual ~iFBO();

	static void clear(float r, float g, float b, float a);
	void bind();
	void bind(Texture* tex);
	void unbind();

	void read(int x, int y, int width, int height, uint32* pixels);

public:
	GLuint fbo;
	GLuint depthBuffer;

	Texture* tex;	// GLuint colorBuffer

	Texture** texStack;
	int stackNum;
};

extern iFBO* fbo;

