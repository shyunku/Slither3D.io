#pragma once
#include "camera.hpp"
#include "worm.hpp"

typedef class player_
{
private:
	const float	LOOK_DISTANCE = 20.f;
	bool		fixed = true;
public:
	Worm*		me;
	Camera		camera;
	player_();
	player_(Worm* me);
	inline void update(float time_tick);
	inline vec3 get_pos();
	inline void set_move_direction();
}Player;