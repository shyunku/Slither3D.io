#include "game_object_manager.hpp"

#include "objvert.hpp"
#include "glutil.hpp"
#include "worm.hpp"
#include "prey.hpp"
#include "world_border.hpp"
#include "player.hpp"
#include <xutility>

using ovprop = ObjectVertexProperty;
typedef unordered_map<uint, Worm> worm_map;
typedef unordered_map<uint, Prey> prey_map;

const uint initial_worm_num = 20;			// 100
const uint initial_prey_num = 7500;			// 10000

const uint prey_create_num_lower_limit = 1000;

// auto prey create
const float PREY_AUTO_CREATE_PERIOD = 5.f;
float prey_auto_create_elapsed = 0.f;

const uint SCOREBOARD_NUM = 6;

InGameObjectManager::in_game_object_manager_() {}
InGameObjectManager::in_game_object_manager_(
	ovprop large_sphere_vertex_property, 
	ovprop small_sphere_vertex_property, 
	ovprop tiny_sphere_vertex_property,
	ovprop circle_vertex_property
)
	: large_sphere_vertex_property(large_sphere_vertex_property),
	small_sphere_vertex_property(small_sphere_vertex_property),
	tiny_sphere_vertex_property(tiny_sphere_vertex_property),
	circle_vertex_property(circle_vertex_property)
{
	srand((unsigned)time(NULL));
	// Create Worms
	for (uint i = 0; i < initial_worm_num; i++)
	{
		push_new_worm_pair();
	}
	
	// Create Preys
	for (uint i = 0; i < initial_prey_num; i++)
	{
		push_new_prey_rand_pos();
	}
}

void InGameObjectManager::render_all()
{
	extern GLuint worldborder_program;
	extern GLuint wormbody_program;
	extern GLuint prey_program;

	// render world border
	set_main_shader(worldborder_program);
	glBindVertexArray(large_sphere_vertex_property.vertex_array_ID);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	world_border.render_sphere(worldborder_program, large_sphere_vertex_property.get_triangles_num());

	// render worms
	set_main_shader(wormbody_program);
	glBindVertexArray(small_sphere_vertex_property.vertex_array_ID);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	for (unordered_map<uint, Worm>::iterator iter = worms.begin(); iter != worms.end(); ++iter)
	{
		iter->second.render_sphere(wormbody_program, small_sphere_vertex_property.get_triangles_num());
	}

	// render preys
	set_main_shader(prey_program);
	glBindVertexArray(tiny_sphere_vertex_property.vertex_array_ID);
	for (prey_map::iterator iter = preys.begin(); iter != preys.end(); ++iter)
	{
		iter->second.render(prey_program, tiny_sphere_vertex_property.get_triangles_num());
	}
}
void InGameObjectManager::update_all(float time_tick)
{
	// update worms
	if (simulate_worms)
	{
		for (unordered_map<uint, Worm>::iterator iter = worms.begin(); iter != worms.end(); ++iter)
		{
			iter->second.update(time_tick, preys);
		}
	}
	magnet_prey(time_tick);
	if(worm_collide_switch) detect_collision_worms();
	detect_collision_worm_prey();
	remove_out_of_range();
	auto_create_prey_system(time_tick);
	delay_create_preys();

	// Select Spectator
	if (player->spectator)
	{
		Worm* worm = &(worms.begin())->second;
		if (worm != NULL)
		{
			player->intent_other(worm);
			player->set = true;
		}
	}
}

void InGameObjectManager::push_delay_create_prey(vec3 pos, float amount, float time)
{
	delayed_create_prey_list.insert(unordered_map<float, pair<vec3, float>>::value_type(time, make_pair(pos, amount)));
}

