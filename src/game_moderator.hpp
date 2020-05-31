#pragma once
#include "cgmath.h"
#include "game_object_manager.hpp"
#include "objvert.hpp"

typedef enum game_mode_code		{ FREE_MODE, CLASSIC_MODE }GameMode;
typedef enum game_status_code	{ GAMING, RESULTING, SUSPENDING }GameStatus;
typedef enum game_position_code { MAIN_MENU, }GameFocusPos;

using ovprop = ObjectVertexProperty;

typedef class game_moderator_
{
private:
	GameMode				curr_game_mode;
	GameStatus				curr_game_status;
	GameFocusPos			curr_game_focus_pos;
public:
	InGameObjectManager		ingame_object_manager;

	game_moderator_(){}
	game_moderator_(
		ovprop large_sphere_vertex_property,
		ovprop small_sphere_vertex_property,
		ovprop tiny_sphere_vertex_property,
		ovprop circle_vertex_property
	)
	{
		curr_game_mode = FREE_MODE;
		curr_game_status = SUSPENDING;
		curr_game_focus_pos = MAIN_MENU;
		ingame_object_manager = InGameObjectManager(
			large_sphere_vertex_property, 
			small_sphere_vertex_property, 
			tiny_sphere_vertex_property,
			circle_vertex_property);
	}
	void update(float time_tick)
	{
		ingame_object_manager.update_all(time_tick);
	}
	void render()
	{
		ingame_object_manager.render_all();
	}
}GameModerator;