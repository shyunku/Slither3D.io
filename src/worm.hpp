#pragma once
#include "cgmath.h"
#include "cgut.h"
using namespace std;

extern const float WORLD_BORDER_RADIUS;

typedef enum ai_status_ { EVADE_BORDER, DEFENSE, OFFENSE, SEEK_PREY, STANDARD, NONE_AI }AI_STATUS;

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

	const float			DEFAULT_SPEED = 10.f;
	const float			BOOST_SPEED_FACTOR = 2.f;
	const float			MAX_SPEED = 200.f;
	vec4				color;
	uint				object_id;
	void render_head(GLuint shader_program, uint sphere_triangles);
	void render_body(GLuint shader_program, uint sphere_triangles);
	void update_body(float time_tick);
public:
	float				speed = DEFAULT_SPEED;
	WormBody			head;
	vector<WormBody>	body;
	bool				is_player = false;
	bool				boosting = false;
	float				growth;

	// AI area
	AI_STATUS			ai_status = STANDARD;
	float				auto_direction_change_period = 0;
	float				elapsed_direction_change_timestamp = 0;
	vec3				decided_direction = vec3(0);

	worm_();
	worm_(float initial_growth, uint id);
	void render_sphere(GLuint shader_program, uint sphere_triangles);
	void update(float time_tick);
	void make_player();
	uint get_id();
	void set_speed(float v);
	void set_pos(vec3 pos);
	void set_fix(bool fix);
	string get_ai_status();
}Worm;