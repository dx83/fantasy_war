
#ifdef GL_ES//android
precision mediump float;
#define VARYING varying

#define FRAGCOLOR gl_FragColor

#else// Windows
#version 150

out vec4 fragColor;
#define VARYING in

#define FRAGCOLOR fragColor
#endif

uniform vec2 center;
uniform float radius;
VARYING vec4 colorV;

void main()
{
	// 보간된 원 그리기
	FRAGCOLOR = vec4(colorV.xyz, clamp(radius - length(center - gl_FragCoord.xy), 0.0, 1.0));
}

