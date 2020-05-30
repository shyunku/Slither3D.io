#include "worm.hpp"

#include "glutil.hpp"
#include "util.hpp"
#include "log_manager.hpp"

extern GameEventLogger gevent;

/* -------------------------------------------------------- Worm Body -------------------------------------------------------- */

WormBody::worm_body_() {}
WormBody::worm_body_(vec3 initial_pos, vec3 direction)
{
	pos = initial_pos;
	radius = 0.6f;
	this->direction = direction;
}
mat4 WormBody::get_model_matrix()
{
	return
		mat4::translate(pos) *
		mat4::rotate(vec3(0, 1, 0), 0) *
		mat4::scale(radius);
}
void WormBody::move(float move_dist)
{
	pos += direction * move_dist;
}
void WormBody::track_parent(vec3 parent_pos)
{
	direction = (parent_pos - pos).normalize();
	pos = parent_pos - direction * radius * 0.8f;
}
void WormBody::set_size(float size)
{
	radius = size;
}

/* -------------------------------------------------------- Worm Body -------------------------------------------------------- */

void Worm::render_head(GLuint shader_program, uint sphere_triangles)
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
void Worm::render_body(GLuint shader_program, uint sphere_triangles)
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
vec3 Worm::get_restricted_vector(vec3 original, float max_difference)
{
	float co = cosf(max_difference);
	float so = sinf(max_difference);
	vec3 virtual_x_vector = vec3(1, 1, (co - original.x - original.y) / original.z).normalize();
	vec3 virtual_y_vector = original.cross(virtual_x_vector).normalize();
	float virtual_random_angle = rand_direction();
	return (co * original + so * (cosf(virtual_random_angle) * virtual_x_vector + sinf(virtual_random_angle) * virtual_y_vector)).normalize();
}
void Worm::update_body(float time_tick)
{
	uint body_num = (uint)(MIN_BODY_LENGTH + growth / BODY_GROWTH);
	int last_body_growth_rate = int(float(int(growth) % int(BODY_GROWTH)) / BODY_GROWTH);
	body_num += last_body_growth_rate > 0;
	uint cur_size = body.size();

	if (body_num < cur_size)
	{
		body.resize(body_num);
	}
	else if (body_num > cur_size)
	{
		WormBody last_body = cur_size == 0 ? head : body.at(cur_size - 1);
		uint count = 0;
		while (body_num != body.size())
		{
			body.push_back(WormBody(last_body.pos - INITIAL_BODY_GAP * last_body.direction * (float)++count, last_body.direction));
		}
	}

	// revalidate children pos

	head.move(time_tick * speed);
	for (vector<WormBody>::iterator iter = body.begin(); iter < body.end(); ++iter)
	{
		WormBody parent = iter == body.begin() ? head : *(iter - 1);
		iter->track_parent(parent.pos);
	}
}

Worm::worm_() {}
Worm::worm_(float initial_growth)
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
Worm::worm_(vec3 initial_pos, float initial_growth)
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
void Worm::render_sphere(GLuint shader_program, uint sphere_triangles)
{
	render_head(shader_program, sphere_triangles);
	render_body(shader_program, sphere_triangles);
}
void Worm::update(float time_tick)
{
	if (!is_player)
	{
		// Enemy worm AI (has priority)

		// 1. Avoid world border
		if (head.pos.length() > AVOID_WALL_START_RANGE)
		{
			float pos_rate = (head.pos.length() - AVOID_WALL_START_RANGE) / (WORLD_BORDER_RADIUS - speed - head.radius - AVOID_WALL_START_RANGE);
			if (rand_fraction() < pos_rate)
			{
				// More further from origin (more close to border), Probability to avoid wall increases
				// and more break angle to origin
				decided_direction = get_restricted_vector(-head.direction, PI / 2 * (2 - pos_rate));
			}
		}
		// 2. Defense/Offense others
		else if (false)
		{

		}
		// 3. Seek for Prey
		else if (false)
		{

		}
		// Last. Standard movement
		else
		{
			elapsed_direction_change_timestamp += time_tick;
			if (elapsed_direction_change_timestamp > auto_direction_change_period)
			{
				elapsed_direction_change_timestamp = 0;
				//decided_direction = get_restricted_vector(head.direction, randf(MIN_DIRECTION_CHANGE, MAX_DIRECTION_CHANGE));
				decided_direction = get_random_vector();

				auto_direction_change_period = randf(0.3f, 3.f);
			}
		}
	}

	head.direction += (decided_direction - head.direction) * 0.01f;
	head.direction = head.direction.normalize();
	update_body(time_tick);

	if (head.pos.length() > WORLD_BORDER_RADIUS)
	{
		gevent.add("Worm[UID: " + UID + "] arranged as out of range! (over: " + to_string(head.pos.length() - WORLD_BORDER_RADIUS));
	}

	// resizing
	float new_size = (logf(growth + 200) + 1) / 2 - logf(sqrtf(200));
	head.set_size(new_size);
	for (vector<WormBody>::iterator iter = body.begin(); iter != body.end(); ++iter)
	{
		iter->set_size(new_size);
	}
}
void Worm::make_player()
{
	is_player = true;
}
string Worm::get_uid()
{
	return UID;
}
void Worm::set_speed(float v)
{
	extern CommandConsole console;
	if (v > MAX_SPEED)
	{
		speed = MAX_SPEED;
		console.add(format_string("Can't set speed over MAX_SPEED(%.2f)!", MAX_SPEED), _WARNING_);
	}
	else
	{
		speed = v;
	}

	console.add(format_string("Player speed set to %.2f.", speed), _VERBOSE_);
}