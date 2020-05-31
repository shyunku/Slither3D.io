#pragma once
#include "cgmath.h"
#include "cgut.h"

extern const float WORLD_BORDER_RADIUS;
extern GLuint get_validated_uniform_location(GLuint shader_program, const char* name);

typedef class world_border_
{
private:
	mat4 get_model_matrix()
	{
		return
			mat4::translate(pos) *
			mat4::rotate(vec3(0, 1, 0), 0) *
			mat4::scale(radius);
	}
public:
	vec3	pos = vec3(0);
	float	radius = WORLD_BORDER_RADIUS;
	world_border_() {}
	void render_sphere(GLuint shader_program, uint sphere_triangles)
	{
		GLint uloc = get_validated_uniform_location(shader_program, "model_matrix");

		// Self-render
		uloc = get_validated_uniform_location(shader_program, "model_matrix");
		glUniformMatrix4fv(uloc, 1, GL_TRUE, get_model_matrix());

		uloc = get_validated_uniform_location(shader_program, "solid_color");
		glUniform4fv(uloc, 1, vec4(0.6f, 0.6f, 0.6f, 0.25f));

		glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);
	}
}WorldBorder;