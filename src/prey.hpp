#pragma once
#include "cgmath.h"
#include "cgut.h"
#include "util.hpp"

extern const float WORLD_BORDER_RADIUS;

typedef class prey_
{
private:
	const float BORDER_PADDING_ARRANGE_OFFSET = 0.01f * WORLD_BORDER_RADIUS;
	const float MAX_RADIUS = 0.3f;
	const float MIN_RADIUS = 0.1f;
	const float MAX_AMOUNT = 4.f;
	const float MIN_AMOUNT = 1.f;

	float flickering_period;
	uint object_id;
	mat4 get_model_matrix();
	vec4 color = vec4(randf(0.5f, 1), randf(0.5f, 1), randf(0.5f, 1), 0.6f);
public:
	vec3 pos;
	float radius;
	float amount;

	prey_();
	prey_(vec3 target_pos);
	void render(GLuint shader_program, uint sphere_triangles);
	uint get_id();
}Prey;