#pragma once

#include "iGraphics.h"		// 헤더 파일 위치 중요

#if (OS==OS_WINDOWS)
#include <GL/glew.h>		// gl.h, glu.h 보다 먼저 정의해야함
#include <GL/gl.h>			/* OpenGL header file */
#include <GL/glu.h>			/* OpenGL utilities header file */
#include <gl/wglew.h>
#pragma comment(lib, "OpenGL32.lib")

void loadOpenGL(HDC hdc);

#elif (OS==OS_ANDROID)
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

void loadOpenGL();
#endif


void freeOpenGL();

enum TextureWrap
{
	CLAMP = 0,
	REPEAT,
};

enum TextureFilter
{
	NEAREST = 0,
	LINEAR,
	MIPMAP,
};

void getTexture(TextureWrap& wrap, TextureFilter& minFilter, TextureFilter& magFilter);

void setTexture();
void setTexture(TextureWrap wrap, TextureFilter filter);
void setTexture(TextureWrap wrap, TextureFilter minFilter, TextureFilter magFilter);
void setTexture(GLuint texID, TextureWrap wrap, TextureFilter filter);
void setTexture(GLuint texID, TextureWrap wrap, TextureFilter minFilter, TextureFilter magFilter);

void readyOpenGL();

//typedef enum _GLBlend
enum GLBlend
{
	GLBlendAlpha = 0,
	GLBlendLight,

	GLBlendMultiplied,
	//GLBlendNum	// 총개수 자동화
	//GLBlendAlpha2 = GLBlendAlpha,	// 다시 0 부터 시작
	//GLBlendLight2 = GLBlendLight
};
//} GLBlend, GLBlend2, GLBlend3, GLBlend4;

void setGLBlend(GLBlend blend);
GLBlend getGLBlend();

// ========================================================
// iVertex
// ========================================================
#define vertex_w_max 8
#define vertex_h_max 8
struct VertexInfo;
struct iMatrix;

extern iMatrix *matrixProj, *matrixView;

struct iVertex
{
	iVertex();
	~iVertex();

	static GLuint getProgram(const char* strVert, const char* strFrag);
	// select
	GLuint useProgram(const char* strVert, const char* strFrag);
	// send
	void bufferSubData(VertexInfo* data, int w, int h);
	// attr
	GLuint enableVertexAttrArray(const char* str, const void* src, int stride, int size, int type);
	void disableVertexAttrArray(GLuint attr);
    // uniform
	void setUniformMat(const char* str, iMatrix* m);
	void setUniform1f(const char* str, float x);
	void setUniform2f(const char* str, float x, float y);
	void setUniform1i(const char* str, int x);
	
	void drawElements();

	int convert(VertexInfo* vtx, int w = 1, int h = 1);
	VertexInfo* vi;
	int viNum;

	GLuint id;

    GLuint vao;
    GLuint vbo;
    GLuint vbe;
};
extern iVertex* vtx;

#include "iColor.h"
#include "iPoint.h"
struct VertexInfo
{
	float position[4];
	iColor4f color;
	iPoint uv;//st
};

// ========================================================
// shader
// ========================================================
// Complile
#define shader_vert 0
#define shader_frag 1
void checkShaderID(GLuint id);
GLuint createShaderID(const char* str, GLuint flag);
void freeShaderID(GLuint id);

// Link
void checkProgramID(GLuint id);
GLuint createProgramID(GLuint vertID, GLuint fragID);
void freeProgramID(GLuint id);

