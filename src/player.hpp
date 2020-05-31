#pragma once
#include "worm.hpp"
#include "camera.hpp"

typedef class player_
{
private:
	const float	LOOK_DISTANCE = 20.f;
	bool		fixed = true;
public:
	Worm*		me;
	Camera		camera;
	uint		possess_worm = 0U;		// real me worm id
	player_();
	player_(Worm* me);
	void update(float time_tick);
	inline vec3 get_pos();
	void set_move_direction();
	void set_player(Worm* worm, uint index);
	void intent_other(Worm* worm);
}Player;