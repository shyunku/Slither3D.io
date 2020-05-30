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
extern vec3 get_random_vector();

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
	mat4 get_model_matrix()
	{
		return
			mat4::translate(pos) *
			mat4::rotate(vec3(0, 1, 0), 0) *
			mat4::scale(radius);
	}
	void move(float move_dist)
	{
		pos += direction * move_dist;
	}
	void track_parent(vec3 parent_pos)
	{
		direction = (parent_pos - pos).normalize();
		pos = parent_pos - direction * radius * 0.8f;
	}
	void set_size(float size)
	{
		radius = size;
	}
}WormBody;

typedef class worm_
{
private:
	const float			INITIAL_BODY_GAP = 0.6f;
	const float			BODY_GROWTH = 50.f;
	const float			MIN_BODY_LENGTH = 10;
	const float			MIN_DIRECTION_CHANGE = PI / 4;
	const float			MAX_DIRECTION_CHANGE = PI / 2;
	vec4				color;
	string				UID;
	vector<WormBody>	body;
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
	vec3 get_restricted_vector(vec3 original, float max_difference)
	{
		float co = cosf(max_difference);
		float so = sinf(max_difference);
		vec3 virtual_x_vector = vec3(1, 1, (co - original.x - original.y) / original.z).normalize();
		vec3 virtual_y_vector = original.cross(virtual_x_vector).normalize();
		float virtual_random_angle = rand_direction();
		return (co * original + so * (cosf(virtual_random_angle) * virtual_x_vector + sinf(virtual_random_angle) * virtual_y_vector)).normalize();
	}
	void update_body(float time_tick)
	{
		uint body_num = (uint)(MIN_BODY_LENGTH + growth / BODY_GROWTH);
		int last_body_growth_rate = int(float(int(growth) % int(BODY_GROWTH)) / BODY_GROWTH);
		body_num += last_body_growth_rate > 0;
		uint cur_size = body.size();

		if (body_num < cur_size)
		{
			body.resize(body_num);
		}
		else if(body_num > cur_size)
		{
			WormBody last_body = cur_size == 0 ? head : body.at(cur_size - 1);
			uint count = 0;
			while (body_num != body.size())
			{
				body.push_back(WormBody(last_body.pos - INITIAL_BODY_GAP * last_body.direction * (float)++count, last_body.direction));
			}
		}

		// revalidate children pos

		head.move(time_tick*speed);
		for (vector<WormBody>::iterator iter = body.begin(); iter < body.end(); ++iter)
		{
			WormBody parent = iter == body.begin() ? head : *(iter - 1);
			iter->track_parent(parent.pos);
		}
	}
public:
	float				speed = 5.f;
	WormBody			head;
	bool				is_player = false;
	float				growth;

	// AI area
	float				auto_direction_change_period = AUTO_DIRECTION_CHANGE_PERIOD_KEYWORD;
	float				elapsed_direction_change_timestamp = 0;
	vec3				decided_direction = vec3(0);

	/*
		size(radius) ÃÖ¼Ò 0.5
		growth - size:
		0 - 0.5
		300 - 1.0
		1000 - 1.5
		0.5ln(x+200) - ln(sqrt(200)) + 0.5
	*/

	worm_() {}
	worm_(float initial_growth)
	{
		float theta = rand_direction();
		float alpha = rand_direction();
		vec3 initd = get_random_vector();

		head.direction = initd;
		float head_pos_rad = randf(0, WORLD_BORDER_RADIUS);
		head = WormBody(head_pos_rad * get_random_vector(), initd);
		color = vec4(randf(0.2f, 1), randf(0.2f, 1), randf(0.2f, 1), 1);

		growth = initial_growth;
		UID = format_string("%04d-%04d", randi(0, 10000), randi(0, 10000));

		update_body(0);
	}
	worm_(vec3 initial_pos, float initial_growth)
	{
		float theta = rand_direction();
		float alpha = rand_direction();
		vec3 initd = get_random_vector();

		head.direction = initd;
		head = WormBody(initial_pos, initd);
		color = vec4(randf(0, 1), randf(0, 1), randf(0, 1), 1);

		growth = initial_growth;
		UID = format_string("%04d-%04d", randi(0, 10000), randi(0, 10000));

		update_body(0);
	}
	void render_sphere(GLuint shader_program, uint sphere_triangles)
	{
		render_head(shader_program, sphere_triangles);
		render_body(shader_program, sphere_triangles);
	}
	void update(float time_tick)
	{
		if (!is_player)
		{
			// Enemy worm AI

			// move algorithm as AI
			elapsed_direction_change_timestamp += time_tick;
			if (elapsed_direction_change_timestamp > auto_direction_change_period)
			{
				elapsed_direction_change_timestamp = 0;
				//decided_direction = get_restricted_vector(head.direction, randf(MIN_DIRECTION_CHANGE, MAX_DIRECTION_CHANGE));
				decided_direction = get_random_vector();

				auto_direction_change_period = AUTO_DIRECTION_CHANGE_PERIOD_KEYWORD;
			}
		}

		head.direction += (decided_direction - head.direction) * 0.01f;
		head.direction = head.direction.normalize();
		update_body(time_tick);

		if (head.pos.length() > WORLD_BORDER_RADIUS)
		{
			extern GameEventLogger gevent;
			gevent.add("Worm[UID: " + UID + "] arranged as out of range!");
		}

		// resizing
		float new_size = (logf(growth + 200) + 1) / 2 - logf(sqrtf(200));
		head.set_size(new_size);
		for (vector<WormBody>::iterator iter = body.begin(); iter != body.end(); ++iter)
		{
			iter->set_size(new_size);
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