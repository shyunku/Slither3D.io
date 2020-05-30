#pragma once
#include "cgut.h"
#include "cgmath.h"
#include "util.hpp"
#include "glutil.hpp"
#include "log_manager.hpp"

extern const float WORLD_BORDER_RADIUS;

typedef class prey_
{
private:
	const float BORDER_PADDING_ARRANGE_OFFSET = 0.01f * WORLD_BORDER_RADIUS;
	const float MAX_RADIUS = 0.3f;
	const float MIN_RADIUS = 0.1f;
	const float MAX_AMOUNT = 15.f;
	const float MIN_AMOUNT = 5.f;
	float flickering_period;
	mat4 get_model_matrix()
	{
		float factor = 1 + 0.3f * cosf(2 * PI * float(glfwGetTime() / flickering_period));
		return
			mat4::translate(pos) *
			mat4::rotate(vec3(0, 1, 0), 0) *
			mat4::scale(radius * factor);
	}
public:
	vec3 pos;
	float radius;
	float amount;
	prey_()
	{
		float virtual_rad = randf(0, WORLD_BORDER_RADIUS - BORDER_PADDING_ARRANGE_OFFSET);
		vec3 rand_vec = get_random_vector();

		pos = virtual_rad * rand_vec;
		radius = randf(MIN_RADIUS, MAX_RADIUS);
		amount = MIN_AMOUNT + (MAX_AMOUNT - MIN_AMOUNT) * (radius - MIN_RADIUS) / (MAX_RADIUS - MIN_RADIUS);

		flickering_period = randf(1.2f, 3.f);
	}
	void render(GLuint shader_program, uint sphere_triangles)
	{
		GLint uloc;

		uloc = get_validated_uniform_location(shader_program, "model_matrix");
		glUniformMatrix4fv(uloc, 1, GL_TRUE, get_model_matrix());

		//uloc = get_validated_uniform_location(shader_program, "solid_color");
		//glUniform4fv(uloc, 1, vec4(1, 0, 1, 1));

		glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);
	}
}Prey;