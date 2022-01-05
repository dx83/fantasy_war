#include "iNDK.h"

#include "native-lib.h"
#include "iStd.h"

JavaVM* g_vm;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    // 함수리스트 동적할당
    g_vm = vm;
    //extern jint JNI_OnLoad_Android(JavaVM* vm, void* reserved);
    //JNI_OnLoad_Android(vm, reserved);

    JNIEnv* env;
    jint r = vm->GetEnv((void**)&env, JNI_VERSION_1_4);

    int num = 7;
    JNINativeMethod* method = new JNINativeMethod[num];

    /*
    B=byte C=char Z=boolean S=short I=int [I=int[]
    */
    JNINativeMethod* m = &method[0];
    m->name = "loadGame";// public static native void loadGame(String apkPath, String ioPath);
    m->signature = "(Ljava/lang/String;Ljava/lang/String;)V";
    m->fnPtr = (void*)Java_win_gatewalker_lib_Native_loadGame;
    m = &method[1];
    m->name = "freeGame";// public static native void freeGame();
    m->signature = "()V";
    m->fnPtr = (void*)Java_win_gatewalker_lib_Native_freeGame;
    m = &method[2];
    m->name = "drawGame";// public static native void drawGame();
    m->signature = "()V";
    m->fnPtr = (void*)Java_win_gatewalker_lib_Native_drawGame;
    m = &method[3];
    m->name = "keyGame";// public static native void keyGame(int state, float x, float y);
    m->signature = "(IFF)V";
    m->fnPtr = (void*)Java_win_gatewalker_lib_Native_keyGame;
    m = &method[4];
    m->name = "multiKeyGame";// public static native void multiKeyGame(int state, int num, float[] x, float[] y);
    m->signature = "(II[F[F)V";
    m->fnPtr = (void*)Java_win_gatewalker_lib_Native_multiKeyGame;
    m = &method[5];
    m->name = "resizeGame";// public static native void resizeGame(int width, int height);
    m->signature = "(II)V";
    m->fnPtr = (void*)Java_win_gatewalker_lib_Native_resizeGame;
    m = &method[6];
    m->name = "pauseGame";// public static native void pauseGame(boolean pause);
    m->signature = "(Z)V";
    m->fnPtr = (void*)Java_win_gatewalker_lib_Native_pauseGame;

    jclass cls = env->FindClass("win/gatewalker/lib/Native");
    r = env->RegisterNatives(cls, method, num);
    env->DeleteLocalRef(cls);

    delete method;
#if XPRINT
    xprint("JNI_Onload");
#endif
    return JNI_VERSION_1_4;
}


void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    xprint("JNI_OnUnload");
    // 함수리스트 동적해제
    JNIEnv* env;
    jint r = vm->GetEnv((void**)&env, JNI_VERSION_1_4);
    jclass cls = env->FindClass("win/gatewalker/lib/Native");

    env->UnregisterNatives(cls);
    env->DeleteLocalRef(cls);

    vm->DetachCurrentThread();
}

JNIEnv* getEnv()
{
    JNIEnv* env;
    g_vm->AttachCurrentThread(&env, NULL);
    return env;
}

jobject getMain()
{
    static jobject app = NULL;
    if( app==NULL )
    {
        JNIEnv* env = getEnv();
        jclass cls = env->FindClass("win/gatewalker/game/App");
        jfieldID fid = env->GetStaticFieldID(cls, "app", "Lwin/gatewalker/game/App;");
        app = env->GetStaticObjectField(cls, fid);
        env->DeleteLocalRef(cls);
    }

    return app;
}

jobject ndkLocal2global(jobject obj)
{
    JNIEnv* env = getEnv();
    jobject o = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    return o;//env->DeleteGlobalRef(o) 이거아니면 절대 지워지지 않음
}

// --------------------------------------------------------
// 문자열
// --------------------------------------------------------
char* jbyteArray2cstr(JNIEnv* env, jbyteArray javaBytes)
{
    int len = env->GetArrayLength(javaBytes);
    jbyte* nativeBytes = env->GetByteArrayElements(javaBytes, NULL);// 원본을 그대로 사용
    char* str = new char[len+1];
    memcpy(str, nativeBytes, len);
    str[len] = 0;
    env->ReleaseByteArrayElements(javaBytes, nativeBytes, JNI_ABORT);

    return str;
}

