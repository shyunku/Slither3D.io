#pragma once
#include "celestial_objects.hpp"
#include "objvert.hpp"
#include "glutil.hpp"
#include "worm.hpp"
#include "prey.hpp"
#include "world_border.hpp"

using ovprop = ObjectVertexProperty;

typedef class in_game_object_manager_
{
private:
	ObjectVertexProperty large_sphere_vertex_property;
	ObjectVertexProperty small_sphere_vertex_property;
	ObjectVertexProperty circle_vertex_property;
public:
	//vector<Star> stars;
	vector<Worm> worms;
	vector<Prey> preys;
	WorldBorder world_border = WorldBorder();

	in_game_object_manager_(){}
	in_game_object_manager_(ovprop large_sphere_vertex_property, ovprop small_sphere_vertex_propery, ovprop circle_vertex_property) 
		: large_sphere_vertex_property(large_sphere_vertex_property),
		small_sphere_vertex_property(small_sphere_vertex_propery),
		circle_vertex_property(circle_vertex_property)
	{
		uint initial_worm_num = 130;
		for (uint i = 0; i < initial_worm_num - 1; i++)
		{
			Worm worm = Worm(randf(100, 1300));
			worms.push_back(worm);
		}
		Worm player_worm = Worm(30);
		player_worm.make_player();
		worms.push_back(player_worm);

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
		for (vector<Worm>::iterator iter = worms.begin(); iter != worms.end(); ++iter)
		{
			iter->render_sphere(default_program, small_sphere_vertex_property.get_triangles_num());
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
		for (vector<Worm>::iterator iter = worms.begin(); iter != worms.end(); ++iter)
		{
			iter->update(time_tick);
		}
	}

	Worm* get_player()
	{
		for (vector<Worm>::iterator iter = worms.begin(); iter != worms.end(); ++iter)
		{
			if (iter->is_player)
			{
				return &(*iter);
			}
		}
		return NULL;
	}
}InGameObjectManager;