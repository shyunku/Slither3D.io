#pragma once
#include "celestial_object.hpp"

typedef class star_ : Celestial
{
public:
	star_(SpinProperty spin_property, float radius, float gravity)
		: Celestial(spin_property, radius, gravity)
	{
	}
	void render(GLint uloc, uint sphere_triangles)
	{
		render_object(uloc, sphere_triangles);
	}
}Star;