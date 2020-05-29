#pragma once
#include "celestial_objects.hpp"
#include "objvert.hpp"
#include "glutil.hpp"
#include "worm.hpp"
#include "world_border.hpp"

using ovprop = ObjectVertexProperty;

typedef class in_game_object_manager_
{
private:
	ObjectVertexProperty sphere_vertex_property;
	ObjectVertexProperty circle_vertex_property;
public:
	//vector<Star> stars;
	vector<Worm> worms;
	WorldBorder world_border = WorldBorder();

	in_game_object_manager_(){}
	in_game_object_manager_(ovprop sphere_vertex_property, ovprop circle_vertex_property) 
		: sphere_vertex_property(sphere_vertex_property), 
		circle_vertex_property(circle_vertex_property)
	{
		uint initial_worm_num = 80;

		for (uint i = 0; i < initial_worm_num - 1; i++)
		{
			Worm worm = Worm(30);
			worms.push_back(worm);
		}

		Worm player_worm = Worm(15);
		player_worm.make_player();
		worms.push_back(player_worm);
	}

	void render_all()
	{
		extern GLuint default_program;

		glUseProgram(default_program);

		glBindVertexArray(sphere_vertex_property.vertex_array_ID);

		// render world border
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		world_border.render_sphere(default_program, sphere_vertex_property.get_triangles_num());

		// render stars
		//for (vector<Star>::iterator iter = stars.begin(); iter != stars.end(); ++iter)
		//{
		//	iter->render_sphere(default_program, sphere_vertex_property.get_triangles_num());
		//}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (vector<Worm>::iterator iter = worms.begin(); iter != worms.end(); ++iter)
		{
			iter->render_sphere(default_program, sphere_vertex_property.get_triangles_num());
		}

		// Bind Orbit and render
		//glBindVertexArray(circle_vertex_property.vertex_array_ID);
		//for (vector<Star>::iterator iter = stars.begin(); iter != stars.end(); ++iter)
		//{
		//	iter->render_orbit(default_program, circle_vertex_property.get_triangles_num(), true);
		//}
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