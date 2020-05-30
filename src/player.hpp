#pragma once
#include "cgmath.h"
#include "camera.hpp"
#include "worm.hpp"

typedef class player_
{
private:
	const float	LOOK_DISTANCE = 20.f;
	bool		fixed = true;
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
	inline void set_move_direction()
	{
		me->decided_direction = camera.look_direction.normalize();
	}
}Player;