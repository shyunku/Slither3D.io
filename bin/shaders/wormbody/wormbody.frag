#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

in vec2 tc;
out vec4 fragColor;

uniform vec4 solid_color;

void main()
{
	fragColor = solid_color * vec4(tc.yyy, 1);
}
