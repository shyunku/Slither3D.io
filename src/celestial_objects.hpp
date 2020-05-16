#pragma once
#include "celestial.hpp"

typedef class satelite_ : public Celestial
{
public:
	satelite_(SpinProperty spin_property, RevolveProperty revolve_property, float radius, float gravity, string name)
		: Celestial(spin_property, revolve_property, radius, gravity, name)
	{
	}
}Satelite;

typedef class planet_ : public Celestial
{
public:
	planet_(SpinProperty spin_property, RevolveProperty revolve_property, float radius, float gravity, string name)
		: Celestial(spin_property, revolve_property, radius, gravity, name)
	{
	}
	void add_satelite(Satelite satelite)
	{
		add_child(satelite);
	}
}Planet;


typedef class star_ : public Celestial
{
public:
	star_(SpinProperty spin_property, float radius, float gravity, string name)
		: Celestial(spin_property, radius, gravity, name)
	{
	}
	void add_planet(Planet planet)
	{
		this->add_child(planet);
	}
}Star;