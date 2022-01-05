#ifdef GL_ES//android
precision mediump float;
#define VARYING varying

#define FRAGCOLOR gl_FragColor

#else// Windows
#version 150

#define VARYING in

out vec4 fragColor;
#define FRAGCOLOR fragColor
#endif


//uniform vec4 tri;
//uniform float lineWidth;
//uniform vec4 color;

void main()
{

}