char* jstring2cstr(JNIEnv* env, jstring jstr)
{
    // byte[] b = jstr.getBytes();----------
    jclass cls = env->FindClass("java/lang/String");
    jmethodID mid = env->GetMethodID(cls, "getBytes", "()[B");
    jbyteArray javaBytes = (jbyteArray)env->CallObjectMethod(jstr, mid);
    env->DeleteLocalRef(cls);
    // -------------------------------------

    return jbyteArray2cstr(env, javaBytes);
}

jbyteArray cstr2byteArray(JNIEnv* env, const char* cstr)
{
    int len = strlen(cstr);
    jbyteArray javaBytes = env->NewByteArray(len);
    env->SetByteArrayRegion(javaBytes, 0, len, (jbyte*)cstr);

    return javaBytes;
}

jstring javaNewStringEncoding(JNIEnv* env, jbyteArray javaBytes, const char* encoding)
{
    // new String(javaBytes, "UTF-8")
    jclass cls = env->FindClass("java/lang/String");
    jmethodID mid = env->GetMethodID(cls, "<init>", "([BLjava/lang/String;)V");//<init> 생성자
    jstring jEncoding = env->NewStringUTF(encoding);
    jstring jstr = (jstring)env->NewObject(cls, mid, javaBytes, jEncoding);
    env->DeleteLocalRef(cls);
    env->DeleteLocalRef(jEncoding);

    return jstr;
}

jstring javaNewStringChar(JNIEnv* env, const char* cstr)
{
    jbyteArray byteArray = cstr2byteArray(env, cstr);
    jstring jstr = javaNewStringEncoding(env, byteArray, "UTF-8");//"KSC5601"
    env->DeleteLocalRef(byteArray);

    return jstr;
}

jint jstringLength(JNIEnv* env, jstring str)
{
    // return str.length;
    jclass cls = env->FindClass("java/lang/String");
    jmethodID mid = env->GetMethodID(cls, "length", "()I");
    int length = env->CallIntMethod(str, mid);
    env->DeleteLocalRef(cls);

    return length;
}

jstring jstringSubString(JNIEnv* env, jstring str, int start, int end)
{
    jclass cls = env->FindClass("java/lang/String");
    jmethodID mid = env->GetMethodID(cls, "substring", "(II)Ljava/lang/String;");
    jstring jstr = (jstring)env->CallObjectMethod(str, mid, start, end);
    env->DeleteLocalRef(cls);

    return jstr;
}

void jstringPrint(jstring jstr)
{
    JNIEnv* env = getEnv();

    static jstring tag = NULL;
    if( tag==NULL )
    {
        jstring t = javaNewStringChar(env, "ssm");
#if 0
        tag = (jstring)env->NewGlobalRef(t);
        env->DeleteLocalRef(t);
#else
        tag = (jstring)ndkLocal2global(t);
#endif
    }

    jclass cls = env->FindClass("android/util/Log");
    jmethodID mid = env->GetStaticMethodID(cls, "e", "(Ljava/lang/String;Ljava/lang/String;)I");
    env->CallStaticIntMethod(cls, mid, tag, jstr);
    env->DeleteLocalRef(cls);
}

