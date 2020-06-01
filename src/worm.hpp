#pragma once
#include "cgmath.h"
#include "cgut.h"

#include "prey.hpp"
#include <unordered_map>

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
	const float			BODY_GROWTH = 8.f;
	const float			MIN_BODY_LENGTH = 10;
	const float			MIN_DIRECTION_CHANGE = PI / 4;
	const float			MAX_DIRECTION_CHANGE = PI / 2;
	const float			AVOID_WALL_START_RANGE = WORLD_BORDER_RADIUS * 2 / 3;

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
	vec3				decided_direction = vec3(0);

	// AI area
	AI_STATUS			ai_status = STANDARD;

	//

	// STANDARD
	float				auto_direction_change_period = 0;
	float				elapsed_evade_wall_timestamp = 0;
	float				elapsed_standard_timestamp = 0;

	worm_();
	worm_(float initial_growth, uint id);
	void render_sphere(GLuint shader_program, uint sphere_triangles);
	void update(float time_tick, unordered_map<uint, Prey> preys);
	void make_player();
	uint get_id();
	void set_speed(float v);
	void set_pos(vec3 pos);
	void set_fix(bool fix);
	string get_ai_status();
	void boost_poof();
	void poof();
	bool detect_death(worm_ other);
	bool detect_eat_prey(Prey prey);
	float get_camera_distance();
	void enable_boost();
	void disable_boost();
	bool is_meetable(worm_ other);
	float get_stretched_length();
}Worm;