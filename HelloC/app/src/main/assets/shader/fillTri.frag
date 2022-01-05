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

uniform vec4 tri;
uniform vec4 color;

float rectDist(vec2 p, vec2 size)
{
	vec2 d = abs(p) - size;
	return min( max( d.x, d.y ), 0.0 ) + length( max( d, 0.0 ) );
}

void main()
{
	float d = rectDist( gl_FragCoord.xy - tri.xy, tri.zw );
	float a = clamp( -d, 0.0, 1.0 );
	FRAGCOLOR = vec4( color.rgb, color.a * a );
}

