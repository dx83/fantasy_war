#include "iStd.h"

#if (OS==OS_WINDOWS)
bool* keyBuf;
#elif (OS==OS_ANDROID)
#include "iNDK.h"
#endif

iSize devSize;
iRect viewport;
float rateWidth, rateHeight;

void appInitialize()
{
    srand((unsigned int)time(NULL));
}

struct Zoom
{
    bool ing;
    float delta, _delta;
    float delay, _delay;
    float scale;
    iPoint position;
    // const char* hp	구조체 데이터 저장이 안됨
    // char a[10]		구조체 데이터 저장이 됨
};

static Zoom zoom =	// 구조체 초기화, static 전역변수지만 접근불가
        {
                false,
                0.5f, 0.5f,
                0.5f, 0.5f,
                1.0f,
                {0, 0}
        };

bool setZoom(iPoint point, float scale, float deltaZoom, float deltaDelay)
{
    if (zoom.ing)
        return false;

    zoom.ing = true;
    zoom.position = point;
    zoom.scale = scale;
    zoom._delta = deltaZoom;
    zoom.delta = 0.0f;
    zoom._delay = deltaDelay;
    zoom.delay = 0.0f;

    return true;
}

float updateZoom(float dt, iPoint& p)
{
    float s = 1.0f;
    if (zoom.ing == false)
        return s;

    if (zoom.delta < zoom._delta)
    {
        // 1.0f => scale
        s = linear(zoom.delta / zoom._delta, 1.0f, zoom.scale);

        zoom.delta += dt;
        if (zoom.delta > zoom._delta)
            zoom.delta = zoom._delta;
    }
    else if (zoom.delta == zoom._delta)
    {
        s = zoom.scale;

        zoom.delay += dt;
        if (zoom.delay > zoom._delay)
            zoom.delta += 0.000001f;
    }
    else if (zoom.delta < zoom._delta * 2)
    {
        // scale => 1.0f
        s = linear((zoom.delta - zoom._delta) / zoom._delta, zoom.scale, 1.0f);

        zoom.delta += dt;
    }
    else
    {
        zoom.ing = false;
    }

    //p = zoom.position;			// P를 기준으로 이미지 뿌림
    p = zoom.position * (1.0f - s);	// 확대 기준점을 비율에 따라 항상 고정된 위치에 둠
    //==> 뿌리는 부분을 밀어냄
    return s;
}

static float _lineWidth = 1.0f;
void setLineWidth(float width)
{
    _lineWidth = width;
}
float getLineWidth()
{
    return _lineWidth;
}

void setClip(float x, float y, float width, float height)
{	// #issue
    if (width == 0.0f || height == 0.0f)
    {
        glDisable(GL_SCISSOR_TEST);
        //glScissor(0, 0, devSize.width, devSize.height);
    }
    else
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, width, height);
    }
}

static float dotSize_ = 1.0f;
void setDotSize(float size)
{
    dotSize_ = size;
}

void drawDot(float x, float y)
{
#if 0
    float radius = dotSize_ / 2;

    float p[4][4] =
    {
            //0.5f : 1픽셀 추가, 보간/안티얼라이징을 위한 공간 확보
            {-radius-0.5f, -radius-0.5f, 0, 1}, {radius+0.5f, -radius-0.5f, 0, 1},
            {-radius-0.5f, +radius+0.5f, 0, 1}, {radius+0.5f, +radius+0.5f, 0, 1},
    };

    GLuint id = vtx->useProgram("dot", "dot");
    glBindBuffer(GL_ARRAY_BUFFER, vtx->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, p);

    GLuint attr = glGetAttribLocation(id, "position");
    glEnableVertexAttribArray(attr);
    glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);
    GLuint attrPosition = attr;

    GLuint loc = glGetUniformLocation(id, "projMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixProj->d());

    loc = glGetUniformLocation(id, "viewMatrix");
    matrixView->push();// 현 좌표 백업
    matrixView->translate(x, y, 0);// 카메라 옮기기
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixView->d());

    // 좌표값(x,y) & 반지름(radius)
    loc = glGetUniformLocation(id, "center");
    glUniform2f(loc, x, devSize.height - y);//좌표계 변경
    loc = glGetUniformLocation(id, "radius");
    glUniform1f(loc, radius);
    loc = glGetUniformLocation(id, "color");
    float _r, _g, _b, _a;
    getRGBA(_r, _g, _b, _a);
    glUniform4f(loc, _r, _g, _b, _a);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vtx->vbe);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(attrPosition);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    matrixView->pop();// 원래 위치로 돌아오기
