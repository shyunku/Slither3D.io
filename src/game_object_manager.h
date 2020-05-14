#pragma once
#include "Star.hpp"
#include "objvert.hpp"

typedef class in_game_object_manager_
{
private:
	ObjectVertexProperty sphere_vertex_property;
public:
	vector<Star> stars;
	in_game_object_manager_(){}
	in_game_object_manager_(ObjectVertexProperty sphere_vertex_property) 
		: sphere_vertex_property(sphere_vertex_property)
	{
		// please modify this (this is for test)
		Star sun = Star(SpinProperty(), 1.f, 1.f);
		stars.push_back(sun);
	}

	void render_all(uint program_id)
	{
		bind_all();
		GLint uloc = glGetUniformLocation(program_id, "model_matrix");
		for (vector<Star>::iterator iter = stars.begin(); iter != stars.end(); ++iter)
		{
			iter->render(uloc, sphere_vertex_property.get_triangles_num());
		}
	}

	void bind_all()
	{
		// bind sphere
		glBindVertexArray(sphere_vertex_property.vertex_array_ID);
	}
}InGameObjectManager;