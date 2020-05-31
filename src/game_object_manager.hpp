#pragma once
//#include "celestial_objects.hpp"
#include "objvert.hpp"
#include "glutil.hpp"
#include "worm.hpp"
#include "prey.hpp"
#include "world_border.hpp"
#include "player.hpp"
#include <unordered_map>

extern CommandConsole console;
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
		uint initial_worm_num = 2;
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
	}
	int push_new_worm_pair()
	{
		Worm worm = Worm(randf(100, 900), worm_id);
		worms.insert(worm_map::value_type(worm_id, worm));

		return worm_id++;
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