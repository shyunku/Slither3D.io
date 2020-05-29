#pragma once
#include "cgmath.h"
#include "camera.hpp"
#include "worm.hpp"

typedef class player_
{
private:
	const float	LOOK_DISTANCE = 20.f;
	bool		fixed = true;
	inline void update_pos(float time_tick)
	{
		//if (fixed) return;
		//pos += camera.look_direction.normalize() * velocity * time_tick;
	}
public:
	Worm*		me = NULL;
	Camera		camera;
	player_() {}
	player_(Worm* me)
	{
		this->me = me;
	}
	inline void update(float time_tick)
	{
		update_pos(time_tick);
		camera.update(me->head.pos - camera.look_direction * LOOK_DISTANCE);
	}
	inline void set_fix(bool fix)
	{
		fixed = fix;
	}
	inline void set_speed(float vel)
	{
		//velocity = vel;
	}
	inline vec3 get_pos()
	{
		return me->head.pos;
	}
}Player;