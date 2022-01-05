#include "Test.h"

#include "iStd.h"

void loadTest()
{
}

void freeTest()
{
}

void drawTest(float dt)
{
    clearRect(0.5f, 0.5f, 0.5f, 1.0f);
    resetRGBA();

#if 0
    // #exit quit
        JNIEnv* env = getEnv();
        jclass cls = env->FindClass("java/lang/System");
        jmethodID mid = env->GetStaticMethodID(cls, "exit", "(I)V");
        env->CallStaticVoidMethod(cls, mid, 0);
        env->DeleteLocalRef(cls);
#endif
}

void keyTest(iKeyStat stat, iPoint point)
{
    switch (stat)
    {
    case iKeyStatBegan:
        //printf("Test began(%f,%f)\n", point.x, point.y);
        //setLoading(gs_intro, freeTest, loadIntro, 2);
        break;

    case iKeyStatMoved:
        break;
    
    case iKeyStatEnded:
        break;
    }
}

