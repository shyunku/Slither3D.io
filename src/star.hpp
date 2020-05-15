#pragma once
#include "celestial_object.hpp"

typedef class star_ : public Celestial
{
public:
	star_(SpinProperty spin_property, float radius, float gravity)
		: Celestial(spin_property, radius, gravity)
	{
	}
}Star;