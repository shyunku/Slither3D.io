#include "prey.hpp"

#include "util.hpp"
#include "glutil.hpp"
#include "log_manager.hpp"

mat4 Prey::get_model_matrix()
{
	float factor = 1 + 0.2f * cosf(2 * PI * float(glfwGetTime() / flickering_period));
	return
		mat4::translate(pos) *
		mat4::rotate(vec3(0, 1, 0), 0) *
		mat4::scale(radius * factor);
}

Prey::prey_()
{
	float virtual_rad = cbrt(randf(0, 1)) * (WORLD_BORDER_RADIUS - BORDER_PADDING_ARRANGE_OFFSET);
	vec3 rand_vec = get_random_vector();
	//vec3 rand_vec = get_restricted_vector(vec3(0, 1, 0), PI);

	pos = virtual_rad * rand_vec;
	radius = randf(MIN_RADIUS, MAX_RADIUS);
	amount = MIN_AMOUNT + (MAX_AMOUNT - MIN_AMOUNT) * (radius - MIN_RADIUS) / (MAX_RADIUS - MIN_RADIUS);

	flickering_period = randf(2.f, 4.f);
}
Prey::prey_(vec3 target_pos)
{
	float virtual_rad = cbrt(randf(0, 1)) * (WORLD_BORDER_RADIUS - BORDER_PADDING_ARRANGE_OFFSET);
	vec3 rand_vec = get_random_vector();

	pos = target_pos;
	radius = randf(MIN_RADIUS, MAX_RADIUS);
	amount = MIN_AMOUNT + (MAX_AMOUNT - MIN_AMOUNT) * (radius - MIN_RADIUS) / (MAX_RADIUS - MIN_RADIUS);

	flickering_period = randf(2.f, 4.f);
}
void Prey::render(GLuint shader_program, uint sphere_triangles)
{
	GLint uloc;

	uloc = get_validated_uniform_location(shader_program, "model_matrix");
	glUniformMatrix4fv(uloc, 1, GL_TRUE, get_model_matrix());

	uloc = get_validated_uniform_location(shader_program, "solid_color");
	glUniform4fv(uloc, 1, color);

	glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);
}
uint Prey::get_id()
{
	return object_id;
}