#endif
    struct Dot
    {
        float position[4];
        float color[4];
    };
    float radius = dotSize_ / 2;
    float _r, _g, _b, _a;
    getRGBA(_r, _g, _b, _a);
    Dot dot[4] =
            {
                    { {-radius - 0.5f, -radius - 0.5f,0,1}, {_r,_g,_b,_a}}, {{+radius + 0.5f, -radius - 0.5f,0,1}, {_r,_g,_b,_a}},
                    { {-radius - 0.5f, +radius + 0.5f,0,1}, {_r,_g,_b,_a}}, {{+radius + 0.5f, +radius + 0.5f,0,1}, {_r,_g,_b,_a}},
            };

    GLuint id = vtx->useProgram("dot", "dot");
    glBindBuffer(GL_ARRAY_BUFFER, vtx->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Dot) * 4, dot);

    GLuint attr = glGetAttribLocation(id, "position");
    glEnableVertexAttribArray(attr);
    glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, sizeof(Dot), (const void*)offsetof(Dot, position));
    GLuint attrPosition = attr;

    attr = glGetAttribLocation(id, "color");
    glEnableVertexAttribArray(attr);
    glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, sizeof(Dot), (const void*)offsetof(Dot, color));
    GLuint attrColor = attr;

    GLint loc = glGetUniformLocation(id, "projMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixProj->d());

    loc = glGetUniformLocation(id, "viewMatrix");
    matrixView->push();
    matrixView->translate(x, y, 0);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixView->d());

    // 좌표값 (x,y) & 반지름 (radius)
    loc = glGetUniformLocation(id, "center");
    glUniform2f(loc, x, devSize.height - y);
    loc = glGetUniformLocation(id, "radius");
    glUniform1f(loc, radius);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vtx->vbe);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(attrPosition);
    glDisableVertexAttribArray(attrColor);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    matrixView->pop();
}

void drawCircle(float x, float y, float radius)
{
    float p[4][4] =
    {
            {-radius - 1.f, -radius - 1.f, 0, 1}, {radius + 1.f, -radius - 1.f, 0, 1},
            {-radius - 1.f, +radius + 1.f, 0, 1}, {radius + 1.f, +radius + 1.f, 0, 1},
    };

    GLuint id = vtx->useProgram("dot", "drawCircle");
    glBindBuffer(GL_ARRAY_BUFFER, vtx->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, p);

    GLuint attr = glGetAttribLocation(id, "position");
    glEnableVertexAttribArray(attr);
    glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);
    GLuint attrPosition = attr;

    GLuint loc = glGetUniformLocation(id, "projMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixProj->d());

    loc = glGetUniformLocation(id, "viewMatrix");
    matrixView->push();// 현 좌표 백업
    matrixView->translate(x, y, 0);// 카메라 옮기기
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixView->d());

    // 좌표값(x,y) & 반지름(radius)
    loc = glGetUniformLocation(id, "center");
    glUniform2f(loc, x, devSize.height - y);//좌표계 변경
    loc = glGetUniformLocation(id, "radius");
    glUniform1f(loc, radius);
    loc = glGetUniformLocation(id, "lineWidth");
    glUniform1f(loc, _lineWidth);
    loc = glGetUniformLocation(id, "color");
    float _r, _g, _b, _a;
    getRGBA(_r, _g, _b, _a);
    glUniform4f(loc, _r, _g, _b, _a);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vtx->vbe);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(attrPosition);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    matrixView->pop();// 원래 위치로 돌아오기
}

void fillCircle(float x, float y, float radius)
{
    float ds = dotSize_;
    setDotSize(radius * 2);

    drawDot(x, y);

    dotSize_ = ds;
}

