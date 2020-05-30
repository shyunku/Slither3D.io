#include "player.hpp"

Player::player_() {}
Player::player_(Worm* me)
{
	this->me = me;
}
inline void Player::update(float time_tick)
{
	camera.update(me->head.pos - camera.look_direction * LOOK_DISTANCE);
}
inline vec3 Player::get_pos()
{
	return me->head.pos;
}
inline void Player::set_move_direction()
{
	me->decided_direction = camera.look_direction.normalize();
}