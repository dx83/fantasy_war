#ifdef GL_ES
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

VARYING vec4 colorV;

void main()
{
	gl_Position = projMatrix * viewMatrix * position;
	colorV = color;
}

