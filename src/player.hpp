#pragma once
#include "cgmath.h"
#include "camera.hpp"

extern void print_vec3(vec3 v);
extern void print_vec3(vec3 v, const char* msg);

typedef class player_
{
private:
	float		default_velocity = 0.01f;
	inline void update_pos()
	{
		pos += direction.normalize() * default_velocity;
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
		//update_pos();
		camera.update(pos);
	}
}Player;