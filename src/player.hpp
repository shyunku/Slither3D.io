#pragma once
#include "cgmath.h"
#include "camera.hpp"

extern void print_vec3(vec3 v);
extern void print_vec3(vec3 v, const char* msg);

typedef class player_
{
private:
	float		default_velocity = 0.0005f;
	bool		fixed = true;
	inline void update_pos()
	{
		if (fixed) return;
		pos += camera.look_direction.normalize() * default_velocity;
	}
public:
	vec3		pos = vec3(0, 4, 12);
	vec3		direction = vec3(0, 0, -1);
	Camera		camera;
	player_()
	{
	}
	inline void update()
	{
		update_pos();
		camera.update(pos);
	}
	inline void set_fix(bool fix)
	{
		fixed = fix;
	}
}Player;