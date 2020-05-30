#pragma once
#include "cgmath.h"
#include "cgut.h"

extern const float WORLD_BORDER_RADIUS;

typedef class camera_
{
private:
	const float		fov = PI / 4.0f;
	const float		dNear = 1.0f;
	const float		dFar = WORLD_BORDER_RADIUS * 3;
	float			aspect_ratio;

	inline void revalidate_all();
public:
	mat4			view_matrix;
	mat4			projection_matrix;
	
	vec3			eye = vec3(0,0,3);
	vec3			look_direction = vec3(0,0,-1);
	vec3			up = vec3(0, 1, 0);
	camera_();
	void update(vec3 camera_pos);
	void set_aspect_ratio(float ratio);
	void revalidate_projection_matrix();
	void look_direction_adjust(float x, float y);
	void set_looking_at(vec3 look);
}Camera;