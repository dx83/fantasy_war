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

//uniform vec2 resolution;
uniform vec2 center;

uniform sampler2D tex;

VARYING vec4 colorV;
VARYING vec2 texcoordV;

void main()
{
	//vec4 c = texture(tex, texcoordV) * colorV;

	// gl_FragCoord - device position

	//vec2 uv = gl_FragCoord.xy / resolution;
	//float a = length(center - gl_FragCoord.xy) / 50.0;
	//
	//float g = clamp(1.0-a, 0.0, 1.0);
	//float b = clamp(a-1.0, 0.0, 1.0);
	//FRAGCOLOR = vec4(0.0, g, b, 1.0) * colorV;
	//FRAGCOLOR = colorV;

	FRAGCOLOR = texture(tex, texcoordV) * colorV;


	//if (a < 1.0)
	//	FRAGCOLOR = vec4(0.0, 1.0, 0.0, 1.0);
	//else
	//	FRAGCOLOR = vec4(0.0, 0.0, 1.0, 1.0);
	//
	//float g = clamp(a, 0.0, 1.0);

	//if (gl_FragCoord.x < 200)
	//	FRAGCOLOR = vec4(1.0, 0.0, 0.0, 1.0);
	//else
	//	FRAGCOLOR = vec4(0.0, 0.0, 1.0, 1.0);
}

