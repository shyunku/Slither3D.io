#pragma once
#include "global_constants.hpp"
#include <iostream>

using namespace std;

inline float randf(float LO, float HI)
{
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}

inline float distancef(vec3 v1, vec3 v2)
{
	return (v1 - v2).length();
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
	double time = glfwGetTime();
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

void logger(string str)
{
	cout << get_current_time_string() << " " << str << endl;
}

void print_vec3(vec3 v, const char* msg)
{
	printf("%s = (%.2f, %.2f, %.2f)\n", msg, v.x, v.y, v.z);
}

void print_vec3(vec3 v)
{
	printf("(%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
}

void print_version_of_app()
{
	cout << app_name << " " << version_str << endl;
}