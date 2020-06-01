#pragma once
#include "worm.hpp"
#include "camera.hpp"

typedef class player_
{
private:
	const float	LOOK_DISTANCE = 20.f;
	const float MIN_ZOOM_FACTOR = 0.3f;
	const float MAX_ZOOM_FACTOR = 10.f;
	bool		fixed = true;
public:
	const float SCORE_TIME_TICK_FACTOR = 2.5f;
	const float SCORE_EAT_PREY_FACTOR = 2.f;
	const float SCORE_KILL_ENEMY = 85.f;

	Worm*		me;
	Camera		camera;
	uint		possess_worm = 0U;		// real me worm id
	float		zoom_factor = 1.0f;
	float		score = 0;
	player_();
	player_(Worm* me);
	void update(float time_tick);
	inline vec3 get_pos();
	void set_move_direction();
	void set_player(Worm* worm, uint index);
	void intent_other(Worm* worm);
	void control_factor(double offset);
}Player;