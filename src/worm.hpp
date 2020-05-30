#pragma once
#include "cgmath.h"
#include "cgut.h"
#include "log_manager.hpp"

#define AUTO_DIRECTION_CHANGE_PERIOD_KEYWORD randf(0.3f, 3.f)
extern inline float rand_fraction();
extern inline float randf(float LO, float HI);
extern inline float rand_direction();
extern inline int randi(int LO, int HI);
extern GLuint get_validated_uniform_location(GLuint shader_program, const char* name);
extern const float WORLD_BORDER_RADIUS;
extern void print_vec3(vec3 v);
extern string format_string(const string fmt, ...);
extern string get_vec3_string(vec3 v);
extern vec3 get_random_vector();


extern const float WORLD_BORDER_RADIUS;

using namespace std;

typedef class worm_body_
{
public:
	vec3	pos;
	float	radius;
	vec3	direction;
	worm_body_();
	worm_body_(vec3 initial_pos, vec3 direction);
	mat4 get_model_matrix();
	void move(float move_dist);
	void track_parent(vec3 parent_pos);
	void set_size(float size);
}WormBody;

typedef class worm_
{
private:
	const float			INITIAL_BODY_GAP = 0.6f;
	const float			BODY_GROWTH = 50.f;
	const float			MIN_BODY_LENGTH = 10;
	const float			MIN_DIRECTION_CHANGE = PI / 4;
	const float			MAX_DIRECTION_CHANGE = PI / 2;
	const float			AVOID_WALL_START_RANGE = WORLD_BORDER_RADIUS * 3 / 4;

	const float			DEFAULT_SPEED = 1.5f;
	const float			MAX_SPEED = 20.f;
	vec4				color;
	string				UID;
	vector<WormBody>	body;
	void render_head(GLuint shader_program, uint sphere_triangles);
	void render_body(GLuint shader_program, uint sphere_triangles);
	vec3 get_restricted_vector(vec3 original, float max_difference);
	void update_body(float time_tick);
public:
	float				speed = DEFAULT_SPEED;
	WormBody			head;
	bool				is_player = false;
	float				growth;

	// AI area
	float				auto_direction_change_period = AUTO_DIRECTION_CHANGE_PERIOD_KEYWORD;
	float				elapsed_direction_change_timestamp = 0;
	vec3				decided_direction = vec3(0);

	/*
		size(radius) √÷º“ 0.5
		growth - size:
		0 - 0.5
		300 - 1.0
		1000 - 1.5
		0.5ln(x+200) - ln(sqrt(200)) + 0.5
	*/

	worm_();
	worm_(float initial_growth);
	worm_(vec3 initial_pos, float initial_growth);
	void render_sphere(GLuint shader_program, uint sphere_triangles);
	void update(float time_tick);
	void make_player();
	string get_uid();
	void set_speed(float v);
}Worm;