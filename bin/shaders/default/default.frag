#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

in vec2 tc;
out vec4 fragColor;

uniform bool is_using_solid_color;
uniform vec4 solid_color;

void main()
{
	fragColor = is_using_solid_color ? solid_color : vec4(tc.yyy, 1);
}