void drawLine(iPoint sp, iPoint ep)
{
    float lw = _lineWidth / 2;
    float d = iPointDistance(sp, ep) / 2;
    float p[4][4] =//x,y,z(좌표),w(weight스케일)
            {
                    { -d-lw-0.5f, -lw-0.5f, 0, 1},	{ +d+lw+0.5f, -lw-0.5f, 0, 1},
                    { -d-lw-0.5f, +lw+0.5f, 0, 1},	{ +d+lw+0.5f, +lw+0.5f, 0, 1},
            };

    GLuint id = vtx->useProgram("dot", "drawLine");
    glBindBuffer(GL_ARRAY_BUFFER, vtx->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, p);

    GLint loc = glGetUniformLocation(id, "projMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixProj->d());

    loc = glGetUniformLocation(id, "viewMatrix");
    matrixView->push();
    iPoint c = (sp + ep) / 2;
    matrixView->translate(c.x, c.y, 0);

    float theta = iPointAngle(iPointMake(1, 0), iPointZero, ep - sp);
    matrixView->rotate(0, 0, 1, 360 - theta);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixView->d());

    GLuint attr = glGetAttribLocation(id, "position");
    glEnableVertexAttribArray(attr);
    //glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void*)0);
    glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);//여기에선 위와 차이없음

    loc = glGetUniformLocation(id, "sp");
    iPoint cp = iPointMake(sp.x, devSize.height - sp.y);
    glUniform2fv(loc, 1, (float*)&cp);

    loc = glGetUniformLocation(id, "ep");
    cp = iPointMake(ep.x, devSize.height - ep.y);
    glUniform2fv(loc, 1, (float*)&cp);

    loc = glGetUniformLocation(id, "lineWidth");
    glUniform1f(loc, _lineWidth);

    loc = glGetUniformLocation(id, "color");
    float _r, _g, _b, _a;
    getRGBA(_r, _g, _b, _a);
    glUniform4f(loc, _r, _g, _b, _a);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vtx->vbe);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(attr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    matrixView->pop();
}

void drawLine(float x0, float y0, float x1, float y1)
{
    drawLine(iPointMake(x0, y0), iPointMake(x1, y1));
}

//void drawRect(const iRect& rt, float radius);
void drawRect(iRect rt, float radius)
{
    drawRect(rt.origin.x, rt.origin.y, rt.size.width, rt.size.height, radius);
}
void drawRect(float x, float y, float width, float height, float radius)
{
    float p[4][4] =//x,y,z(좌표),w(weight스케일)
    {
        { -width/2-0.5f, -height/2-0.5f, 0, 1},	{ +width/2+0.5f, -height/2-0.5f, 0, 1},
        { -width/2-0.5f, +height/2+0.5f, 0, 1},	{ +width/2+0.5f, +height/2+0.5f, 0, 1},
    };
    GLuint id = vtx->useProgram("dot", "drawRect");
    glBindBuffer(GL_ARRAY_BUFFER, vtx->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, p);

    GLuint attr = glGetAttribLocation(id, "position");
    glEnableVertexAttribArray(attr);
    glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);

    GLint loc = glGetUniformLocation(id, "projMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixProj->d());

    loc = glGetUniformLocation(id, "viewMatrix");
    matrixView->push();
    matrixView->translate(x + width / 2, y + height / 2, 0);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixView->d());

    loc = glGetUniformLocation(id, "rect");
    glUniform4f(loc, x + width / 2, devSize.height - (y + height / 2), width / 2, height / 2);

    loc = glGetUniformLocation(id, "radius");
    glUniform1f(loc, radius);

    loc = glGetUniformLocation(id, "lineWidth");
    glUniform1f(loc, _lineWidth);

    loc = glGetUniformLocation(id, "color");
    float _r, _g, _b, _a;
    getRGBA(_r, _g, _b, _a);
    glUniform4f(loc, _r, _g, _b, _a);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vtx->vbe);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(attr);
    matrixView->pop();
}

