#include "iFBO.h"

#include "iStd.h"

iFBO* fbo = NULL;

iFBO::iFBO(int width, int height)
{
	// renderBuffer - depth
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
#if (OS==OS_WINDOWS)
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
#elif (OS==OS_ANDROID)
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
#endif
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// colorBuffer - FBO texture, istd
	tex = createTexture(width, height);
	GLuint texID = tex->texID;

	// framebuffer - fbo
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#if (OS==OS_WINDOWS)
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	GLenum fboBuffs[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, fboBuffs);
#endif
	// ANDROID에서는 GL_COLOR_ATTACHMENT1, 2, 3, ... 없음
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
#if (OS==OS_WINDOWS)
		printf("glCheckFramebufferStatus %d, %d\n", GL_FRAMEBUFFER_COMPLETE, status);
#elif (OS==OS_ANDROID)
		//loge("glCheckFramebufferStatus %d, %d\n", GL_FRAMEBUFFER_COMPLETE, status);
#endif
	clear(0, 0, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	texStack = new Texture*[10];
	stackNum = 0;
}

iFBO::~iFBO()
{	// 해제할때는 역순으로
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteRenderbuffers(1, &depthBuffer);
	freeImage(tex);
	glDeleteFramebuffers(1, &fbo);
}

void iFBO::clear(float r, float g, float b, float a)	// static
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void iFBO::bind()
{
	bind(tex);
}

void iFBO::bind(Texture* tex)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#if (OS==OS_WINDOWS)
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	GLenum fboBuffs[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, fboBuffs);
#endif
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->texID, 0);

	glViewport(0, 0, tex->width, tex->height);
	matrixProj->loadIdentity();
	matrixProj->ortho(0, tex->width, tex->height, 0, -1, 1);
	matrixView->loadIdentity();

	texStack[stackNum] = tex;
	stackNum++;
}

void iFBO::unbind()
{
	stackNum--;
	if (stackNum)
	{
		stackNum--;
		bind(texStack[stackNum]);
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(viewport.origin.x, viewport.origin.y, viewport.size.width, viewport.size.height);
	matrixProj->loadIdentity();
	matrixProj->ortho(0, devSize.width, devSize.height, 0, -1, 1);
	matrixView->loadIdentity();
}

void iFBO::read(int x, int y, int width, int height, uint32* pixels)
{
#if (OS==OS_WINDOWS)
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels);
#elif (OS==OS_ANDROID)
	glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
#endif
}