// --------------------------------------------------------
// FILE IO
// --------------------------------------------------------
char* getStream(const char* fileName, int& length)
{
    static AAssetManager* am = NULL;
    if( am==NULL )
    {
        JNIEnv* env = getEnv();

        //App.app
        jclass cls = env->FindClass("win/gatewalker/game/App");
        jfieldID fid = env->GetStaticFieldID(cls, "app", "Lwin/gatewalker/game/App;");
        jobject app = env->GetStaticObjectField(cls, fid);

        // am = App.app.getAssets();
        jmethodID mid = env->GetMethodID(cls, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject jam = env->CallObjectMethod(app, mid);

        env->DeleteLocalRef(cls);
        env->DeleteLocalRef(app);

        am = AAssetManager_fromJava(env, jam);
        env->DeleteLocalRef(jam);
    }

    AAsset* asset = AAssetManager_open(am, fileName, AASSET_MODE_UNKNOWN);

    if( asset == 0 )
        return NULL;
    length = AAsset_getLength(asset);
    char* buf = new char[length + 1];
    AAsset_read(asset, buf, length);
    buf[length] = 0;
    AAsset_close(asset);

    return buf;
}

static char* strDevicePath = NULL;
void setDevicePath(jstring path)
{
    // strDevicePath from path
    if( strDevicePath )
        delete strDevicePath;
    strDevicePath = jstring2cstr(getEnv(), path);
}
const char* getDevicePath()
{
    return strDevicePath;
}

// --------------------------------------------------------
// to do
// --------------------------------------------------------
jobject ndkCreateBitmap(JNIEnv* env, int width, int height, const char* config)
{
    //------------------------------------------------------------------------
    // config = Bitmap.Config.ARGB_8888; // "ARGB_8888"
    //------------------------------------------------------------------------
    jclass cls = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID mid = env->GetStaticMethodID(cls, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jstring string = javaNewStringChar(env, config);
    jobject conf = env->CallStaticObjectMethod(cls, mid, string);
    env->DeleteLocalRef(string);
    env->DeleteLocalRef(cls);

    //------------------------------------------------------------------------
    // Bitmap b = Bitmap.createBitmap(width, height, config);
    //------------------------------------------------------------------------
    cls = env->FindClass("android/graphics/Bitmap");
    mid = env->GetStaticMethodID(cls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(cls, mid, width, height, conf);
    env->DeleteLocalRef(conf);
    env->DeleteLocalRef(cls);

    return bitmap;
}

// Bitmap bmp = Bitmap.createScaledBitmap(bitmap, width, height, true);//스스로 만드셈
// 디바이스 저장소에 있는 이미지(논 압축)
jobject ndkCreateBitmap(const char* fileName)// 주용도 : 로컬
{
    JNIEnv* env = getEnv();
    // -----------------------------
    // Bitmap bitmap = BitmapFactory.decodeFile(string);
    // -----------------------------
    jclass cls = env->FindClass("android/graphics/BitmapFactory");
    jmethodID mid = env->GetStaticMethodID(cls, "decodeFile", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    jstring string = javaNewStringChar(env, fileName);// path
    jobject bitmap = env->CallStaticObjectMethod(cls, mid, string);
    env->DeleteLocalRef(string);
    env->DeleteLocalRef(cls);

    return bitmap;
}

jobject ndkCreateBitmap(char* buf, int length)// 주용도 : 네트워크
{
    JNIEnv* env = getEnv();
    // ---------------------------------------------------------------------
    // Bitmap bitmap = BitmapFactory.decodeByteArray(byte[] buf, 0, length);
    // ---------------------------------------------------------------------
    jbyteArray javaBytes = env->NewByteArray(length);//java code : byte[] javaBytes = new byte[length];
    env->SetByteArrayRegion(javaBytes, 0, length, (jbyte*)buf);
    size_t len = env->GetArrayLength(javaBytes);
    jclass cls = env->FindClass("android/graphics/BitmapFactory");
    jmethodID mid = env->GetStaticMethodID(cls, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(cls, mid, javaBytes, 0, (int)len);
    env->DeleteLocalRef(javaBytes);
    env->DeleteLocalRef(cls);

    return bitmap;
}

Texture* createImage(const char* path)
{
    JNIEnv* env = getEnv();

#if 0
    int length;
    char* buf = getStream(path, length);
    jobject bitmap = ndkCreateBitmap(buf, length);
    delete buf;
#else
    jobject bitmap = ndkCreateBitmap(path);
#endif
    Texture* tex = ndkCreateImage(bitmap);
    ndkFreeBitmap(bitmap);
    env->DeleteLocalRef(bitmap);

    return tex;
}

void ndkFreeBitmap(jobject bitmap)
{
    JNIEnv* env = getEnv();
    //------------------
    // bitmap.recycle();
    //------------------
    jclass cls = env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(cls, "recycle", "()V");
    env->CallVoidMethod(bitmap, mid);
    env->DeleteLocalRef(cls);
}

Texture* ndkCreateImage(jobject bitmap)
{
    JNIEnv* env = getEnv();
    // ----------------------------------------------------
    // int pixels = new int[potW * potH];
    // bitmap.getPixels(pixels, 0, potW, 0, 0, potW, potH);
    // ----------------------------------------------------
    jclass cls = env->FindClass("android/graphics/Bitmap");
    iSize size = ndkGetBitmapSize(bitmap);
    int potWidth = nextPot(size.width);
    int potHeight = nextPot(size.height);
    jintArray jPixels = env->NewIntArray(potWidth*potHeight);
    jmethodID mid = env->GetMethodID(cls, "getPixels", "([IIIIIII)V");
    env->CallVoidMethod(bitmap, mid, jPixels, 0, potWidth, 0, 0, (jint)size.width, (jint)size.height);
    env->DeleteLocalRef(cls);

    // --------------------------
    // jPixels ==> pixels => rgba
    // --------------------------
    jint* pixels = env->GetIntArrayElements(jPixels, NULL);   // new
    uint8* rgba = new uint8[potWidth * potHeight * 4];
    int i, j = potWidth * potHeight;
    for( int i = 0; i < j; i++ )
    {
        rgba[4*i+3] = (uint8)((pixels[i] & 0xff000000) >> 24);  // a
        rgba[4*i+0] = (uint8)((pixels[i] & 0x00ff0000) >> 16);  // r
        rgba[4*i+1] = (uint8)((pixels[i] & 0x0000ff00) >> 8);   // g
        rgba[4*i+2] = (uint8)((pixels[i] & 0x000000ff));// >> 0 // b
    }
    env->ReleaseIntArrayElements(jPixels, pixels, JNI_ABORT);       // delete
    env->DeleteLocalRef(jPixels);

    Texture* tex = createImageWithRGBA(rgba, size.width, size.height);
    delete rgba;

    return tex;
}

Texture* ndkCreateImage(const char* fileName)
{
#if 0// 디바이스 저장소에 있는 이미지(논 압축)
    jobject bitmap = ndkCreateBitmap(fileName);
#else// files in apk
    int length;
    char* buf = getStream(fileName, length);
    if( buf==NULL )
        return NULL;
    jobject bitmap = ndkCreateBitmap(buf, length);
    delete buf;
#endif
    Texture* tex = ndkCreateImage(bitmap);
    JNIEnv* env = getEnv();
    ndkFreeBitmap(bitmap);
    env->DeleteLocalRef(bitmap);
    return tex;
}

#include "iStd.h"
#include "iFPS.h"
#include "iOpenAL.h"

Texture** ndkCreateImageDevide(int numX, int numY, const char* fileName)
{
    jobject bitmap = ndkCreateBitmap(fileName);

    JNIEnv* env = getEnv();
    // ----------------------------------------------------
    // int pixels = new int[potW * potH];
    // bitmap.getPixels(pixels, 0, potW, 0, 0, potW, potH);
    // ----------------------------------------------------
    jclass cls = env->FindClass("android/graphics/Bitmap");
    iSize size = ndkGetBitmapSize(bitmap);
    int potWidth = nextPot(size.width);
    int potHeight = nextPot(size.height);
    jintArray jPixels = env->NewIntArray(potWidth*potHeight);
    jmethodID mid = env->GetMethodID(cls, "getPixels", "([IIIIIII)V");
    env->CallVoidMethod(bitmap, mid, jPixels, 0, potWidth, 0, 0, (jint)size.width, (jint)size.height);
    env->DeleteLocalRef(cls);

    ndkFreeBitmap(bitmap);
    env->DeleteLocalRef(bitmap);

    // --------------------------
    // jPixels ==> pixels => rgba
    // --------------------------
    jint* pixels = env->GetIntArrayElements(jPixels, NULL);   // new
    uint8* rgba = new uint8[potWidth * potHeight * 4];
    int i, j = potWidth * potHeight;
    for( int i = 0; i < j; i++ )
    {
        rgba[4*i+3] = (uint8)((pixels[i] & 0xff000000) >> 24);  // a
        rgba[4*i+0] = (uint8)((pixels[i] & 0x00ff0000) >> 16);  // r
        rgba[4*i+1] = (uint8)((pixels[i] & 0x0000ff00) >> 8);   // g
        rgba[4*i+2] = (uint8)((pixels[i] & 0x000000ff));// >> 0 // b
    }
    env->ReleaseIntArrayElements(jPixels, pixels, JNI_ABORT);       // delete
    env->DeleteLocalRef(jPixels);

    // ------------
    // rgba => texs
    // ------------
    Texture** texs = new Texture * [numX * numY];
    int w = ((int)size.width) / numX;
    int h = ((int)size.height) / numY;
    int potW = nextPot(w);
    int potH = nextPot(h);
    uint32* pixel = new uint32[potW * potH];
    for (int j = 0; j < numY; j++)
    {
        for (int i = 0; i < numX; i++)
        {
            copyBmpToRgba((uint32*)rgba, potWidth, w * i, h * j, w, h, pixel, w, h);
            texs[numX * j + i] = createImageWithRGBA((uint8*)pixel, w, h);
        }
    }
    delete pixel;

    return texs;
}

iSize ndkGetBitmapSize(jobject bitmap)
{
    JNIEnv* env = getEnv();
    //--------------------------------------------------
    // iSizeMake(bitmap.getWidth(), bitmap.getHeight());
    //--------------------------------------------------
    jclass cls = env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(cls, "getWidth", "()I");
    int width = env->CallIntMethod(bitmap, mid);
    mid = env->GetMethodID(cls, "getHeight", "()I");
    int height = env->CallIntMethod(bitmap, mid);
    env->DeleteLocalRef(cls);

    return iSizeMake(width, height);
}

void ndkDrawBitmap(jobject canvas, jobject paint, jobject bitmap, float x, float y, float width, float height, int anc)
{
    iSize size = ndkGetBitmapSize(bitmap);

    switch( anc )
    {
        case TOP|LEFT:                                                      break;
        case TOP|RIGHT:         x -= size.width;                            break;
        case TOP|HCENTER:       x -= size.width/2;                          break;

        case BOTTOM|LEFT:                           y -= size.height;       break;
        case BOTTOM|RIGHT:      x -= size.width;    y -= size.height;       break;
        case BOTTOM|HCENTER:    x -= size.width/2;  y -= size.height;       break;

        case VCENTER|LEFT:                          y -= size.height/2;     break;
        case VCENTER|RIGHT:     x -= size.width;    y -= size.height/2;     break;
        case VCENTER|HCENTER:   x -= size.width/2;  y -= size.height/2;     break;
    }
    // ---------------------------------------------------------------------
    // Rect src = new Rect(0, 0, 0+bitmap.getWidth(), 0+bitmap.getHeight());//원본
    // Rect dst = new Rect(x, y, x+bitmap.getWidth(), y+bitmap.getHeight());//그릴부분
    // ---------------------------------------------------------------------
    jobject src = ndkCreateRect(0, 0, size.width, size.height);
    jobject dst = ndkCreateRect(x, y, x+width, y+height);

    JNIEnv* env = getEnv();
    // -------------------------------------------
    // canvas.drawBitmap(bitmap, src, dst, paint);
    // -------------------------------------------
    jclass cls = env->FindClass("android/graphics/Canvas");
    jmethodID mid = env->GetMethodID(cls, "drawBitmap", "(Landroid/graphics/Bitmap;Landroid/graphics/Rect;Landroid/graphics/Rect;Landroid/graphics/Paint;)V");
    env->CallVoidMethod(canvas, mid, bitmap, src, dst, paint);
    env->DeleteLocalRef(cls);

    env->DeleteLocalRef(src);
    env->DeleteLocalRef(dst);
}

jobject ndkNewCanvas(JNIEnv* env, jobject bitmap)
{
    //--------------------------------
    // Canvas canvas = new Canvas();
    //--------------------------------
    jclass cls = env->FindClass("android/graphics/Canvas");
    jmethodID mid = env->GetMethodID(cls, "<init>", "()V");
    jobject canvas = env->NewObject(cls, mid);

    //--------------------------------
    // canvas.setBitmap(bitmap);
    //--------------------------------
    mid = env->GetMethodID(cls, "setBitmap", "(Landroid/graphics/Bitmap;)V");
    env->CallVoidMethod(canvas, mid, bitmap);

    env->DeleteLocalRef(cls);

    return canvas;
}

jobject ndkCreateTypeface(const char* fileName)
{
    JNIEnv* env = getEnv();
    jobject typeface;

    if( memcmp(fileName, "assets/", 6) )
    {
        // typeface = Typeface.SERIF;
        jclass cls = env->FindClass("android/graphics/Typeface");
        jfieldID fid = env->GetStaticFieldID(cls, "SERIF", "Landroid/graphics/Typeface;");
        typeface = env->GetStaticObjectField(cls, fid);
        env->DeleteLocalRef(cls);
    }
    else
    {
        //App.app
        jclass cls = env->FindClass("win/gatewalker/game/App");
        jfieldID fid = env->GetStaticFieldID(cls, "app", "Lwin/gatewalker/game/App;");
        jobject app = env->GetStaticObjectField(cls, fid);
        // am = App.app.getAssets();
        jmethodID mid = env->GetMethodID(cls, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject assets = env->CallObjectMethod(app, mid);

        jstring otf = javaNewStringChar(env, &fileName[7]);

        cls = env->FindClass("android/graphics/Typeface");
        mid = env->GetStaticMethodID(cls, "createFromAsset", "(Landroid/content/res/AssetManager;Ljava/lang/String;)Landroid/graphics/Typeface;");
        typeface = env->CallStaticObjectMethod(cls, mid, assets, otf);
        env->DeleteLocalRef(cls);

        env->DeleteLocalRef(assets);
        env->DeleteLocalRef(otf);
    }
    return typeface;
}

struct Typeface
{
    jobject typeface;
    char* fileName;
};

iArray* arrayTypeface = NULL;
void freeTypeface(void* parm)
{
    Typeface* tf = (Typeface*)parm;
    JNIEnv* env = getEnv();
    env->DeleteLocalRef(tf->typeface);
    delete tf->fileName;
    delete tf;
}

jobject getTypeface(const char* fileName)
{
    if( arrayTypeface==NULL )
        arrayTypeface = new iArray(freeTypeface);

    for( int i=0; i < arrayTypeface->count; i++ )
    {
        Typeface* tf = (Typeface*)arrayTypeface->objectAtIndex(i);
        if( strcmp(fileName, tf->fileName)==0 )
            return tf->typeface;
    }

    Typeface* tf = new Typeface;
    tf->typeface = ndkLocal2global( ndkCreateTypeface(fileName) );
    char* s = new char[strlen(fileName)];
    strcpy(s, fileName);
    tf->fileName = s;
    arrayTypeface->addObject(tf);

    return tf->typeface;
}
void ndkSetStringName(jobject paint, const char* fileName)
{
    jobject typeface = getTypeface(fileName);
    JNIEnv* env = getEnv();

    // paint.setTypeface(typeface);
    jclass cls = env->FindClass("android/graphics/Paint");
    jmethodID mid = env->GetMethodID(cls, "setTypeface", "(Landroid/graphics/Typeface;)Landroid/graphics/Typeface;");
    jobject tmp = env->CallObjectMethod(paint, mid, typeface);

    env->DeleteLocalRef(tmp);
    env->DeleteLocalRef(cls);
}

jobject ndkNewPaint(JNIEnv* env)
{
    //------------------------------------
    //Paint paint = new Paint();
    //------------------------------------
    jclass cls = env->FindClass("android/graphics/Paint");
    jmethodID mid = env->GetMethodID(cls, "<init>", "()V");
    jobject paint = env->NewObject(cls, mid);
    env->DeleteLocalRef(cls);

    return paint;
}

void ndkDrawARGB(jobject canvas, int a, int r, int g, int b)
{
    //---------------------------------
    // canvas.drawARGB(0, 0, 0, 0);
    //---------------------------------
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Canvas");
    jmethodID mid = env->GetMethodID(cls, "drawARGB", "(IIII)V");
    env->CallVoidMethod(canvas, mid, a, r, g, b);
    env->DeleteLocalRef(cls);
}

void ndkSetAntialias(jobject paint, bool anti)
{
    //---------------------------------
    // paint.setAntiAlias(aa);
    //---------------------------------
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Paint");
    jmethodID mid = env->GetMethodID(cls, "setAntiAlias", "(Z)V");
    env->CallVoidMethod(paint, mid, (jboolean)anti);
    env->DeleteLocalRef(cls);
}

void ndkSetTextSize(jobject paint, float stringSize)
{
    //---------------------------------
    // paint.setTextSize(ss);
    //---------------------------------
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Paint");
    jmethodID mid = env->GetMethodID(cls, "setTextSize", "(F)V");
    env->CallVoidMethod(paint, mid, (jfloat)stringSize);
    env->DeleteLocalRef(cls);
}

void ndkSetTextScaleX(jobject paint, float scaleX)
{
    //---------------------------------
    // paint.setTextScaleX(sx);
    //---------------------------------
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Paint");
    jmethodID mid = env->GetMethodID(cls, "setTextScaleX", "(F)V");
    env->CallVoidMethod(paint, mid, (jfloat)scaleX);
    env->DeleteLocalRef(cls);
}

void ndkSetARGB(jobject paint, int a, int r, int g, int b)
{
    //---------------------------------
    // paint.setARGB(a, r, g, b);
    //---------------------------------
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Paint");
    jmethodID mid = env->GetMethodID(cls, "setARGB", "(IIII)V");
    env->CallVoidMethod(paint, mid, a, r, g, b);
    env->DeleteLocalRef(cls);
}

void ndkSetStrokeWidth(jobject paint, float lineWidth)
{
    //---------------------------------
    // paint.setStrokeWidth(lineWidth);
    //---------------------------------
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Paint");
    jmethodID mid = env->GetMethodID(cls, "setStrokeWidth", "(F)V");
    env->CallVoidMethod(paint, mid, (jfloat)lineWidth);
    env->DeleteLocalRef(cls);
}

void ndkDrawLine(jobject canvas, jobject paint, float x0, float y0, float x1, float y1)
{
    //----------------------------------------
    // canvas.drawLine(sx, sy, ex, ey, paint);
    //----------------------------------------
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Canvas");
    jmethodID mid = env->GetMethodID(cls, "drawLine", "(FFFFLandroid/graphics/Paint;)V");
    env->CallVoidMethod(paint, mid, x0, y0, x1, y1, paint);
    env->DeleteLocalRef(cls);

}

void ndkSetPaintStyle(jobject paint, const char* strType)// STROKE, FILL
{
    JNIEnv* env = getEnv();
    //----------------------------------------
    // Paint.Style style = Paint.Style.valueOf("STROKE");
    //----------------------------------------
    jclass cls = env->FindClass("android/graphics/Paint$Style");
    jmethodID mid = env->GetStaticMethodID(cls, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Paint$Style;");

    jstring string = javaNewStringChar(env, strType);
    jobject style = env->CallStaticObjectMethod(cls, mid, string);
    env->DeleteLocalRef(string);

    env->DeleteLocalRef(cls);

    //----------------------------------------
    // paint.setStyle(Paint.Style.STROKE);
    //----------------------------------------
    cls = env->FindClass("android/graphics/Paint");
    mid = env->GetMethodID(cls, "setStyle", "(Landroid/graphics/Paint$Style;)V");
    env->CallVoidMethod(paint, mid, style);

    env->DeleteLocalRef(style);
    env->DeleteLocalRef(cls);
}

void ndkDrawRect(jobject canvas, jobject paint, float x, float y, float width, float height, float radius)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Canvas");

    if( radius==0.0f )
    {
        //--------------------------------------------------------------------------
        // canvas.drawRect(x, y, x + width, y + height, paint);
        //--------------------------------------------------------------------------

        jmethodID mid = env->GetMethodID(cls, "drawRect", "(FFFFLandroid/graphics/Paint;)V");
        env->CallVoidMethod(canvas, mid, x, y, x+width, y+height, paint);
    }
    else
    {
        //--------------------------------------------------------------------------
        // canvas.drawRoundRect(x, y, x + width, y + height, radius, radius, paint);
        //--------------------------------------------------------------------------
        jmethodID mid = env->GetMethodID(cls, "drawRoundRect", "(FFFFFFLandroid/graphics/Paint;)V");
        env->CallVoidMethod(canvas, mid, x, y, x+width, y+height, radius, radius, paint);
    }

    env->DeleteLocalRef(cls);
}

void ndkDrawText(jobject canvas, jstring s, float x, float y, jobject paint)
{
    //-----------------------------------
    // canvas.drawText(s, x, y, paint);
    //-----------------------------------
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("android/graphics/Canvas");
    jmethodID mid = env->GetMethodID(cls, "drawText", "(Ljava/lang/String;FFLandroid/graphics/Paint;)V");
    env->CallVoidMethod(canvas, mid, s, x, y, paint);
    env->DeleteLocalRef(cls);
}

//String sss = "Hello World";
//float width = paint.measureText(sss);

iRect ndkGetRect(jobject rt)
{
    JNIEnv* env = getEnv();
    // -------------------------
    // int left = rect.left;
    // int right = rect.right;
    // int top = rect.top;
    // int bottom = rect.bottom;
    // -------------------------
    jclass cls = env->FindClass("android/graphics/Rect");
    jfieldID fid = env->GetFieldID(cls, "left", "I");
    int left = env->GetIntField(rt, fid);
    fid = env->GetFieldID(cls, "right", "I");
    int right = env->GetIntField(rt, fid);
    fid = env->GetFieldID(cls, "top", "I");
    int top = env->GetIntField(rt, fid);
    fid = env->GetFieldID(cls, "bottom", "I");
    int bottom = env->GetIntField(rt, fid);
    env->DeleteLocalRef(cls);

    return iRectMake(left, top, right-left, bottom-top);
}

jobject ndkCreateRect(int x, int y, int w, int h)
{
    JNIEnv* env = getEnv();
    // -------------------------------------
    // Rect rect = new Rect(x, y, x+w, y+h);
    // -------------------------------------
    jclass cls = env->FindClass("android/graphics/Rect");
    jmethodID mid = env->GetMethodID(cls, "<init>", "(IIII)V");
    jobject rt = env->NewObject(cls, mid, x, y, x+w, y+h);
    env->DeleteLocalRef(cls);

    return rt;
}

jobject ndkCreateRect()
{
    JNIEnv* env = getEnv();
    // -----------------------
    // Rect rect = new Rect();
    // -----------------------
    jclass cls = env->FindClass("android/graphics/Rect");
    jmethodID mid = env->GetMethodID(cls, "<init>", "()V");
    jobject rt = env->NewObject(cls, mid);
    env->DeleteLocalRef(cls);

    return rt;
}

iRect ndkRectOfString(jobject paint, const char* str)
{
    // -----------------------
    // Rect rect = new Rect();
    // -----------------------
    jobject rt = ndkCreateRect();

    JNIEnv* env = getEnv();
    // ------------------------------------------------
    // paint.getTextBounds(sss, 0, sss.length(), rect);
    // ------------------------------------------------
    jclass cls = env->FindClass("android/graphics/Paint");
    jmethodID mid = env->GetMethodID(cls, "getTextBounds", "(Ljava/lang/String;IILandroid/graphics/Rect;)V");
    jstring jStr = javaNewStringChar(env, str);
    jint len = jstringLength(env, jStr);
    env->CallVoidMethod(paint, mid, jStr, 0, len, rt);

    env->DeleteLocalRef(jStr);
    env->DeleteLocalRef(cls);

    // -------------------
    // rt(java) => rect(c)
    // -------------------
    iRect rect = ndkGetRect(rt);
    env->DeleteLocalRef(rt);

    return rect;
}

void ndkDrawChar(jobject canvas, const char* szText, float x, float y, jobject paint)
{
    JNIEnv* env = getEnv();
    jstring s = javaNewStringChar(env, szText);
    ndkDrawText(canvas, s, x, y, paint);
    env->DeleteLocalRef(s);
}

//sound
void ndkLoadBgmSound(jstring path)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "loadBgmSound", "(Ljava/lang/String;)V");
    env->CallStaticVoidMethod(cls, mid, path);
    env->DeleteLocalRef(cls);
}

void ndkLoadSfxSound(jstring path)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "loadSfxSound", "(Ljava/lang/String;)V");
    env->CallStaticVoidMethod(cls, mid, path);
    env->DeleteLocalRef(cls);
}

void ndkPlayBgmSound(jint index)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "playBgmSound", "(I)V");
    env->CallStaticVoidMethod(cls, mid, index);
    env->DeleteLocalRef(cls);
}

