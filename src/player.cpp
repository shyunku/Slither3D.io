#include "player.hpp"

Player::player_() {}
Player::player_(Worm* me)
{
	this->me = me;
}
void Player::update(float time_tick)
{
	if (!set)return;
	camera.update(me->head.pos - camera.look_direction * zoom_factor * (LOOK_DISTANCE + me->get_camera_distance()));
}
inline vec3 Player::get_pos()
{
	return me->head.pos;
}
void Player::set_move_direction()
{
	if (!set)return;
	if (me->get_id() != possess_worm)return;
	me->decided_direction = camera.look_direction.normalize();
}
void Player::set_player(Worm* worm, uint index)
{
	me = worm;
	possess_worm = index;
}
void Player::intent_other(Worm* worm)
{
	me = worm;
}
void Player::control_factor(double offset)
{
	zoom_factor += float(offset) * 0.1f;
	if (zoom_factor < MIN_ZOOM_FACTOR)
	{
		zoom_factor = MIN_ZOOM_FACTOR;
	}
	else if (zoom_factor > MAX_ZOOM_FACTOR) 
	{
		zoom_factor = MAX_ZOOM_FACTOR;
	}
}