//void fillRect(const iRect& rt, float radius);
void fillRect(iRect rt, float radius)
{
    fillRect(rt.origin.x, rt.origin.y, rt.size.width, rt.size.height, radius);
}
void fillRect(float x, float y, float width, float height, float radius)
{
    float p[4][4] =//x,y,z(좌표),w(weight스케일)
    {
        { -width / 2 - 0.5f, -height / 2 - 0.5f, 0, 1},	{ +width / 2 + 0.5f, -height / 2 - 0.5f, 0, 1},
        { -width / 2 - 0.5f, +height / 2 + 0.5f, 0, 1},	{ +width / 2 + 0.5f, +height / 2 + 0.5f, 0, 1},
    };
    GLuint id = vtx->useProgram("dot", "fillRect");
    glBindBuffer(GL_ARRAY_BUFFER, vtx->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, p);

    GLuint attr = glGetAttribLocation(id, "position");
    glEnableVertexAttribArray(attr);
    glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);

    GLint loc = glGetUniformLocation(id, "projMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixProj->d());

    loc = glGetUniformLocation(id, "viewMatrix");
    matrixView->push();
    matrixView->translate(x + width / 2, y + height / 2, 0);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixView->d());

    loc = glGetUniformLocation(id, "rect");
    glUniform4f(loc, x + width / 2, devSize.height - (y + height / 2), width / 2, height / 2);

    loc = glGetUniformLocation(id, "radius");
    glUniform1f(loc, radius);

    loc = glGetUniformLocation(id, "color");
    float _r, _g, _b, _a;
    getRGBA(_r, _g, _b, _a);
    glUniform4f(loc, _r, _g, _b, _a);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vtx->vbe);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(attr);
    matrixView->pop();
}

void drawTri(float x, float y, float width, float height, int reverse)
{
    if (reverse == REVERSE_HEIGHT)
    {
        drawLine(x + width / 2, y + height, x, y);
        drawLine(x + width / 2, y + height, x + width, y);
        drawLine(x, y, x + width, y);
    }
    else//REVERSE_NONE
    {
        drawLine(x + width / 2, y, x, y + height);
        drawLine(x + width / 2, y, x + width, y + height);
        drawLine(x, y + height, x + width, y + height);
    }
}

void fillTri(float x, float y, float width, float height, int reverse)
{
    float p[3][4] =//x,y,z(좌표),w(weight스케일)
            {
                    { -width / 2 - 0.5f, -height / 2 - 0.5f, 0, 1},
                    { +width / 2 + 0.5f, -height / 2 - 0.5f, 0, 1},
                    { - 0.5f, +height / 2 + 0.5f, 0, 1},
            };

    GLuint id = vtx->useProgram("dot", "fillTri");
    glBindBuffer(GL_ARRAY_BUFFER, vtx->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 12, p);

    GLuint attr = glGetAttribLocation(id, "position");
    glEnableVertexAttribArray(attr);
    glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);

    GLint loc = glGetUniformLocation(id, "projMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixProj->d());

    loc = glGetUniformLocation(id, "viewMatrix");
    matrixView->push();
    matrixView->translate(x + width / 2, y + height / 2, 0);
    if (reverse != REVERSE_HEIGHT)
        matrixView->rotate(x, 0, 0, 180);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)matrixView->d());

    loc = glGetUniformLocation(id, "tri");
    glUniform4f(loc, x + width / 2, devSize.height - (y + height / 2), width / 2, height / 2);

    loc = glGetUniformLocation(id, "color");
    float _r, _g, _b, _a;
    getRGBA(_r, _g, _b, _a);
    glUniform4f(loc, _r, _g, _b, _a);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vtx->vbe);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(attr);
    matrixView->pop();
}

uint32 nextPot(uint32 x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return x + 1;
}

void copyBmpToRgba(uint32* src, int stride, int x, int y, int w, int h,
                   uint32* dst, int dw, int dh)
{
    int potWidth = nextPot(dw);
    int potHeight = nextPot(dh);
    memset(dst, 0x00, sizeof(uint32) * potWidth * potHeight);
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            uint8* d = (uint8*)&dst[potWidth * j + i];
            uint8* s = (uint8*)&src[stride * (y + j) + (x + i)];
            d[0] = s[2];// r
            d[1] = s[1];// g
            d[2] = s[0];// b
            d[3] = s[3];// a
        }
    }
}

Texture* createImageWithRGBA(uint8* rgba, int width, int height)
{
    int potWidth = nextPot(width);
    int potHeight = nextPot(height);

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    setTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, potWidth, potHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glBindTexture(GL_TEXTURE_2D, 0);

    Texture* tex = new Texture;
    tex->texID = texID;
    tex->width = width;
    tex->height = height;
    tex->potWidth = potWidth;
    tex->potHeight = potHeight;
    tex->retainCount = 1;
#if CHECK_TEXTURE
    texture_num++;
#endif

    return tex;
}

