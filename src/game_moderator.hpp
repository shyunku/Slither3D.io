#pragma once
#include "cgmath.h"
#include "game_object_manager.hpp"

typedef enum game_mode_code		{ FREE_MODE, CLASSIC_MODE }GameMode;
typedef enum game_status_code	{ GAMING, RESULTING, SUSPENDING }GameStatus;
typedef enum game_position_code { MAIN_MENU, }GameFocusPos;

typedef class game_moderator_
{
private:
	GameMode				curr_game_mode;
	GameStatus				curr_game_status;
	GameFocusPos			curr_game_focus_pos;
	InGameObjectManager		ingame_object_manager;
public:
	game_moderator_(){}
	game_moderator_(ObjectVertexProperty sphere_vertex_property)
	{
		curr_game_mode = FREE_MODE;
		curr_game_status = SUSPENDING;
		curr_game_focus_pos = MAIN_MENU;
		ingame_object_manager = InGameObjectManager(sphere_vertex_property);
	}

	void render(uint program_id)
	{
		ingame_object_manager.render_all(program_id);
	}
}GameModerator;