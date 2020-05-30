#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

in vec2 tc;
out vec4 fragColor;

//uniform vec4 solid_color;

void main()
{
	//vec2 p = gl_FragCoord.xy;
	//float f = 0.1 / abs(p.x*p.x + p.y*p.y);

	//if(p.x < 500)
	//	fragColor = vec4(0,1,0,1);
	//else
	//	fragColor = vec4(1,0,0,1);

	fragColor = vec4(1,0,0,1);
}
