#pragma once
#include "cgut.h"
#include "cgmath.h"
#include "log_manager.hpp"

#define AUTO_DIRECTION_CHANGE_PERIOD_KEYWORD randf(0.3f, 3.f)

using namespace std;

extern inline float randf(float LO, float HI);
extern inline float rand_direction();
extern inline int randi(int LO, int HI);
extern GLuint get_validated_uniform_location(GLuint shader_program, const char* name);
extern const float WORLD_BORDER_RADIUS;
extern void print_vec3(vec3 v);
extern string format_string(const string fmt, ...);
extern string get_vec3_string(vec3 v);

typedef class worm_body_
{
public:
	vec3	pos;
	float	radius;
	vec3	direction;
	worm_body_(){}
	worm_body_(vec3 initial_pos, vec3 direction)
	{
		pos = initial_pos;
		radius = 0.6f;
		this->direction = direction;
	}
	worm_body_(vec3 initial_pos, float size, vec3 direction)
	{
		pos = initial_pos;
		radius = size;
		this->direction = direction;
	}
	mat4 get_model_matrix()
	{
		return
			mat4::translate(pos) *
			mat4::rotate(vec3(0, 1, 0), 0) *
			mat4::scale(radius);
	}
	void update_and_move(vec3 prev_parent_pos, float move_distance)
	{
		direction = (prev_parent_pos - pos).normalize();
		pos += direction * move_distance;
	}
}WormBody;

typedef class worm_
{
private:
	const float			BODY_GAP = 0.6f;
	const float			MIN_DIRECTION_CHANGE = PI / 4;
	const float			MAX_DIRECTION_CHANGE = PI / 2;
	vec4				color;
	string				UID;
	void render_head(GLuint shader_program, uint sphere_triangles)
	{
		GLint uloc;

		uloc = get_validated_uniform_location(shader_program, "model_matrix");
		glUniformMatrix4fv(uloc, 1, GL_TRUE, head.get_model_matrix());

		uloc = get_validated_uniform_location(shader_program, "is_using_solid_color");
		glUniform1i(uloc, true);

		uloc = get_validated_uniform_location(shader_program, "solid_color");
		glUniform4fv(uloc, 1, color);

		glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);
	}
	void render_body(GLuint shader_program, uint sphere_triangles)
	{
		GLint uloc;

		for (size_t i = 0; i < body.size(); i++)
		{
			uloc = get_validated_uniform_location(shader_program, "model_matrix");
			glUniformMatrix4fv(uloc, 1, GL_TRUE, body.at(i).get_model_matrix());

			uloc = get_validated_uniform_location(shader_program, "is_using_solid_color");
			glUniform1i(uloc, true);

			uloc = get_validated_uniform_location(shader_program, "solid_color");
			glUniform4fv(uloc, 1, color);

			glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);
		}
	}
	vec3 get_random_vector()
	{
		float theta = randf(0, 2 * PI);
		float alpha = randf(0, 2 * PI);
		return vec3(cos(theta) * cos(alpha), sin(theta) * cos(alpha), sin(alpha)).normalize();
	}
	vec3 get_restricted_vector(vec3 original, float max_difference)
	{
		float co = cosf(max_difference);
		float so = sinf(max_difference);
		vec3 virtual_x_vector = vec3(1, 1, (co - original.x - original.y) / original.z).normalize();
		vec3 virtual_y_vector = original.cross(virtual_x_vector).normalize();
		float virtual_random_angle = rand_direction();
		return (co * original + so * (cosf(virtual_random_angle) * virtual_x_vector + sinf(virtual_random_angle) * virtual_y_vector)).normalize();
	}
public:
	float				speed = 5.f;
	WormBody			head;
	vector<WormBody>	body;
	bool				is_player = false;

	// AI area
	float				auto_direction_change_period = AUTO_DIRECTION_CHANGE_PERIOD_KEYWORD;
	float				elapsed_direction_change_timestamp = 0;
	vec3				decided_direction = vec3(0);

	worm_() {}
	worm_(uint initial_body_num)
	{
		float theta = rand_direction();
		float alpha = rand_direction();
		vec3 initd = get_random_vector();

		head.direction = initd;

		float head_pos_rad = randf(0, WORLD_BORDER_RADIUS);
		head = WormBody(head_pos_rad * get_random_vector(), initd);

		color = vec4(randf(0.2f, 1), randf(0.2f, 1), randf(0.2f, 1), 1);

		for (size_t i = 1; i <= initial_body_num; i++)
		{
			WormBody child = WormBody(head.pos - BODY_GAP * head.direction * (float)i, initd);
			body.push_back(child);
		}

		UID = format_string("%04d-%04d", randi(0, 10000), randi(0, 10000));
	}
	worm_(vec3 initial_pos, uint initial_body_num)
	{
		float theta = rand_direction();
		float alpha = rand_direction();
		vec3 initd = get_random_vector();

		head.direction = initd;

		head = WormBody(initial_pos, initd);

		color = vec4(randf(0, 1), randf(0, 1), randf(0, 1), 1);

		for (size_t i = 1; i <= initial_body_num; i++)
		{
			WormBody child = WormBody(head.pos - BODY_GAP * head.direction * (float)i, initd);
			body.push_back(child);
		}
		UID = format_string("%04d-%04d", randi(0, 10000), randi(0, 10000));
	}
	void render_sphere(GLuint shader_program, uint sphere_triangles)
	{
		render_head(shader_program, sphere_triangles);
		render_body(shader_program, sphere_triangles);
	}
	void update(float time_tick)
	{
		extern GameLogDrawer gld;
		if (!is_player)
		{
			// ���߿� ���� �� ��
			using wit = vector<WormBody>::iterator;
			float move_dist = speed * time_tick;

			for (wit iter = body.end() - 1; iter > body.begin(); --iter)
			{
				wit parent = iter - 1;
				iter->update_and_move(parent->pos, move_dist);
			}
			wit eldest_child = body.begin();
			eldest_child->update_and_move(head.pos, move_dist);

			// move algorithm as AI
			elapsed_direction_change_timestamp += time_tick;
			if (elapsed_direction_change_timestamp > auto_direction_change_period)
			{
				elapsed_direction_change_timestamp = 0;
				//head.direction = get_restricted_vector(head.direction, randf(MIN_DIRECTION_CHANGE, MAX_DIRECTION_CHANGE));
				auto_direction_change_period = AUTO_DIRECTION_CHANGE_PERIOD_KEYWORD;
				decided_direction = get_random_vector();
				gld.add("Worm[UID: " + UID + "] changed its direction");
			}
			head.direction += (decided_direction - head.direction) * 0.01f;
			head.update_and_move(head.direction + head.pos, move_dist);
		}
	}
	void make_player()
	{
		is_player = true;
	}
	string get_uid()
	{
		return UID;
	}
}Worm;