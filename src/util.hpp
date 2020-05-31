#pragma once
#include "cgmath.h"
#include "cgut.h"
#include <iostream>

using namespace std;

float randf(float LO, float HI);
float rand_direction();
float rand_fraction();
int randi(int LO, int HI);
inline float distancef(vec3 v1, vec3 v2);
float refinef(float f);
vec3 get_random_vector();
string format_string(const string fmt, ...);
string get_current_time_string();
void logger(const string fmt, ...);
void print_vec3(vec3 v, const char* msg);
void print_vec3(vec3 v);
string get_vec3_string(vec3 v);
string get_vec3_pair_string(vec3 v);
void print_version_of_app();
vec3 get_restricted_vector(vec3 original, float max_difference);