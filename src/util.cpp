#include "util.hpp"
#include <iostream>

extern string	app_name;
extern string	version_str;

float rand_direction()
{
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2 * PI));
}

float randf(float LO, float HI)
{
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}

float rand_fraction()
{
	return randf(0, 1);
}

int randi(int LO, int HI)
{
	return LO + rand() % (HI - LO);
}

inline float distancef(vec3 v1, vec3 v2)
{
	return (v1 - v2).length();
}

float refinef(float f)
{
	if (f > 1)
	{
		return 1.f;
	}
	else if (f < 0)
	{
		return 0.f;
	}
	else
	{
		return f;
	}
}

vec4 get_brighter_color(vec4 color, float f)
{
	f = refinef(f);
	vec3 rgb = vec3(color.x, color.y, color.z);
	return vec4(rgb + (vec3(1) - rgb) * f, color.a);
}

int check_string_bool(string str)
{
	if (!str.compare("true")) return 1;
	if (!str.compare("false")) return -1;
	return 0;
}

vec3 get_random_vector()
{
	float theta = randf(0, 1) * 2 * PI;
	float alpha = randf(0, 1) * 2 * PI;
	return vec3(cosf(theta) * cosf(alpha), sinf(theta) * cosf(alpha), sin(alpha)).normalize();
}

string format_string(const string fmt, ...)
{
	int size = ((int)fmt.size()) * 2;
	string buffer;
	va_list ap;
	while (1) {
		buffer.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char*)buffer.data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size) {
			buffer.resize(n);
			return buffer;
		}
		if (n > -1)
			size = n + 1;
		else
			size *= 2;
	}
	return buffer;
}

string get_current_time_string()
{
	float time = float(glfwGetTime());
	uint cur = uint(time);
	uint h = cur / 3600;
	uint m = (cur % 3600) / 60;
	uint s = (cur % 60);
	uint ms = uint(1000 * (time - (double)cur));
	if (h > 0)
	{
		return format_string("[%d:%02d:%02d.%03ds]", h, m, s, ms);
	}
	return format_string("[%02d:%02d.%03ds]", m, s, ms);
}

void logger(const string fmt, ...)
{
	int size = ((int)fmt.size()) * 2;
	string buffer;
	va_list ap;
	while (1) {
		buffer.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char*)buffer.data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size) {
			buffer.resize(n);
			break;
		}
		if (n > -1)
			size = n + 1;
		else
			size *= 2;
	}
	cout << get_current_time_string() << " " << buffer << endl;
}

void print_vec3(vec3 v, const char* msg)
{
	printf("%s = (%.2f, %.2f, %.2f)\n", msg, v.x, v.y, v.z);
}

void print_vec3(vec3 v)
{
	printf("(%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
}

string get_vec3_string(vec3 v)
{
	return format_string("(%.2f, %.2f, %.2f)", v.x, v.y, v.z);
}

string get_vec3_pair_string(vec3 v)
{
	return format_string("%.2f, %.2f, %.2f", v.x, v.y, v.z);
}

void print_version_of_app()
{
	cout << app_name << " " << version_str << endl;
}

vec3 get_restricted_vector(vec3 original, float max_difference)
{
	float refdef = refinef(max_difference);
	float co = cosf(max_difference);
	float so = sinf(max_difference);

	vec3 sli = original + vec3(1, 1, 0);
	vec3 vr = co * sli / (original * sli).sum();
	
	vec3 vx = (vr - co * original).normalize();
	vec3 vy = original.cross(vx).normalize();
	float virtual_random_angle = randf(0,2*PI);
	vec3 nm = co * original + so * (cosf(virtual_random_angle) * vx + sinf(virtual_random_angle) * vy);
	
	return nm.normalize();
}