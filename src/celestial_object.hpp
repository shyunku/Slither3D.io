#pragma once
#include "cgmath.h"
#include "util.hpp"

// Objects that has revolve origin
typedef class revolve_property_
{
private:
	bool	revolve;
	float	revolved_angle;
public:
	uint	origin;			// need modification as address &
	vec3	revolve_axis_direction;
	float	revolve_period;
	bool	clock_wise;
	float	orbit_radius;
	revolve_property_()
	{
		this->revolve = false;
	}
	bool is_revolvable()
	{
		return this->revolve;
	}
}RevolveProperty;

// Self-rotate property
typedef class spin_property_
{
private:
	float	spinned_angle;
public:
	vec3	spin_axis_direction;
	float	rotate_period;			// Calculate in 1 sec
	bool	clock_wise;				// spin direction in terms of spin-axis
	spin_property_()
	{
		// Initialize angle as random
		this->spinned_angle = randf(0, 2 * PI);
	}
	float get_current_spin_angle()
	{
		return this->spinned_angle;
	}
}SpinProperty;

// Sphere class as super class of orbs
typedef class celestial_object_
{
protected:
	RevolveProperty				revolve_property;
	SpinProperty				spin_property;
	mat4						model_matrix;
	mat4 get_model_matrix()
	{
		return
			mat4::translate(pos) *
			mat4::rotate(vec3(0, 1, 0), spin_property.get_current_spin_angle()) *
			mat4::scale(radius);
	}
public:
	vector<celestial_object_>	satelites;
	vec3						pos;
	float						radius;
	float						gravity;			// will be used in game (not affects to non-player object)

	celestial_object_(SpinProperty spin_property, float radius, float gravity) 
		: spin_property(spin_property), radius(radius), gravity(gravity)
	{
		this->revolve_property = RevolveProperty();
	}
	void render_sphere(GLint uloc, uint sphere_triangles)
	{
		// Self-render
		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, get_model_matrix());
		glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);
	}
}Celestial;