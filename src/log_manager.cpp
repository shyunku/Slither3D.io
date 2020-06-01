#include "log_manager.hpp"
#include "game_moderator.hpp"
#include <utility>

extern GameModerator game_moderator;
extern Player* player;

/* -------------------------------------------------------- Command Console -------------------------------------------------------- */
void CommandConsole::flush()
{
	command_builder.clear();
}
void CommandConsole::fetch_command_from_history()
{
	command_builder = history_cursor == -1 ? current_command_save : command_history.at(history_cursor);
}
void CommandConsole::init_command_history_seeker()
{
	history_cursor = -1;
}
void CommandConsole::execute_command()
{
	string keyword = command_parser.get_command_keyword();
	size_t words = command_parser.get_segment_size_except_keyword();
	Worm* me = player->me;

	if (!keyword.compare("help") || !keyword.compare("h"))
	{
		add("You can use command starting with /.", _HIGHLIGHTED_);
		add("/tp 30 40 35.5", _VERBOSE_);
		add("/tp 30 40 35.5", _VERBOSE_);
		return;
	}
	else if (!keyword.compare("setv"))
	{
		if (words != 1)
		{
			add("Invalid Parameter. usage: /setv <speed>", _WARNING_);
			return;
		}
		me->set_speed(stof(command_parser.get_segment(0)));
		return;
	}
	else if (!keyword.compare("tp"))
	{
		
		if (words != 3)
		{
			add("Invalid Parameter. usage: /tp <x> <y> <z>", _WARNING_);
			return;
		}
		float xf = stof(command_parser.get_segment(0));
		float yf = stof(command_parser.get_segment(1));
		float zf = stof(command_parser.get_segment(2));

		me->set_pos(vec3(xf, yf, zf));
		return;
	}
	else if (!keyword.compare("fix") || !keyword.compare("unfix"))
	{
		if (words == 0)
		{
			if (!keyword.compare("fix"))
			{
				me->set_fix(true);
				return;
			}
			me->set_fix(false);
			return;
		}
		else if (words == 1)
		{
			if (!command_parser.get_segment(0).compare("true"))
			{
				me->set_fix(true);
				return;
			}
			else if (!command_parser.get_segment(0).compare("false"))
			{
				me->set_fix(false);
				return;
			}
		}
		else
		{
			add("Invalid parameter. usage: /fix <Boolean> or /fix", _WARNING_);
			return;
		}
	}
	else if (!keyword.compare("intent"))
	{
		if (words != 1)
		{
			add("Invalid Parameter. usage: /intent <id>", _WARNING_);
			return;
		}
		Worm* intenting_worm = game_moderator.ingame_object_manager.get_worm_with_id(stoi(command_parser.get_segment(0)));
		if (intenting_worm)
		{
			player->intent_other(intenting_worm);
		}
		return;
	}
	else if (!keyword.compare("spawn"))
	{
		if (words == 0)
		{
			int id = game_moderator.ingame_object_manager.push_new_worm_pair();
			add(format_string("Worm Spawned with id=%d", id), _VERBOSE_);
		}
		else if (words == 1)
		{
			pair<uint, uint> p = game_moderator.ingame_object_manager.push_new_worm_pairs(stoi(command_parser.get_segment(0)));
			add(format_string("Worm Spawned with id=%d~%d", p.first, p.second), _VERBOSE_);
		}
		else
		{
			add("Invalid Parameter. usage: /spawn or /spawn <worm_num>", _WARNING_);
			return;
		}
		
		return;
	}
	else if (!keyword.compare("kill"))
	{
		if (words != 1)
		{
			add("Invalid Parameter. usage: /kill <id>", _WARNING_);
			return;
		}
		game_moderator.ingame_object_manager.remove_worm(stoi(command_parser.get_segment(0)));
		return;
	}
	else if (!keyword.compare("rkill"))
	{
		if (words != 1)
		{
			add("Invalid Parameter. usage: /rkill <worm_num> or /rkill all", _WARNING_);
			return;
		}
		try 
		{
			uint num = stoi(command_parser.get_segment(0));
			if (num < 1)
			{
				add("Invalid Parameter. worm_num can't be under 1.", _WARNING_);
				return;
			}
			game_moderator.ingame_object_manager.remove_worms(num);
		}
		catch (const std::exception& e)
		{
			string str = e.what();
			string arg = command_parser.get_segment(0);
			if (!arg.compare("all"))
			{
				game_moderator.ingame_object_manager.remove_worms(-1);
			}
			else
			{
				add("Invalid Parameter. Do you mean /rkill all?", _WARNING_);
				return;
			}
		}
		
		return;
	}
	else if (!keyword.compare("exkill"))
	{
		if (words != 1)
		{
			add("Invalid Parameter. usage: /exkill <except_worm_id>", _WARNING_);
			return;
		}
		game_moderator.ingame_object_manager.remove_worms_except(stoi(command_parser.get_segment(0)));
		return;
	}
	else if (!keyword.compare("alive"))
	{
		if (words != 0)
		{
			add("Invalid Parameter. usage: /alive", _WARNING_);
			return;
		}
		game_moderator.ingame_object_manager.print_alive_worms();
		return;
	}
	else if (!keyword.compare("rule"))
	{
		if (words == 0)
		{
			add("Invalid Parameter. usage: /rule <rule_name> ...", _WARNING_);
			return;
		}
		string rule_word = command_parser.get_segment(0);

		if (!rule_word.compare("collide_worm"))
		{
			if (words != 2)
			{
				add("Invalid Parameter. usage: /rule collide_worm <Boolean>", _WARNING_);
				return;
			}

			string bool_word = command_parser.get_segment(1);

			switch (check_string_bool(bool_word))
			{
			case 1:
				game_moderator.ingame_object_manager.worm_collide_switch = true;
				add("Worm Collision Enabled.", _VERBOSE_);
				return;
			case -1:
				game_moderator.ingame_object_manager.worm_collide_switch = false;
				add("Worm Collision Disabled.", _VERBOSE_);
				return;
			case 0:
				add("Invalid Parameter. usage: /rule collide_worm <Boolean>", _WARNING_);
				return;
			}
		}
		else if (!rule_word.compare("simulate_worm"))
		{
			if (words != 2)
			{
				add("Invalid Parameter. usage: /rule simulate_worm <Boolean>", _WARNING_);
				return;
			}

			string bool_word = command_parser.get_segment(1);

			switch (check_string_bool(bool_word))
			{
			case 1:
				game_moderator.ingame_object_manager.simulate_worms = true;
				add("Worm Simulation Enabled.", _VERBOSE_);
				return;
			case -1:
				game_moderator.ingame_object_manager.simulate_worms = false;
				add("Worm Simulation Disabled.", _VERBOSE_);
				return;
			case 0:
				add("Invalid Parameter. usage: /rule simulate_worm <Boolean>", _WARNING_);
				return;
			}
		}
	}
	else if (!keyword.compare("growth"))
	{
		if (words != 2)
		{
			add("Invalid Parameter. usage: /growth <set/add> <amount>", _WARNING_);
			return;
		}

		string sec_word = command_parser.get_segment(0);
		float amount = stof(command_parser.get_segment(1));

		if (!sec_word.compare("set"))
		{
			player->me->growth = amount;
			return;
		}
		else if (!sec_word.compare("add"))
		{
			player->me->growth += amount;
			return;
		}
		else
		{
			add("Invalid Parameter. usage: /growth <set/add> <amount>", _WARNING_);
			return;
		}
	}
	else if (!keyword.compare("start"))
	{
		if (words != 0)
		{
			add("Invalid Parameter. usage: /start", _WARNING_);
			return;
		}

		//start
		player->spectator = false;
		game_moderator.ingame_object_manager.worm_collide_switch = true;
		game_moderator.ingame_object_manager.push_player_worm_pair();
	}

	add("No command keyword for '" + keyword + "'", _WARNING_);
	add("Try /help or /h to see how to use.", _HIGHLIGHTED_);
}