Texture* createTexture(int width, int height, bool rgba32f)	// FBO texture
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    setTexture();
#if (OS==OS_WINDOWS)
    if (rgba32f == false)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    }
#elif (OS==OS_ANDROID)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#endif
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, 0);

    Texture* tex = new Texture;
    tex->texID = texID;
    tex->width = width;
    tex->height = height;
    tex->potWidth = width;
    tex->potHeight = height;
    tex->retainCount = 1;
#if CHECK_TEXTURE
    texture_num++;
#endif

    return tex;
}
#if CHECK_TEXTURE
int texture_num = 0;
#endif

Texture** createImageDevide(int numX, int numY, const char* szFormat, ...)
{
    char szText[1024];
    va_start_end(szText, szFormat);

#if (OS==OS_WINDOWS)
    wchar_t* ws = utf8_to_utf16(szText);
    Bitmap* bmp = Bitmap::FromFile(ws, false);
    delete ws;

    int width = bmp->GetWidth();
    int height = bmp->GetHeight();
    Rect rect(0, 0, width, height);
    BitmapData bmpData;
    bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

    int stride = bmpData.Stride / 4;
    uint32* rgba = (uint32*)bmpData.Scan0;

    Texture** texs = new Texture * [numX * numY];
    int w = width / numX;
    int h = height / numY;
    int potW = nextPot(w);
    int potH = nextPot(h);
    uint32* pixel = new uint32[potW * potH];
    for (int j = 0; j < numY; j++)
    {
        for (int i = 0; i < numX; i++)
        {
            copyBmpToRgba(rgba, stride, w * i, h * j, w, h, pixel, w, h);
            texs[numX * j + i] = createImageWithRGBA((uint8*)pixel, w, h);
        }
    }
    delete pixel;

    bmp->UnlockBits(&bmpData);
    delete bmp;
#elif (OS==OS_ANDROID)
    Texture** texs = ndkCreateImageDevide(numX, numY, szText);
#endif
    return texs;
}

Texture* createImage(const char* szFormat, ...)
{
    char szText[1024];
    va_start_end(szText, szFormat);

#if (OS==OS_WINDOWS)
    wchar_t* ws = utf8_to_utf16(szText);
    Bitmap* bmp = Bitmap::FromFile(ws, false);
    delete ws;

    int width = bmp->GetWidth();
    int height = bmp->GetHeight();
    Rect rect(0, 0, width, height);
    BitmapData bmpData;
    bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

    int stride = bmpData.Stride / 4;
    uint32* rgba = (uint32*)bmpData.Scan0;

    int potWidth = nextPot(width);
    int potHeight = nextPot(height);
    uint32* pixel = new uint32[potWidth * potHeight];
    copyBmpToRgba(rgba, stride, 0, 0, width, height,
                  pixel, width, height);

    Texture* tex = createImageWithRGBA((uint8*)pixel, width, height);
    delete pixel;

    bmp->UnlockBits(&bmpData);
    delete bmp;
#elif (OS==OS_ANDROID)
    if( szText[0]=='a' &&
        szText[1]=='s' &&
        szText[2]=='s' &&
        szText[3]=='e' &&
        szText[4]=='t' &&
        szText[5]=='s' &&
        szText[6]=='/')
        strcpy(szText, &szText[7]);

    Texture* tex = ndkCreateImage(szText);
#endif

    return tex;
}

void freeImage(Texture* tex)
{
    if (tex==NULL)
        return;

    if (tex->retainCount > 1)
    {
        tex->retainCount--;
        return;
    }
    glDeleteTextures(1, &tex->texID);
    delete tex;
#if CHECK_TEXTURE
    texture_num--;
#endif
}