void InGameObjectManager::delay_create_preys()
{
	for (unordered_map<float, pair<vec3, float>>::iterator iter = delayed_create_prey_list.begin(); iter != delayed_create_prey_list.end();)
	{
		if (iter->first <= float(glfwGetTime()))
		{
			pair<vec3, float> p = iter->second;
			push_new_prey_pos(p.first, p.second);
			iter = delayed_create_prey_list.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void InGameObjectManager::auto_create_prey_system(float time_tick)
{
	prey_auto_create_elapsed += time_tick;
	if (prey_auto_create_elapsed > PREY_AUTO_CREATE_PERIOD)
	{
		prey_auto_create_elapsed = 0;

		if (preys.size() < initial_prey_num - prey_create_num_lower_limit)
		{
			for (uint i = 0; i < prey_create_num_lower_limit; i++)
			{
				push_new_prey_rand_pos();
			}
		}
	}
}

void InGameObjectManager::remove_out_of_range()
{
	for (worm_map::iterator iter1 = worms.begin(); iter1 != worms.end();)
	{
		if (iter1->second.out_of_range())
		{
			gevent.add(format_string("Worm[id=%d] is destroyed by gamma ray, out of the border.", iter1->first));
			if (iter1->first == player->possess_worm)
			{
				player_gameover();
			}
			else
			{
				push_new_worm_pair();
			}
			iter1 = worms.erase(iter1);
		}
		else
		{
			++iter1;
		}
	}
}

void InGameObjectManager::player_gameover()
{
	player->spectator = true;
	worm_collide_switch = false;
	player->score = 0;
}

void InGameObjectManager::detect_collision_worms() {
	// iter1 == target worm - body
	for (worm_map::iterator iter1 = worms.begin(); iter1 != worms.end(); ++iter1)
	{
		// iter2 == other worm - head
		for (worm_map::iterator iter2 = worms.begin(); iter2 != worms.end();)
		{
			// Is in collide range?
			if (iter1->second.is_meetable(iter2->second) && iter1->first != iter2->first)
			{
				bool is_dead = iter2->second.detect_death(iter1->second);
				if (is_dead)
				{
					if (iter1->first == player->possess_worm)
					{
						player->score += player->SCORE_KILL_ENEMY;
					}

					if (iter1->first == player->possess_worm)
					{
						player_gameover();
					}
					else
					{
						push_new_worm_pair();
					}
					iter2->second.spread_prey_before_die();
					gevent.add(format_string("Worm[id=%d] is dead by worm[id=%d]", iter2->first, iter1->first));
					iter2 = worms.erase(iter2);
					continue;
				}
			}
			iter2++;
		}
	}
}
void InGameObjectManager::magnet_prey(float time_tick) {
	for (unordered_map<uint, Worm>::iterator iter1 = worms.begin(); iter1 != worms.end(); ++iter1)
	{
		// iter2 == prey
		for (prey_map::iterator iter2 = preys.begin(); iter2 != preys.end(); ++iter2)
		{
			if (distance(iter1->second.head.pos, iter2->second.pos) <= iter1->second.head.radius + iter2->second.radius + iter1->second.head.radius * 3.f)
			{
				vec3 diff_vec = iter2->second.pos - iter1->second.head.pos;
				iter2->second.pos -= diff_vec * (5.f * time_tick);
			}
		}
	}
}
void InGameObjectManager::detect_collision_worm_prey() {
	// iter1 == target worm
	for (unordered_map<uint, Worm>::iterator iter1 = worms.begin(); iter1 != worms.end(); ++iter1)
	{
		// iter2 == prey
		for (prey_map::iterator iter2 = preys.begin(); iter2 != preys.end();)
		{
			if (iter1->second.detect_eat_prey(iter2->second))
			{
				iter1->second.growth += iter2->second.amount;
				// Player score
				if (iter1->first == player->possess_worm)
				{
					player->score += player->SCORE_EAT_PREY_FACTOR * iter2->second.amount;
				}

				iter2 = preys.erase(iter2);
				break;
			}
			else
			{
				++iter2;
			}
		}
	}
}
int InGameObjectManager::push_new_worm_pair()
{
	Worm worm = Worm(0, worm_id);
	worms.insert(worm_map::value_type(worm_id, worm));

	return worm_id++;
}
void InGameObjectManager::push_new_prey_rand_pos()
{
	Prey prey = Prey();
	preys.insert(prey_map::value_type(prey_id, prey));

	prey_id++;
}
void InGameObjectManager::push_new_prey_pos(vec3 pos, float amount)
{
	Prey prey = Prey(pos, amount);
	preys.insert(prey_map::value_type(prey_id, prey));

	prey_id++;
}
pair<uint, uint> InGameObjectManager::push_new_worm_pairs(uint num)
{
	uint start = worm_id;
	for (uint i = 0; i < num; i++)
	{
		Worm worm = Worm(0, worm_id);
		worms.insert(worm_map::value_type(worm_id, worm));
		worm_id++;
	}
	uint end = worm_id - 1;

	return make_pair(start, end);
}
void InGameObjectManager::push_player_worm_pair()
{
	extern Player* player;
	Worm worm = Worm(0, worm_id);
	worm.make_player();

	worms.insert(worm_map::value_type(worm_id, worm));

	worm_map::iterator object = worms.find(worm_id);
	if (object != worms.end())
	{
		// Found object
		player->set_player(&((*object).second), worm_id);
	}

	worm_id++;
}
void InGameObjectManager::remove_worm(uint id)
{
	worm_map::iterator iter = worms.find(id);

	if (iter == worms.end())
	{
		console.add(format_string("worm[id=%d] doesn't exist.", id), _WARNING_);
		return;
	}

	Worm worm = (*iter).second;

	if (worm.get_id() == player->possess_worm)
	{
		console.add(format_string("worm[id=%d] is your own worm. Can't kill myself.", id), _WARNING_);
		return;
	}

	if (worm.get_id() == player->me->get_id())
	{
		console.add(format_string("worm[id=%d] is your intenting worm. Can't kill it.", id), _WARNING_);
		return;
	}

	console.add(format_string("Killed worm[id=%d]", worm.get_id()), _VERBOSE_);
	worms.erase(iter);

	return;
}
void InGameObjectManager::remove_worms(uint num)
{
	if (num == -1)
	{
		num = worms.size() - 1;
	}
	else if (num > worms.size() - 1)
	{
		console.add(format_string("You can't kill more than entire worms. Resized to %d (except yourself).", worms.size() - 1), _WARNING_);
		num = worms.size() - 1;
	}

	uint killed = 0U;
	for (worm_map::iterator iter = worms.begin(); iter != worms.end();)
	{
		if (iter->second.is_player)
		{
			++iter;
			continue;
		}
		iter = worms.erase(iter);
		if (++killed == num)
		{
			break;
		}
	}

	console.add(format_string("Killed %d worms.", killed), _VERBOSE_);
	return;
}
void InGameObjectManager::remove_worms_except(uint id)
{
	uint killed = 0;
	for (worm_map::iterator iter = worms.begin(); iter != worms.end();)
	{
		if (iter->second.is_player || iter->first == id)
		{
			++iter;
			continue;
		}
		iter = worms.erase(iter);
		killed++;
	}

	console.add(format_string("Killed %d worms except worm[id=%d]", killed, id), _VERBOSE_);
	return;
}
void InGameObjectManager::print_alive_worms()
{
	uint count = 0;
	const uint max_print = 5;
	string msg = format_string("Alive %d worms: ", worms.size());
	for (worm_map::iterator iter = worms.begin(); iter != worms.end(); ++iter)
	{
		msg.append(format_string("[id=%d]", iter->first));
		count++;
		if (count < worms.size() && count < max_print)
		{
			msg.append(", ");
		}
		else
		{
			if (max_print < worms.size())
			{
				msg.append(" ...");
			}
			break;
		}
	}

	console.add(msg, _VERBOSE_);
}
Worm* InGameObjectManager::get_worm_with_id(uint id)
{
	worm_map::iterator iter = worms.find(id);

	if (iter == worms.end())
	{
		console.add(format_string("worm[id=%d] doesn't exist.", id), _WARNING_);
		return NULL;
	}
	Worm* worm = &((*iter).second);
	console.add(format_string("Intented to worm[id=%d]", worm->get_id()), _VERBOSE_);
	return worm;
}