
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

uniform vec2 center;
uniform float radius;
uniform float lineWidth;
uniform vec4 color;

void main()
{
#if 0
	float w = lineWidth / 2.;
	float d = abs(length(center - gl_FragCoord.xy) - (radius - w));
	if (d > w)
		FRAGCOLOR = vec4(color.rgb, 0.0);
	else
		FRAGCOLOR = vec4(color.rgb, color.a);
#else
	float d = length(center - gl_FragCoord.xy) - radius;
	float a = clamp(d + lineWidth / 2., 0.0, 1.0) - clamp(d, 0.0, 1.0);
	FRAGCOLOR = vec4(color.rgb, color.a * a);
#endif
}

