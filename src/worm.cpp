#include "worm.hpp"
#include "glutil.hpp"
#include "util.hpp"
#include "log_manager.hpp"
#include "game_moderator.hpp"
#include "prey.hpp"
#include <unordered_map>

extern GameEventLogger gevent;
extern CommandConsole console;
extern GameModerator game_moderator;

const string AI_STATUS_CODE[7] = { "EVADE_BORDER", "DEFENSE", "OFFENSE", "SEEK_PREY", "SEEK_PREY_BOOST", "STANDARD", "NONE_AI" };
const double TICK_MOMENTUM_FACTOR = 7.2f;
const float CREATE_WORM_BORDER_RADIUS_FACTOR = 0.85f;
const float POOF_STACK_MAX_GAGE = 3.0f;
const float SEEK_PREY_RANGE = 8.0f; //8.0
const float	PREY_CREATE_DELAY = 10.f;

const float BOOST_BRIGHTER_FACTOR = 0.3f;

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
void WormBody::track_parent(vec3 parent_pos, float body_gap)
{
	direction = (parent_pos - pos).normalize();
	pos = parent_pos - direction * body_gap;
}
void WormBody::track_parent(vec3 parent_pos, float body_gap, float rate)
{
	direction = (parent_pos - pos).normalize();
	
	pos = parent_pos - direction * body_gap * rate;
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
	glUniform4fv(uloc, 1, get_brighter_color(get_current_color(), 0.3f));

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
		glUniform4fv(uloc, 1, get_current_color());

		//uloc = get_validated_uniform_location(shader_program, "shadow_direction");
		//glUniform3fv(uloc, 1, body.at(i).direction);

		glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);
	}
}
void Worm::update_body(float time_tick)
{
	boost_poof();
	uint body_num = (uint)(MIN_BODY_LENGTH + growth / BODY_GROWTH);
	float last_body_growth_rate = float(growth - (body_num - MIN_BODY_LENGTH) * BODY_GROWTH) / BODY_GROWTH;
	uint cur_size = body.size();
	body_num += last_body_growth_rate > 0;

	if (body_num < cur_size)
	{
		// body size down
		body.resize(body_num);
	}
	else if (body_num > cur_size)
	{
		// body expands
		uint count = 0;
		WormBody last_body = body.empty() ? head : body.back();
		while (body_num != body.size())
		{
			body.push_back(WormBody(last_body.pos - get_body_gap() * last_body.direction * float(++count), last_body.direction));
		}
	}

	// revalidate children pos
	head.move(time_tick * speed * (boosting ? BOOST_SPEED_FACTOR : 1));
	//cout << "vector check" << endl;
	for (vector<WormBody>::iterator iter = body.begin(); iter != body.end(); ++iter)
	{
		WormBody parent = ((iter == body.begin()) ? head : *(iter - 1));
		if (last_body_growth_rate > 0 && iter == std::prev(body.end(),1))
		{
			iter->track_parent(parent.pos, get_body_gap(), last_body_growth_rate);
		}
		else
		{
			iter->track_parent(parent.pos, get_body_gap());
		}
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
	float head_pos_rad = cbrt(randf(0, 1)) * CREATE_WORM_BORDER_RADIUS_FACTOR * WORLD_BORDER_RADIUS;
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
void Worm::update(float time_tick, unordered_map<uint, Prey> preys)
{
	if (!is_player)
	{
		static bool allow_other_ai = true;
		bool ai_decide = false;
		// Enemy worm AI (has priority)

		// 1. Evade world border - check range
		elapsed_evade_wall_timestamp += time_tick;
		if (elapsed_evade_wall_timestamp > force_direction_change_period)
		{
			elapsed_evade_wall_timestamp = 0;

			// Check range
			if (head.pos.length() > (WORLD_BORDER_RADIUS - speed - head.radius))
			{
				decided_direction = - head.pos.normalize();

				ai_status = EVADE_BORDER;
				ai_decide = true;
				allow_other_ai = false;
			}
			else
			{
				allow_other_ai = true;
			}
		}
		else if(!allow_other_ai)
		{
			// lock other ai
			ai_status = EVADE_BORDER;
			ai_decide = true;
		}

		// 2. Defense/Offense others
		if (!ai_decide)
		{
			ai_status = DEFENSE;
			//ai_decide = true;
		}
		// 3. Seek for Prey & boost
		if (!ai_decide)
		{
			float min_dist = 1000000.f;
			bool found_prey = false;
			vec3 closest_pos;

			for (unordered_map<uint, Prey>::iterator iter = preys.begin(); iter != preys.end(); ++iter)
			{
				float dist = distance(iter->second.pos, head.pos);
				if (dist < SEEK_PREY_RANGE)
				{
					if (dist < min_dist)
					{
						min_dist = dist;
						closest_pos = iter->second.pos;
						found_prey = true;
					}
				}
			}

			if (found_prey)
			{
				decided_direction = (closest_pos - head.pos).normalize();
				ai_status = SEEK_PREY;
				ai_decide = true;
			}
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

				auto_direction_change_period = randf(0.3f, 7.f);
			}
			ai_status = STANDARD;
			ai_decide = true;
		}
	}

	head.direction += (decided_direction - head.direction) * TICK_MOMENTUM_FACTOR * time_tick;
	head.direction = head.direction.normalize();
	update_body(time_tick);

	revalidate_body_radius();

	if (boosting && growth <= 0)
	{
		disable_boost();
	}
}
void Worm::revalidate_body_radius()
{
	// resizing
	float new_size = (logf(growth + 600) + 1) / 2 - logf(sqrtf(600));
	head.set_size(new_size);
	for (vector<WormBody>::iterator iter = body.begin(); iter != body.end(); ++iter)
	{
		iter->set_size(new_size);
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
	static float poof_gage = 0;
	if (boosting && growth > 0)
	{
		poof_gage += 0.1f;

		if (poof_gage >= POOF_STACK_MAX_GAGE)
		{
			poof_gage -= POOF_STACK_MAX_GAGE;
			poof();
		}
	}
}
void Worm::poof()
{
	growth -= POOF_STACK_MAX_GAGE;
	if (growth < 0)
	{
		growth = 0;
		return;
	}
	WormBody tail = body.back();
	game_moderator.ingame_object_manager.push_delay_create_prey(tail.pos, POOF_STACK_MAX_GAGE, float(glfwGetTime()) + PREY_CREATE_DELAY);
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
		poof();
		boosting = true;
	}
}
void Worm::disable_boost()
{
	boosting = false;
}
float Worm::get_body_gap()
{
	return head.radius * 0.8f;
}
float Worm::get_stretched_length()
{
	return head.radius * 2 + body.size() * get_body_gap();
}
void Worm::spread_prey_before_die()
{
	float avg = (growth / body.size()) + 1.f;
	for (vector<WormBody>::iterator iter = body.begin(); iter != body.end(); ++iter)
	{
		uint prey_num_per_body = rand() % 5 + 1;
		float avg_avg = avg / prey_num_per_body;
		for (uint i = 0; i < prey_num_per_body; i++)
		{
			float virtual_rad = cbrt(randf(0, 1)) * iter->radius;
			vec3 rand_vec = get_random_vector();
			float amount = (1 + 0.3f * randf(0, 1)) * avg_avg;
			game_moderator.ingame_object_manager.push_new_prey_pos(iter->pos + rand_vec * virtual_rad, amount);
		}
	}
}
vec4 Worm::get_current_color()
{
	return boosting ? get_brighter_color(color, BOOST_BRIGHTER_FACTOR) : color;
}
bool Worm::out_of_range()
{
	return head.pos.length() > (WORLD_BORDER_RADIUS - head.radius);
}