void drawImage(Texture* tex, float x, float y, int anc,
               float ix, float iy, float iw, float ih,
               float rx, float ry,
               int xyz, float degree, int reverse)
{
    //float w = tex->width * rx, h = tex->height * ry;
    float w = iw * rx, h = ih * ry;

    switch (anc) {
        case TOP | LEFT:								break;
        case TOP | RIGHT:		x -= w;					break;
        case TOP | HCENTER:		x -= w / 2;				break;

        case BOTTOM | LEFT:					y -= h;		break;
        case BOTTOM | RIGHT:	x -= w;		y -= h;		break;
        case BOTTOM | HCENTER:	x -= w / 2;	y -= h;		break;

        case VCENTER | LEFT:				y -= h / 2;	break;
        case VCENTER | RIGHT:	x -= w;		y -= h / 2;	break;
        case VCENTER | HCENTER:	x -= w / 2;	y -= h / 2;	break;
    }
    iPoint dstPoint[4] =
            {
                    {0, 0},		{w, 0},
                    {0, h},		{w, h}
            };
    if (reverse == REVERSE_NONE);
    else if (reverse == REVERSE_WIDTH)
    {
        float t = dstPoint[0].x;
        dstPoint[0].x = dstPoint[1].x;
        dstPoint[1].x = t;

        dstPoint[2].x = dstPoint[0].x;
        dstPoint[3].x = dstPoint[1].x;
    }
    else if (reverse == REVERSE_HEIGHT)
    {
        float t = dstPoint[0].y;
        dstPoint[0].y = dstPoint[2].y;
        dstPoint[2].y = t;

        dstPoint[1].y = dstPoint[0].y;
        dstPoint[3].y = dstPoint[2].y;
    }

    matrixView->push();
    matrixView->translate(x, y, 0);

    if (degree)
    {
        iPoint t = iPointMake(x + w / 2, y + h / 2);
        if (xyz == 0)// x
        {
            matrixView->translate(w / 2, h / 2, 0);
            matrixView->rotate(1, 0, 0, degree);
            matrixView->translate(-w / 2, -h / 2, 0);
        }
        else if (xyz == 1)// y
        {
            matrixView->translate(w / 2, h / 2, 0);
            matrixView->rotate(0, 1, 0, degree);
            matrixView->translate(-w / 2, -h / 2, 0);
        }
        else// if (xyz == 2)// z
        {
            matrixView->translate(w / 2, h / 2, 0);
            matrixView->rotate(0, 0, 1, degree);
            matrixView->translate(-w / 2, -h / 2, 0);
        }
    }

    // 0,0   1,0
    //
    // 0,1   1,1
    iPoint st[4] = {
            {ix / tex->potWidth, iy / tex->potHeight},			{(ix + iw) / tex->potWidth, iy / tex->potHeight},
            {ix / tex->potWidth, (iy + ih) / tex->potHeight},	{(ix + iw) / tex->potWidth, (iy + ih) / tex->potHeight},
    };


    //iColor4f c[4];
    //for (int i = 0; i < 4; i++)
    //    getRGBA(c[i].r, c[i].g, c[i].b, c[i].a);
    float _r, _g, _b, _a;
    getRGBA(_r, _g, _b, _a);
    iColor4f c[4] =
    {
            {_r, _g, _b, _a}, {_r, _g, _b, _a},
            {_r, _g, _b, _a}, {_r, _g, _b, _a},
    };

    GLuint id = vtx->useProgram("alpha", "alpha");
    VertexInfo vi[4];
    for (int i = 0; i < 4; i++)
    {
        memcpy(vi[i].position, &dstPoint[i], sizeof(iPoint));
        vi[i].position[2] = 0;
        vi[i].position[3] = 1;
        memcpy(&vi[i].color, &c[i], sizeof(iColor4f));
        memcpy(&vi[i].uv, &st[i], sizeof(iPoint));
    }
    vtx->bufferSubData(vi, 1, 1);
    GLuint attrP = vtx->enableVertexAttrArray("position", (const void*)offsetof(VertexInfo, position), sizeof(VertexInfo), 4, GL_FLOAT);
    GLuint attrC = vtx->enableVertexAttrArray("color", (const void*)offsetof(VertexInfo, color), sizeof(VertexInfo), 4, GL_FLOAT);
    GLuint attrT = vtx->enableVertexAttrArray("texcoord", (const void*)offsetof(VertexInfo, uv), sizeof(VertexInfo), 2, GL_FLOAT);

    vtx->setUniformMat("projMatrix", matrixProj);
    vtx->setUniformMat("viewMatrix", matrixView);
    vtx->setUniform1i("tex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->texID);

    vtx->drawElements();

    vtx->disableVertexAttrArray(attrP);
    vtx->disableVertexAttrArray(attrC);
    vtx->disableVertexAttrArray(attrT);
    vtx->bufferSubData(NULL, 0, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    matrixView->pop();
}

void drawImage(Texture* tex, float x, float y, int anc)
{
    drawImage(tex, x, y, anc, 0, 0, tex->width, tex->height, 1.0f, 1.0f, 2, 0);
}

static char* stringName = NULL;
static float stringSize = 20.0f;
static float stringLineHeight = 30.0f;
static float stringBorder = 0.0f;
static int stringStyle = 0;

const char* getStringName()
{
    return stringName;
}

void setStringName(const char* str)
{
    if (stringName)
    {
        if (strcmp(stringName, str) == 0)
            return;

        delete stringName;
    }
    stringName = new char[strlen(str) + 1];
    strcpy(stringName, str);
}

float getStringSize()
{
    return stringSize;
}
void setStringSize(float size)
{
    stringSize = size;
}

void setStringLineHeight(float height)
{
    stringLineHeight = height;
}

float getStringLineHeight()
{
    return stringLineHeight;
}

float getStringBorder()
{
    return stringBorder;
}
void setStringBorder(float border)
{
    stringBorder = border;
}

void setStringStyle(int fontStyle)
{
    stringStyle = fontStyle;
}

int getStringStyle()
{
    return stringStyle;
}

iFont* loadFont(const char* strOTFTTF, float height, const char* strUse)
{
    iFont* f = new iFont;
    f->height = height;
    f->interval = 1;
    f->texs = new Texture*[256];
    memset(f->texs, 0x00, sizeof(Texture*) * 256);

    setStringName(strOTFTTF);
    setStringSize(height);
    setStringRGBA(255, 255, 255, 255);
    setStringBorder(0);
    iGraphics* g = iGraphics::share();

    //int i, len = strlen(strUse);
    //for (i = 0; i < len; i++)
    for (int i = 0; strUse[i]; i++)	// 저급언어라 가능
    {
        char c = strUse[i];
        if (f->texs[c]) continue;	// 중복 처리

        if (c == ' ')	// 띄어쓰기는 0,0이 되므로... 예외처리
        {
            g->init(height * 0.6, height);

            f->texs[c] = g->getTexture();
            continue;
        }
        else if (c == ',')
        {
            g->init(height * 0.6, height);

            iRect rt = iGraphics::rectOfString(",");
            g->drawString(0, height - rt.size.height, TOP | LEFT, ",");

            f->texs[c] = g->getTexture();
            continue;
        }

        char str[2] = { c, 0 };
        iRect rt = iGraphics::rectOfString(str);
        g->init(rt.size);

        g->drawString(-rt.origin.x, -rt.origin.y, TOP | LEFT, str);

        f->texs[c] = g->getTexture();
    }

    return f;
}

void freeFont(iFont* font)
{
    for (int i = 0; i < 256; i++)
    {
        if (font->texs[i])
            freeImage(font->texs[i]);
    }
    delete font->texs;
    delete font;
}

iFont* fontCurr;
void setFont(iFont* font)
{
    fontCurr = font;
}

char* loadFile(const char* fileName, int& length)
{
#if (OS==OS_WINDOWS)
    FILE* pf = fopen(fileName, "rb");
#elif (OS==OS_ANDROID)
    char path[1024];
    sprintf(path, "%s/%s", getDevicePath(), fileName);
    FILE* pf = fopen(path, "rb");
#endif
    if (!pf)
    {
        length = 0;
        return NULL;
    }

    fseek(pf, 0, SEEK_END);
    length = ftell(pf);
    fseek(pf, 0, SEEK_SET);

    char* buf = new char[length + 1];
    fread(buf, sizeof(char), length, pf);
    buf[length] = 0;	// 널문자 삽입

    fclose(pf);

    return buf;
}

void saveFile(const char* fileName, char* buf, int length)
{
#if (OS==OS_WINDOWS)
    FILE* pf = fopen(fileName, "wb");
#elif (OS==OS_ANDROID)
    char path[1024];
    sprintf(path, "%s/%s", getDevicePath(), fileName);
    FILE* pf = fopen(path, "wb");
#endif
    fwrite(buf, sizeof(char), length, pf);
    fclose(pf);
}

