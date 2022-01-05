#include "native-lib.h"

#include "iNDK.h"

#include "Game.h"
#include "iStd.h"

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_loadGame(JNIEnv* env, jobject obj,
                                        jobject apkFilePath, jobject ioPath)
{
#if XPRINT
    xprint("loadGame");
#endif
    //jstringPrint((jstring)ioPath);
    setDevicePath((jstring)ioPath);

    devSize.width = DEV_WIDTH;
    devSize.height = DEV_HEIGHT;

    initRGBA();
    //iGraphics::share()->start(hdc);
    loadOpenGL();

    appInitialize();
    keyBufInit();
    loadGame();
}

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_freeGame(JNIEnv* env, jobject obj)
{
#if XPRINT
    xprint("freeGame");
#endif
    freeRGBA();
}

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_drawGame(JNIEnv* env, jobject obj)
{
    //xprint("drawGame");
    mainLoop();
}

iPoint coordinate(int x, int y);
extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_keyGame(JNIEnv* env, jobject obj,
                                        jint state, jfloat x, jfloat y)
{
    //xprint("keyGame(%d, %d)", x, y);
    keyGame((iKeyStat)state, coordinate(x, y));
}

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_multiKeyGame(JNIEnv* env, jobject obj,
                                   jint state, jint num, jobject mx, jobject my)
{
#if XPRINT
    xprint("multiKeyGame");
#endif
}

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_resizeGame(JNIEnv* env, jobject obj,
                                        jint width, jint height)
{
    resizeApp(width, height);
    readyOpenGL();
    //mainLoop();//안드로이드에는 있을 필요 없음
}

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_pauseGame(JNIEnv* env, jobject obj,
                                          jboolean pause)
{
#if XPRINT
    xprint(pause==0 ? "pause" : "resume");
#endif
    //env->GetObjectClass("");
    //env->GetStaticObjectField();
}

iPoint coordinate(int x, int y)
{
    float r = devSize.width / viewport.size.width;
    //float r = devSize.height / viewport.size.height;
    iPoint p;
    p.x = (x - viewport.origin.x) * r;
    p.y = (y - viewport.origin.y) * r;

    return p;
}

void resizeApp(int width, int height)
{
#if XPRINT
    xprint("resizeGame %d %d", width, height);
#endif
    float rx = width / devSize.width;
    float ry = height / devSize.height;
    rateWidth = rx, rateHeight = ry;
    if (rx < ry)
    {
        viewport.origin.x = 0;
        viewport.size.width = width;

        viewport.size.height = devSize.height * rx;
        viewport.origin.y = (height - viewport.size.height) / 2;
    }
    else
    {
        viewport.origin.y = 0;
        viewport.size.height = height;

        viewport.size.width = devSize.width * ry;
        viewport.origin.x = (width - viewport.size.width) / 2;
    }
#if XPRINT
    xprint("한글 devSize(%.f,%.f), real(%d, %d), viewport(%.f,%.f,%.f,%.f)\n",
            devSize.width, devSize.height,
            width, height,
            viewport.origin.x, viewport.origin.y, viewport.size.width, viewport.size.height);
#endif
}

void drawApp(FLOAT_METHOD methodDraw)
{
#if 0
    int f = iFPS::instance()->framesPerSec;
#endif
    float dt = iFPS::instance()->update();

#if 1
    if (dt > 0.1f)
        dt = 0.1f;//dt = 0.001f;
    //xprintf("drawGame %f %d\n", dt, f);
#endif// 브레이크포인트/스팀오버레이/전화받을때/인게임로딩 의 경우
    // 게임이 멈추지만 dt는 계속 흐르므로 delta time을 멈춤

    // binding fbo
    methodDraw(dt);// drawGame

#if (OS==OS_WINDOWS)
    keyDown = 0;
    // 원래는 App.cpp 56 라인에 있어야 함.
    resetRGBA();
    extern void drawCursor(float dt);
    drawCursor(iFPS::instance()->lastDt);
#endif

    // unbind fbo
    // draw fbo
}

void mainLoop()
{
#if 0//None-FBO
    readyOpenGL();
    drawApp(drawGame);
    //SwapBuffers(hDC);
#elif 1
    //readyOpenGL();
fbo->bind();
    glViewport(0, 0, devSize.width, devSize.height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 컬러 버퍼 | 렌더(뎁스) 버퍼 를 비움
    drawApp(drawGame);
    // 원래는 여기 있는게 맞음
    //drawCursor(iFPS::instance()->lastDt);
fbo->unbind();

    glViewport(viewport.origin.x, viewport.origin.y,
               viewport.size.width, viewport.size.height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 컬러 버퍼 | 렌더(뎁스) 버퍼 를 비움

    setGLBlend(GLBlendMultiplied);  //FBO 사용시
    resetRGBA();

    iPoint p = iPointZero;
    float s = 1;//updateZoom(iFPS::instance()->lastDt, p);

    //drawImage(fbo->tex, 0, 0, TOP | LEFT);
    Texture* t = fbo->tex;
    drawImage(t, p.x, p.y, TOP | LEFT,
              0, 0, t->width, t->height, s, s, 2, 0, REVERSE_HEIGHT);   // 주화면
    //drawImage(t, devSize.width, devSize.height, BOTTOM | RIGHT,
    //    0, 0, t->width, t->height, 0.3f, 0.3f, 2, 0, REVERSE_HEIGHT);   // 일종의 미니맵 구현
    setGLBlend(GLBlendAlpha);       // 다시 되돌리기
    // 위의 명령을 받는대로 화면에 갱신 | 위의 명령을 받아 모두 그려진후 화면에 갱신
    // glFlush();// glFinish(); // PFD_DOUBLEBUFFER 를 사용하면 없어도 됨
#if (OS==OS_WINDOWS)
    SwapBuffers(hDC);
#endif

#endif
}