CommandConsole::command_virtual_window_drawer_() : reversed_history_log_manager_()
{
	text_size = 0.4f;
}

inline void CommandConsole::draw_all()
{
	extern ivec2 window_size;
	// Console Contents
	int display_log_limit = window_size.y / 70;

	if (listener_switch)
	{
		// Current command on console
		draw_string
		(
			command_builder,
			X_MIN_OFFSET,
			window_size.y - Y_MIN_OFFSET,
			text_size,
			white
		);
	}

	for (int i = announcements.size() - 1, j = 0; i >= 0 && j <= display_log_limit; i--, j++)
	{
		SystemAnnounceLog announcement = announcements.at(i);
		if (!listener_switch && announcement.is_expired())continue;
		draw_string
		(
			announcement.content,
			X_MIN_OFFSET,
			window_size.y - Y_MIN_OFFSET - LINE_GAP_HEIGHT * j - CONSOLE_Y_OFFSET,
			text_size,
			announcement.get_color_style(listener_switch)
		);
	}
}
void CommandConsole::add(string content, SystemAnnounceLevel level)
{
	announcements.push_back(SystemAnnounceLog(level, content));
}
void CommandConsole::remove_expired()
{
	for (vector<SystemAnnounceLog>::iterator iter = announcements.begin(); iter != announcements.end();)
	{
		if (iter->is_expired())
		{
			announcements.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}
void CommandConsole::activate()
{
	listener_switch = true;
}
void CommandConsole::disable_keylistener()
{
	key_listener_disabler = true;
}
void CommandConsole::enable_keylistener()
{
	key_listener_disabler = false;
}
void CommandConsole::complete()
{
	string completed = command_builder;
	flush();
	listener_switch = false;

	if (!completed.empty() && completed.at(0) == '/')
	{
		command_history.push_back(completed);
		command_parser.accept(completed);
		execute_command();
	}
	// non - command action (ex. chat?)

	init_command_history_seeker();
}
void CommandConsole::cancel()
{
	flush();
	listener_switch = false;
	init_command_history_seeker();
}
void CommandConsole::append(char c)
{
	command_builder.push_back(c);
}
void CommandConsole::backspace()
{
	if (!command_builder.empty())
	{
		command_builder.pop_back();
	}
}
void CommandConsole::seek_command_history(bool seek_prev)
{
	if (!listener_switch || command_history.empty())return;
	if (seek_prev)
	{
		// seek up
		if (history_cursor == -1)
		{
			// goto history
			history_cursor = command_history.size() - 1;
			current_command_save = command_builder;
			fetch_command_from_history();
		}
		else if (history_cursor != 0)
		{
			// get prev command
			history_cursor--;
			fetch_command_from_history();
		}
	}
	else
	{
		// seek down
		if (history_cursor == command_history.size() - 1)
		{
			// return to current line
			history_cursor = -1;
			fetch_command_from_history();
		}
		else if (history_cursor != -1)
		{
			// get next command
			history_cursor++;
			fetch_command_from_history();
		}
	}
}

/* -------------------------------------------------------- System Announce Logger -------------------------------------------------------- */
SystemAnnounceLog::system_announce_log_(SystemAnnounceLevel level, string str)
{
	announce_level = level;
	content = str;
	registered_timestamp = float(glfwGetTime());
}
bool SystemAnnounceLog::is_expired()
{
	return alive_time() > LOG_EXPIRE_TIME;
}
float SystemAnnounceLog::alive_time()
{
	return float(glfwGetTime()) - registered_timestamp;
}
float SystemAnnounceLog::text_opacity()
{
	float alive = alive_time();
	if (alive < LOG_EXPIRE_TIME - LOG_FADEOUT_TIME)
	{
		return 1.f;
	}
	else
	{
		float ropacity = (alive - LOG_EXPIRE_TIME + LOG_FADEOUT_TIME) / LOG_FADEOUT_TIME;
		return 1.f - ropacity;
	}
}
vec4 SystemAnnounceLog::get_color_style(bool is_force_active)
{
	float opacity = is_force_active ? 1.f : text_opacity();
	vec4 factor = vec4(1, 1, 1, opacity);

	switch (announce_level)
	{
	case _VERBOSE_:
		return console_verbose * factor;
	case _WARNING_:
		return console_warning * factor;
	case _ERROR_:
		return console_error * factor;
	case _HIGHLIGHTED_:
		return console_highlighted * factor;
	default:
		return console_verbose * factor;
	}
}

/* -------------------------------------------------------- System Announce Logger -------------------------------------------------------- */
AppInfoLogger::app_info_drawer_() : stacking_log_manager_() {}
inline void AppInfoLogger::draw(string str)
{
	draw_string
	(
		str,
		X_MIN_OFFSET,
		Y_MIN_OFFSET + LINE_GAP_HEIGHT * stack_index++,
		text_size,
		standard_log
	);
}

/* -------------------------------------------------------- Static Value Logger -------------------------------------------------------- */
StaticValueLogger::static_value_drawer_() : stacking_log_manager_() {}
inline void StaticValueLogger::draw(string str)
{
	extern ivec2 window_size;
	draw_string
	(
		str,
		(GLint)(window_size.x - X_MIN_OFFSET - get_string_width(str, text_size)),
		Y_MIN_OFFSET + LINE_GAP_HEIGHT * stack_index++,
		text_size,
		standard_log
	);
}

void StaticValueLogger::draw(string str, vec4 color)
{
	extern ivec2 window_size;
	draw_string
	(
		str,
		(GLint)(window_size.x - X_MIN_OFFSET - get_string_width(str, text_size)),
		Y_MIN_OFFSET + LINE_GAP_HEIGHT * stack_index++,
		text_size,
		color
	);
}

/* -------------------------------------------------------- Game Event Logger -------------------------------------------------------- */
GameEventLogger::game_log_drawer_() : reversed_history_log_manager_() {}
inline void GameEventLogger::draw_all()
{
	extern ivec2 window_size;
	int display_log_limit = window_size.y / 50;
	for (int i = log_history.size() - 1, j = 0; i >= 0 && j < display_log_limit; i--, j++)
	{
		string current_str = log_history.at(i);
		draw_string(
			current_str,
			(GLint)(window_size.x - X_MIN_OFFSET - get_string_width(current_str, text_size)),
			window_size.y - Y_MIN_OFFSET - LINE_GAP_HEIGHT * j,
			text_size,
			event_log
		);
	}
}
void GameEventLogger::add(string str)
{
	log_history.push_back(str+ " " +get_current_time_string());
	if (log_history.size() > MAX_LOG_LIMIT)
	{
		log_history.erase(log_history.begin());
	}
}