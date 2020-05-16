#pragma once
#include "celestial_objects.hpp"
#include "objvert.hpp"
#include "glutil.hpp"

using ovprop = ObjectVertexProperty;

typedef class in_game_object_manager_
{
private:
	ObjectVertexProperty sphere_vertex_property;
	ObjectVertexProperty circle_vertex_property;
public:
	vector<Star> stars;
	in_game_object_manager_(){}
	in_game_object_manager_(ovprop sphere_vertex_property, ovprop circle_vertex_property) 
		: sphere_vertex_property(sphere_vertex_property), 
		circle_vertex_property(circle_vertex_property)
	{
		// please modify this (this is for test)
		Star sun = Star(SpinProperty(), 1.f, 1.f, "sun");
		Planet pluto = Planet(
			SpinProperty(),
			RevolveProperty(1.4f, 0.2f, vec3(0, 1, 0), false),
			0.03f, 0.f, "pluto"
		);
		Planet earth = Planet(
			SpinProperty(),
			RevolveProperty(2.f, 1.f, vec3(0, 1, 0), true), 
			0.1f, 0.f, "earth"
		);
		Planet jupiter = Planet(
			SpinProperty(),
			RevolveProperty(4.5f, 4.6f, vec3(0, 1, 0), true),
			0.4f, 0.f, "jupiter"
		);
		Planet uranus = Planet(
			SpinProperty(),
			RevolveProperty(13.f, 6.2f, vec3(0, 1, 0), true),
			0.35f, 0.f, "uranus"
		);
		Planet neptune = Planet(
			SpinProperty(),
			RevolveProperty(27.f, 10.f, vec3(0, 1, 0), true),
			0.2f, 0.f, "neptune"
		);
		Satelite moon = Satelite(
			SpinProperty(),
			RevolveProperty(0.25f, 0.6f, vec3(0, 1, 0), true),
			0.02f, 0.f, "moon"
		);

		earth.add_satelite(moon);
		sun.add_planet(earth);
		sun.add_planet(pluto);
		sun.add_planet(jupiter);
		sun.add_planet(uranus);
		sun.add_planet(neptune);
		stars.push_back(sun);
	}

	void render_all()
	{
		extern GLuint default_program;

		glUseProgram(default_program);

		// Bind Sphere and render
		glBindVertexArray(sphere_vertex_property.vertex_array_ID);
		for (vector<Star>::iterator iter = stars.begin(); iter != stars.end(); ++iter)
		{
			iter->render_sphere(default_program, sphere_vertex_property.get_triangles_num());
		}

		// Bind Orbit and render
		glBindVertexArray(circle_vertex_property.vertex_array_ID);
		for (vector<Star>::iterator iter = stars.begin(); iter != stars.end(); ++iter)
		{
			iter->render_orbit(default_program, circle_vertex_property.get_triangles_num(), true);
		}
	}

	void update_all(float time_tick)
	{
		for (vector<Star>::iterator iter = stars.begin(); iter != stars.end(); ++iter)
		{
			iter->update(time_tick, vec3(), true);
		}
	}
}InGameObjectManager;