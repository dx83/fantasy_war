#include <jni.h>>

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_loadGame(JNIEnv* env, jobject obj,
                                        jobject apkFilePath, jobject ioPath);

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_freeGame(JNIEnv* env, jobject obj);

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_drawGame(JNIEnv* env, jobject obj);

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_keyGame(JNIEnv* env, jobject obj,
                                        jint state, jfloat x, jfloat y);

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_multiKeyGame(JNIEnv* env, jobject obj,
                                        jint state, jint num, jobject mx, jobject my);

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_resizeGame(JNIEnv* env, jobject obj,
                                        jint width, jint height);

extern "C" JNIEXPORT void JNICALL
Java_win_gatewalker_lib_Native_pauseGame(JNIEnv* env, jobject obj,
                                        jboolean pause);

void resizeApp(int width, int height);
void mainLoop();

