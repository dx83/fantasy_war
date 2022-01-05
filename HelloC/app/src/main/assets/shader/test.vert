#ifdef GL_ES//android
precision mediump float;
#define ATTR attribute
#define VARYING varying

#else// Windows
#version 150
#define ATTR in
#define VARYING out

#endif

uniform mat4 projMatrix, viewMatrix;

// App.cpp에서 받아옴
ATTR vec4 position;
ATTR vec4 color;
ATTR vec2 texcoord;
// .frag로 넘겨줌
VARYING vec4 colorV;
VARYING vec2 texcoordV;

void main()
{
	gl_Position = projMatrix * viewMatrix * position;
	colorV = color;
	texcoordV = texcoord;
}

