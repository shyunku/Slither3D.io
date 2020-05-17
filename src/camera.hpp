#pragma once
#include "cgmath.h"		// slee's simple math library

typedef class camera_
{
private:
	const float		fov = PI / 4.0f;
	const float		dNear = 1.0f;
	const float		dFar = 1000.f;
	float			aspect_ratio;

	inline void revalidate_all()
	{
		StandardViewModel svm = view_matrix.parse();
		look_direction = svm.look_at.normalize();
		up = svm.up.normalize();
	}
public:
	mat4			view_matrix;
	mat4			projection_matrix;
	
	vec3			eye = vec3(0,0,3);
	vec3			look_direction = vec3(0,0,-1);
	vec3			up = vec3(0, 1, 0);
	camera_()
	{
	}
	void update(vec3 camera_pos)
	{
		eye = camera_pos;
		view_matrix = mat4::look_at(eye, eye + look_direction, up);
	}
	void set_aspect_ratio(float ratio)
	{
		this->aspect_ratio = ratio;
	}
	void revalidate_projection_matrix()
	{
		this->projection_matrix = mat4::perspective(fov, aspect_ratio, dNear, dFar);
	}
	inline void direction_adjust(float x, float y)
	{
		vec3 right = look_direction.cross(up).normalize();
		view_matrix *= mat4::rotate(up, x * 0.001f)*mat4::rotate(right, y * 0.001f);
		revalidate_all();
	}
}Camera;