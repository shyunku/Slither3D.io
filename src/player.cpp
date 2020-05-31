#include "player.hpp"

Player::player_() {}
Player::player_(Worm* me)
{
	this->me = me;
}
void Player::update(float time_tick)
{
	camera.update(me->head.pos - camera.look_direction * LOOK_DISTANCE);
}
inline vec3 Player::get_pos()
{
	return me->head.pos;
}
void Player::set_move_direction()
{
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