#pragma once
#include "cgmath.h"		// slee's simple math library

typedef class camera_
{
private:
	const float		fov = PI / 4.0f;
	const float		dNear = 1.0f;
	const float		dFar = 1000.f;
	float			aspect_ratio;
public:
	mat4			view_matrix;
	mat4			projection_matrix;
	camera_()
	{
		vec3 eye = vec3(0, 0, 3);
		vec3 at = vec3(0);
		vec3 up = vec3(0, 1, 0);
		view_matrix = mat4::look_at(eye, at, up);
	}
	camera_(vec3 initial_eye, vec3 initial_at)
	{
		vec3 up = vec3(0, 1, 0);
		view_matrix = mat4::look_at(initial_eye, initial_at, up);
	}
	void set_aspect_ratio(float ratio)
	{
		this->aspect_ratio = ratio;
	}
	void revalidate_projection_matrix()
	{
		this->projection_matrix = mat4::perspective(fov, aspect_ratio, dNear, dFar);
	}
}Camera;