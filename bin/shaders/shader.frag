#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

in vec2 tc;
out vec4 fragColor;

void main()
{
	fragColor = vec4(tc.xy, 0, 1);
}
