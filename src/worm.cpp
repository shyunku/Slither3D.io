#include "worm.hpp"
#include "glutil.hpp"
#include "util.hpp"
#include "log_manager.hpp"
#include "game_moderator.hpp"
#include "prey.hpp"

extern GameEventLogger gevent;
extern CommandConsole console;
extern GameModerator game_moderator;

const string AI_STATUS_CODE[6] = { "EVADE_BORDER", "DEFENSE", "OFFENSE", "SEEK_PREY", "STANDARD", "NONE_AI" };
const double TICK_MOMENTUM_FACTOR = 7.2f;
const float CREATE_WORM_BORDER_RADIUS_FACTOR = 0.85f;

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

		uloc = get_validated_uniform_location(shader_program, "solid_color");
		glUniform4fv(uloc, 1, color);

		//uloc = get_validated_uniform_location(shader_program, "shadow_direction");
		//glUniform3fv(uloc, 1, body.at(i).direction);

		glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);
	}
}
void Worm::update_body(float time_tick)
{
	boost_poof();
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

	head.move(time_tick * speed * (boosting ? BOOST_SPEED_FACTOR : 1));
	for (vector<WormBody>::iterator iter = body.begin(); iter < body.end(); ++iter)
	{
		WormBody parent = iter == body.begin() ? head : *(iter - 1);
		iter->track_parent(parent.pos);
	}
}

Worm::worm_() {}
Worm::worm_(float initial_growth, uint id)
{
	float theta = rand_direction();
	float alpha = rand_direction();
	vec3 initd = get_random_vector();

	this->object_id = id;

	head.direction = initd;
	float head_pos_rad = randf(0, CREATE_WORM_BORDER_RADIUS_FACTOR * WORLD_BORDER_RADIUS);
	//float head_pos_rad = 0;
	head = WormBody(head_pos_rad * get_random_vector(), initd);
	
	color = vec4(randf(0.5f, 1), randf(0.5f, 1), randf(0.5f, 1), 1);

	growth = initial_growth;
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
		bool ai_decide = false;
		// Enemy worm AI (has priority)

		// 1. Evade world border
		if (!ai_decide)
		{
			if (head.pos.length() > AVOID_WALL_START_RANGE)
			{
				if (ai_status != EVADE_BORDER)
				{
					auto_direction_change_period = 0;
				}

				elapsed_evade_wall_timestamp += time_tick;
				if (elapsed_evade_wall_timestamp > auto_direction_change_period)
				{
					float pos_rate = (head.pos.length() - AVOID_WALL_START_RANGE) / (WORLD_BORDER_RADIUS - AVOID_WALL_START_RANGE);
					// More further from origin (more close to border), Probability to avoid wall increases
					// and more break angle to origin

					elapsed_evade_wall_timestamp = 0;
					float restrict_angle = (1.f - refinef(pos_rate)) * PI;
					decided_direction = get_restricted_vector(-head.pos.normalize(), restrict_angle);

					//gevent.add(format_string("[%d] Pos_rate: %7.3f %7.3f", object_id, pos_rate, restrict_angle*180/PI));

					auto_direction_change_period = 1.f;
				}
				ai_status = EVADE_BORDER;
				ai_decide = true;
			}
		}
		// 2. Defense/Offense others
		if (!ai_decide)
		{
			ai_status = DEFENSE;
			//ai_decide = true;
		}
		// 3. Seek for Prey
		if (!ai_decide)
		{
			ai_status = SEEK_PREY;
			//ai_decide = true;
		}
		// Last. Standard movement
		if (!ai_decide)
		{
			elapsed_standard_timestamp += time_tick;
			if (elapsed_standard_timestamp > auto_direction_change_period)
			{
				elapsed_standard_timestamp = 0;
				//decided_direction = get_restricted_vector(head.direction, randf(MIN_DIRECTION_CHANGE, MAX_DIRECTION_CHANGE));
				decided_direction = get_random_vector();

				auto_direction_change_period = randf(0.3f, 3.f);
			}
			ai_status = STANDARD;
			ai_decide = true;
		}
	}

	head.direction += (decided_direction - head.direction) * TICK_MOMENTUM_FACTOR * time_tick;
	head.direction = head.direction.normalize();
	update_body(time_tick);

	if (head.pos.length() > WORLD_BORDER_RADIUS - head.radius)
	{
		gevent.add(format_string("[%d] out of range", object_id));
	}

	// resizing
	float new_size = (logf(growth + 600) + 1) / 2 - logf(sqrtf(600));
	head.set_size(new_size);
	for (vector<WormBody>::iterator iter = body.begin(); iter != body.end(); ++iter)
	{
		iter->set_size(new_size);
	}

	if (growth < 0)
	{
		growth = 0;
	}
}
void Worm::make_player()
{
	is_player = true;
	ai_status = NONE_AI;
}
uint Worm::get_id()
{
	return object_id;
}
void Worm::set_speed(float v)
{
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
void Worm::set_pos(vec3 pos)
{
	head.pos = pos;
	console.add("Player teleported to " + get_vec3_pair_string(head.pos)+".", _VERBOSE_);
}
void Worm::set_fix(bool fix)
{
	speed = fix ? 0 : DEFAULT_SPEED;
	console.add("Player position fixed.", _VERBOSE_);
}
string Worm::get_ai_status()
{
	return AI_STATUS_CODE[ai_status];
}
void Worm::boost_poof()
{
	if (boosting && growth > 0)
	{
		growth -= 0.2f;
		if (growth < 0)
		{
			growth = 0;
			return;
		}

		WormBody tail = body.back();
		int ran = (int)rand();
		if (ran % 5 == 1) 
		{
			game_moderator.ingame_object_manager.push_new_prey_pos(tail.pos);
		}
	}
}
bool Worm::is_meetable(worm_ other)
{
	return distance(head.pos, other.head.pos) <= (other.get_stretched_length() + get_stretched_length());
}
bool Worm::detect_death(worm_ other)
{
	if (object_id != other.get_id())
	{
		for (vector<WormBody>::iterator iter = other.body.begin(); iter != other.body.end(); ++iter)
		{
			if (distance(head.pos, iter->pos) <= (head.radius + iter->radius))
			{
				return true;
			}
		}
	}
	return false;
}
bool Worm::detect_eat_prey(Prey prey)
{
	return distance(head.pos, prey.pos) < (head.radius + prey.radius);
}
float Worm::get_camera_distance()
{
	return growth / BODY_GROWTH;
}
void Worm::enable_boost()
{
	if (growth > 0)
	{
		boosting = true;
	}
}
void Worm::disable_boost()
{
	boosting = false;
}
float Worm::get_stretched_length()
{
	return head.radius * 2 + body.size() * INITIAL_BODY_GAP;
}