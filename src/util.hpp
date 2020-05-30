#pragma once
#include <iostream>
#include "cgmath.h"
#include "cgut.h"

using namespace std;

inline float rand_direction();
inline float randf(float LO, float HI);
inline float rand_fraction();
inline int randi(int LO, int HI);
inline float distancef(vec3 v1, vec3 v2);
inline float refinef(float f);
vec3 get_random_vector();
string format_string(const string fmt, ...);
string get_current_time_string();
void logger(const string fmt, ...);
void print_vec3(vec3 v, const char* msg);
void print_vec3(vec3 v);
string get_vec3_string(vec3 v);
void print_version_of_app();