#pragma once
//#include "celestial_objects.hpp"
#include "objvert.hpp"
#include "glutil.hpp"
#include "worm.hpp"
#include "prey.hpp"
#include "world_border.hpp"
#include "player.hpp"
#include <unordered_map>
#include <utility>

extern CommandConsole console;
extern GameEventLogger gevent;
extern Player* player;

using ovprop = ObjectVertexProperty;
typedef unordered_map<uint, Worm> worm_map;

typedef class in_game_object_manager_
{
private:
	ObjectVertexProperty large_sphere_vertex_property;
	ObjectVertexProperty small_sphere_vertex_property;
	ObjectVertexProperty circle_vertex_property;
	uint worm_id = 0U;
public:
	//vector<Star> stars;
	unordered_map<uint, Worm> worms;
	//vector<Worm> worms;
	vector<Prey> preys;
	WorldBorder world_border = WorldBorder();

	in_game_object_manager_(){}
	in_game_object_manager_(ovprop large_sphere_vertex_property, ovprop small_sphere_vertex_propery, ovprop circle_vertex_property) 
		: large_sphere_vertex_property(large_sphere_vertex_property),
		small_sphere_vertex_property(small_sphere_vertex_propery),
		circle_vertex_property(circle_vertex_property)
	{
		uint initial_worm_num = 1;
		for (uint i = 0; i < initial_worm_num - 1; i++)
		{
			push_new_worm_pair();
		}
		push_player_worm_pair();

		uint initial_prey_num = 13000;
		for (uint i = 0; i < initial_prey_num; i++)
		{
			Prey prey = Prey();
			preys.push_back(prey);
		}
	}

	void render_all()
	{
		extern GLuint default_program;
		extern GLuint glow_program;

		set_main_shader(default_program);

		// render world border
		glBindVertexArray(large_sphere_vertex_property.vertex_array_ID);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		world_border.render_sphere(default_program, large_sphere_vertex_property.get_triangles_num());

		// render worms
		glBindVertexArray(small_sphere_vertex_property.vertex_array_ID);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
		for (unordered_map<uint, Worm>::iterator iter = worms.begin(); iter != worms.end(); ++iter)
		{
			iter->second.render_sphere(default_program, small_sphere_vertex_property.get_triangles_num());
		}

		// render preys
		set_main_shader(glow_program);
		glBindVertexArray(small_sphere_vertex_property.vertex_array_ID);
		for (vector<Prey>::iterator iter = preys.begin(); iter != preys.end(); ++iter)
		{
			iter->render(glow_program, small_sphere_vertex_property.get_triangles_num());
		}
	}
	void update_all(float time_tick)
	{
		// update worms
		for (unordered_map<uint, Worm>::iterator iter = worms.begin(); iter != worms.end(); ++iter)
		{
			iter->second.update(time_tick);
		}
		magnet_prey();
		detect_collision_worms();
		detect_collision_worm_prey();
	}
	void detect_collision_worms() {

		// iter1 == target worm - body
		for (worm_map::iterator iter1 = worms.begin(); iter1 != worms.end(); ++iter1)
		{
			// iter2 == other worm - head
			for (worm_map::iterator iter2 = worms.begin(); iter2 != worms.end();)
			{
				if (iter1->first != iter2->first)
				{
					bool is_dead = iter2->second.detect_death(iter1->second);
					if (is_dead)
					{
						gevent.add(format_string("Worm[id=%d] dead by worm[id=%d]", iter2->first, iter1->first));
						iter2 = worms.erase(iter2);
						continue;
					}
				}
				iter2++;
			}
		}
	}
	void magnet_prey() {
		for (unordered_map<uint, Worm>::iterator iter1 = worms.begin(); iter1 != worms.end(); ++iter1)
		{
			// iter2 == prey
			for (vector<Prey>::iterator iter2 = preys.begin(); iter2 != preys.end(); ++iter2)
			{
				if (distance(iter1->second.head.pos, iter2->pos) <= iter1->second.head.radius + iter2->radius + 0.5f)
				{
					vec3 diff_vec = iter2->pos - iter1->second.head.pos;
					iter2->pos -= diff_vec * 0.1f;
				}
			}
		}
	}
	void detect_collision_worm_prey() {
		// iter1 == target worm
		for (unordered_map<uint, Worm>::iterator iter1 = worms.begin(); iter1 != worms.end(); ++iter1)
		{
			// iter2 == prey
			for (vector<Prey>::iterator iter2 = preys.begin(); iter2 != preys.end(); ++iter2)
			{
				if (distance(iter1->second.head.pos, iter2->pos) <= iter1->second.head.radius + iter2->radius)
				{
					iter1->second.growth += iter2->amount;
				}
			}
		}
	}
	int push_new_worm_pair()
	{
		Worm worm = Worm(randf(100, 900), worm_id);
		worms.insert(worm_map::value_type(worm_id, worm));

		return worm_id++;
	}

	void push_new_prey_pos(vec3 pos)
	{
		Prey prey = Prey(pos);
		preys.push_back(prey);
	}
	pair<uint, uint> push_new_worm_pairs(uint num)
	{
		uint start = worm_id;
		for (uint i = 0; i < num; i++)
		{
			Worm worm = Worm(randf(100, 900), worm_id);
			worms.insert(worm_map::value_type(worm_id, worm));
			worm_id++;
		}
		uint end = worm_id - 1;

		return make_pair(start,end);
	}
	void push_player_worm_pair()
	{
		extern Player* player;
		Worm worm = Worm(30, worm_id);
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
	void remove_worm(uint id)
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
	void remove_worms(uint num)
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
	void print_alive_worms()
	{
		uint count = 0;
		const uint max_print = 7;
		string msg = format_string("Alive %d worms: ", worms.size());
		for (worm_map::iterator iter = worms.begin(); iter != worms.end();++iter)
		{
			msg.append(format_string("[id=%d]",iter->first));
			count++;
			if (count < worms.size() && count < max_print)
			{
				msg.append(", ");
			}
			else
			{
				break;
			}
		}

		console.add(msg, _VERBOSE_);
	}
	Worm* get_worm_with_id(uint id)
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
}InGameObjectManager;