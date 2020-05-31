#pragma once
#include "cgmath.h"
#include "cgut.h"
#include "util.hpp"
#include "glutil.hpp"

using namespace std;

extern inline float randf(float LO, float HI);

// Objects that has revolve origin
typedef class revolve_property_
{
private:
	bool	revolve;
	float	revolved_angle;
public:
	vec3	origin_pos;
	vec3	revolve_axis_direction;
	float	revolve_period;
	bool	clock_wise;
	float	orbit_radius;
	revolve_property_()
	{
		this->revolve = false;
	}
	revolve_property_(float orbit_radius, float revolve_period, vec3 axis_direction, bool clock_wise)
	{
		this->revolve = true;
		this->orbit_radius = orbit_radius;
		this->revolve_period = revolve_period;
		this->clock_wise = clock_wise;
		this->revolve_axis_direction = axis_direction;
	}
	void update(float time_tick)
	{
		if (!revolve)return;
		revolved_angle += (clock_wise ? -1 : 1) * time_tick / revolve_period;
	}
	vec3 update_and_get_current_pos(vec3 origin_pos)
	{
		this->origin_pos = origin_pos;
		// temporary method:: apply axis direction
		return origin_pos + orbit_radius * vec3(cosf(revolved_angle), 0, sinf(revolved_angle));
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
	void update()
	{

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
	mat4 get_orbit_model_matrix()
	{
		return
			mat4::translate(revolve_property.origin_pos) *
			mat4::rotate(vec3(0, 1, 0), spin_property.get_current_spin_angle()) *
			mat4::scale(revolve_property.orbit_radius);
	}
	void add_child(celestial_object_ satelite)
	{
		satelites.push_back(satelite);
	}
public:
	std::string					name;
	vector<celestial_object_>	satelites;
	vec3						pos;
	float						radius;
	float						gravity;			// will be used in game (not affects to non-player object)

	celestial_object_(SpinProperty spin_property, float radius, float gravity, string name) 
		: spin_property(spin_property), radius(radius), gravity(gravity), name(name)
	{
		this->revolve_property = RevolveProperty();
	}
	celestial_object_(SpinProperty spin_property, RevolveProperty revolve_property, float radius, float gravity, string name)
		: spin_property(spin_property), revolve_property(revolve_property), radius(radius), gravity(gravity), name(name)
	{
	}

	void render_sphere(GLuint shader_program, uint sphere_triangles)
	{
		GLint uloc = get_validated_uniform_location(shader_program, "model_matrix");

		// Self-render
		uloc = get_validated_uniform_location(shader_program, "model_matrix");
		glUniformMatrix4fv(uloc, 1, GL_TRUE, get_model_matrix());

		uloc = get_validated_uniform_location(shader_program, "is_using_solid_color");
		glUniform1i(uloc, false);

		uloc = get_validated_uniform_location(shader_program, "solid_color");
		glUniform4fv(uloc, 1, vec4(0.3f, 0.3f, 0.3f, 1.f));

		glDrawElements(GL_TRIANGLES, sphere_triangles, GL_UNSIGNED_INT, nullptr);

		// Children-render
		for (uint i = 0; i < satelites.size(); i++)
		{
			satelites[i].render_sphere(shader_program, sphere_triangles);
		}
	}

	void render_orbit(GLuint shader_program, uint circle_triangles, float isParent)
	{
		GLint uloc;
		// Self-render
		if (!isParent)
		{
			uloc = get_validated_uniform_location(shader_program, "model_matrix");
			glUniformMatrix4fv(uloc, 1, GL_TRUE, get_orbit_model_matrix());

			uloc = get_validated_uniform_location(shader_program, "is_using_solid_color");
			glUniform1i(uloc, true);

			uloc = get_validated_uniform_location(shader_program, "solid_color");
			glUniform4fv(uloc, 1, vec4(0.4f, 0.4f, 1.f, 0.6f));

			glDrawElements(GL_TRIANGLES, circle_triangles, GL_UNSIGNED_INT, nullptr);
		}
		
		// Children-render
		for (uint i = 0; i < satelites.size(); i++)
		{
			satelites[i].render_orbit(shader_program, circle_triangles, false);
		}
	}

	void update(float time_tick, vec3 parent_pos, float isParent)
	{
		// Self-update
		revolve_property.update(time_tick);
		spin_property.update();
		if (!isParent)
		{
			pos = revolve_property.update_and_get_current_pos(parent_pos);
		}
		
		// Children-update
		for (uint i = 0; i < satelites.size(); i++)
		{
			satelites[i].update(time_tick, pos, false);
		}
	}
}Celestial;