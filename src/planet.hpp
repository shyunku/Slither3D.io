#pragma once
# include "celestial_object.hpp"

typedef class planet_ : Celestial
{
public:
	planet_(SpinProperty spin_property, float radius, float gravity)
		: Celestial(spin_property, radius, gravity)
	{
	}
}Planet;