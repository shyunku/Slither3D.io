#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

in vec2 tc;
out vec4 fragColor;

uniform vec4 solid_color;
//uniform vec3 shadow_direction;

/*#define PI 3.1415926535

float refine_angle(float a)
{
	if(a > PI * 2) return a - PI * 2;
	if(a < 0)return a+ PI*2;
	return a;
}

float dot(vec3 a, vec3 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}*/

void main()
{
	fragColor = solid_color * vec4(tc.yyy, 1);
}

/* 2
	float alpha = (2 * tc.y - 1) * PI/2;
	float theta = 2 * PI * tc.x;

	float cl = cos(alpha);
	float sl = sin(alpha);

	float co = cos(theta);
	float so = sin(theta);

	vec3 dir = vec3(cl*co, sl, -cl*so);

	float doti = dot(shadow_direction, dir);
	if(doti < 0) doti = 0.f;
	if(doti > 1) doti = 1.f;
	vec3 v = vec3((doti+1)/2);


	fragColor = vec4(v, 1);
*/

/* 1
	float yf = (shadow_direction.y+1)/2;
	vec2 xzf = shadow_direction.xz;
	float thetaf = 0;


	if(xzf.y>0)
	{
		thetaf = acos(xzf.x);
	}
	else
	{
		thetaf = 2*PI - acos(xzf.x);
	}

	thetaf = refine_angle(thetaf + PI/2);

	vec2 ntc = vec2(thetaf/(2*PI), yf);

	float tx = tc.x * ntc.x + tc.y * ntc.y;

*/