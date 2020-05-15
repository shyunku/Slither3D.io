#pragma once
#include "cgmath.h"
#include "camera.hpp"

typedef class player_
{
private:
	float		default_velocity = 0.01f;
	inline void update_pos()
	{
		pos += direction.normalize() * default_velocity;
	}
	inline void update_camera()
	{

	}
public:
	vec3		pos = vec3(0, 0, 5);
	vec3		direction = vec3(0, 0, 0);
	vec3		up = vec3(0,1,0);
	Camera		camera = Camera(pos, vec3(0), up);
	player_()
	{
	}
	inline void update()
	{
		update_pos();
		update_camera();
	}
}Player;