void ndkPlaySfxSound(jint index)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "playSfxSound", "(I)V");
    env->CallStaticVoidMethod(cls, mid, index);
    env->DeleteLocalRef(cls);
}

void ndkStopBgmSound(jint index)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "stopBgmSound", "(I)V");
    env->CallStaticVoidMethod(cls, mid, index);
    env->DeleteLocalRef(cls);
}

void ndkStopSfxSound(jint index)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "stopSfxSound", "(I)V");
    env->CallStaticVoidMethod(cls, mid, index);
    env->DeleteLocalRef(cls);
}

void ndkVolumeBgmSound(jfloat volume)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "volumeBgmSound", "(F)V");
    env->CallStaticVoidMethod(cls, mid, volume);
    env->DeleteLocalRef(cls);
}

void ndkVolumeSfxSound(jfloat volume)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "volumeSfxSound", "(F)V");
    env->CallStaticVoidMethod(cls, mid, volume);
    env->DeleteLocalRef(cls);
}

void ndkPauseBgmSound(bool pause)
{
    JNIEnv* env = getEnv();
    jclass cls = env->FindClass("win/gatewalker/game/App");
    jmethodID mid = env->GetStaticMethodID(cls, "pauseBgmSound", "(Z)V");
    env->CallStaticVoidMethod(cls, mid, pause);
    env->DeleteLocalRef(cls);
}

