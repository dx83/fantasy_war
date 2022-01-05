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

uniform vec4 rect;
uniform float radius;
uniform vec4 color;

float rectDist(vec2 p, vec2 size, float radius)
{
	size -= vec2(radius);
	vec2 d = abs(p) - size;
	return min( max( d.x, d.y ), 0.0 ) + length( max( d, 0.0 ) ) - radius;
}

void main()
{
	float d = rectDist( gl_FragCoord.xy - rect.xy, rect.zw, radius );
	float a = clamp( -d, 0.0, 1.0 );
	FRAGCOLOR = vec4( color.rgb, color.a * a );
}

