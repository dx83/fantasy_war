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

uniform vec2 sp, ep;
uniform float lineWidth;
uniform vec4 color;

float lineDist(vec2 p, vec2 start, vec2 end, float width)
{
	vec2 n = end - start;
	float len = length(n);
	n /= len;
	// 내적, 제2코사인법칙
	vec2 m = p - start;
	vec2 proj = clamp( dot( m, n ), 0.0, len ) * n;
	return length( m-proj ) - width / 2.0;
}

void main()
{
	float d = lineDist( gl_FragCoord.xy, sp, ep, lineWidth );
	d = d / ( lineWidth / 2. );
	float a = clamp( -d, 0.0, 1.0 );
	FRAGCOLOR = vec4( color.rgb, color.a * a);
}

