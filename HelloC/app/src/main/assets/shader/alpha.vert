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

ATTR vec4 position;
ATTR vec4 color;
ATTR vec2 texcoord;

VARYING vec4 colorV;
VARYING vec2 texcoordV;

void main()
{
	gl_Position = projMatrix * viewMatrix * position;
	//colorV = vec4(color/255.);//최적화시
	colorV = color;
	texcoordV = texcoord;
}

