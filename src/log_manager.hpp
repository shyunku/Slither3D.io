#pragma once
#include "global_constants.hpp"
#include "cgmath.h"
#include "cgut.h"
#include "command.hpp"

using namespace std;
extern void draw_string(string text, GLint _x, GLint _y, GLfloat scale, vec4 color);
extern float get_string_width(string str, float scale);
extern string get_current_time_string();

typedef enum system_announcement_level {_VERBOSE_, _HIGHLIGHTED_, _WARNING_, _ERROR_ }SystemAnnounceLevel;

typedef class stacking_log_manager_
{
protected:
	const uint X_MIN_OFFSET = 10;
	const uint Y_MIN_OFFSET = 20;
	const uint LINE_GAP_HEIGHT = 15;
	uint stack_index;
	float text_size;
public:
	stacking_log_manager_()
	{
		stack_index = 0U;
		text_size = 0.3f;
	}
	virtual inline void draw(string str) = 0;
	inline void blank(uint lines)
	{
		stack_index += lines;
	}
	inline void clear_stack()
	{
		stack_index = 0U;
	}
}StackLogManager;

typedef class reversed_history_log_manager_
{
protected:
	const uint X_MIN_OFFSET = 10;
	const uint Y_MIN_OFFSET = 30;
	float text_size;
public:
	reversed_history_log_manager_()
	{
		text_size = 0.3f;
	}
	virtual inline void draw_all() = 0;
}ReversedHistoryLogManager;

typedef class system_announce_log_
{
private:
	float LOG_EXPIRE_TIME = 8.f;
	float LOG_FADEOUT_TIME = 2.f;
public:
	SystemAnnounceLevel announce_level;
	string content;
	float registered_timestamp;
	system_announce_log_(SystemAnnounceLevel level, string str)
	{
		announce_level = level;
		content = str;
		registered_timestamp = float(glfwGetTime());
	}
	bool is_expired()
	{
		return alive_time() > LOG_EXPIRE_TIME;
	}
	float alive_time()
	{
		return float(glfwGetTime()) - registered_timestamp;
	}
	float text_opacity()
	{
		float alive = alive_time();
		if (alive < LOG_EXPIRE_TIME - LOG_FADEOUT_TIME)
		{
			return 1.f;
		}
		else
		{
			float ropacity =  (alive - LOG_EXPIRE_TIME + LOG_FADEOUT_TIME) / LOG_FADEOUT_TIME;
			return 1.f - ropacity;
		}
	}
	vec4 get_color_style(bool is_force_active)
	{
		float opacity = is_force_active?1.f:text_opacity();
		vec4 factor = vec4(1, 1, 1, opacity);

		switch (announce_level)
		{
		case _VERBOSE_:
			return ucol::console_verbose * factor;
		case _WARNING_:
			return ucol::console_warning * factor;
		case _ERROR_:
			return ucol::console_error * factor;
		case _HIGHLIGHTED_:
			return ucol::console_highlighted * factor;
		default:
			return ucol::console_verbose * factor;
		}
	}
}SystemAnnounceLog;

/*
	Display App's default info
	Top Left Log Panel
*/
typedef class app_info_drawer_ : public StackLogManager
{
public:
	app_info_drawer_() : stacking_log_manager_() {}
	virtual inline void draw(string str)
	{
		draw_string
		(
			str, 
			X_MIN_OFFSET, 
			Y_MIN_OFFSET + LINE_GAP_HEIGHT * stack_index++, 
			text_size, 
			ucol::standard_log
		);
	}
}AppInfoLogger;

/*
	Display static values of variables
	Top Right Log Panel
*/
typedef class static_value_drawer_ : public StackLogManager
{
public:
	static_value_drawer_() : stacking_log_manager_() {}
	virtual inline void draw(string str)
	{
		extern ivec2 window_size;
		draw_string
		(
			str, 
			(GLint)(window_size.x - X_MIN_OFFSET - get_string_width(str, text_size)), 
			Y_MIN_OFFSET + LINE_GAP_HEIGHT * stack_index++, 
			text_size, 
			ucol::standard_log
		);
	}
}StaticValueLogger;

/*
	Display Command History or console
	Bottom Left Log Panel
*/
typedef class command_virtual_window_drawer_ : public ReversedHistoryLogManager
{
private:
	const uint LINE_GAP_HEIGHT = 25;
	const uint CONSOLE_Y_OFFSET = 30;
	string command_builder = "";
	CommandParser command_parser = CommandParser();
	vector<SystemAnnounceLog> announcements;
public:
	bool listener_switch = false;

	command_virtual_window_drawer_() : reversed_history_log_manager_() 
	{
		text_size = 0.4f;
	}
	virtual inline void draw_all()
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
				ucol::white
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
	void add(string content, SystemAnnounceLevel level)
	{
		announcements.push_back(SystemAnnounceLog(level, content));
	}
	void remove_expired()
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
	void activate()
	{
		listener_switch = true;
	}
	void complete()
	{
		string completed = command_builder;
		flush();
		listener_switch = false;

		if (!completed.empty() && completed.at(0) == '/')
		{
			command_parser.accept(completed);
			execute_command();
		}
	}
	void cancel()
	{
		flush();
		listener_switch = false;
	}
	void append(char c)
	{
		command_builder.push_back(c);
	}
	void backspace()
	{
		command_builder.pop_back();
	}
	void flush()
	{
		command_builder.clear();
	}
	void execute_command()
	{
		string keyword = command_parser.get_command_keyword();
		size_t words = command_parser.get_segment_size_except_keyword();

		if (!keyword.compare("help") || !keyword.compare("h"))
		{
			add("You can use command starting with /.", _VERBOSE_);
			return;
		}
		//else if (!keyword.compare("tp"))
		//{
		//	if (words != 3)
		//	{
		//		logger("Invalid parameter. usage: /tp <x> <y> <z>");
		//		return;
		//	}
		//	
		//	vec3 orig = player->pos;

		//	float xf = stof(command_parser.get_segment(0));
		//	float yf = stof(command_parser.get_segment(1));
		//	float zf = stof(command_parser.get_segment(2));

		//	player->pos = vec3(xf, yf, zf);
		//	logger("Teleported to %d, %d, %d to %d, %d, %d", (int)orig.x, (int)orig.y, (int)orig.z, (int)xf, (int)yf, (int)zf);
		//	return;
		//}
		/*else if (!keyword.compare("look"))
		{
			if (words != 3)
			{
				logger("Invalid parameter. usage: /look <x> <y> <z>");
				return;
			}

			float xf = stof(command_parser.get_segment(0));
			float yf = stof(command_parser.get_segment(1));
			float zf = stof(command_parser.get_segment(2));

			player->camera.set_looking_at(vec3(xf, yf, zf));
			logger("Now looking at %.2f, %.2f, %.2f ", xf, yf, zf);
			return;
		}
		else if (!keyword.compare("fix") || !keyword.compare("unfix"))
		{
			if (words == 0)
			{
				if (!keyword.compare("fix"))
				{
					player->set_fix(true);
					logger("Player position fixed.");
					return;
				}
				player->set_fix(false);
				logger("Player position unfixed.");
				return;
			}
			else if (words == 1)
			{
				if (!command_parser.get_segment(0).compare("true"))
				{
					player->set_fix(true);
					logger("Player position fixed.");
					return;
				}
				else if (!command_parser.get_segment(0).compare("false"))
				{
					player->set_fix(false);
					logger("Player position unfixed.");
					return;
				}
			}
			else
			{
				logger("Invalid parameter. usage: /fix <Boolean> or /fix");
				return;
			}
		}
		else if (!keyword.compare("setspeed"))
		{
			if (words != 1)
			{
				return;
			}
			player->set_speed(stof(command_parser.get_segment(0)));
			return;
		}*/

		add("No command keyword for '" + keyword + "'", _WARNING_);
		add("Try /help or /h to see how to use.", _HIGHLIGHTED_);
	}
}CommandConsole;


/*
	Display Game Event Log
	Bottom Right Log Panel
*/
typedef class game_log_drawer_ : public ReversedHistoryLogManager
{
private:
	const uint MAX_LOG_LIMIT = 30;
	const uint LINE_GAP_HEIGHT = 15;
	vector<string> log_history;
public:
	game_log_drawer_() : reversed_history_log_manager_() {}
	virtual inline void draw_all()
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
				ucol::event_log
			);
		}
	}
	inline void add(string str)
	{
		log_history.push_back(get_current_time_string() + " " + str);
		if (log_history.size() > MAX_LOG_LIMIT)
		{
			log_history.erase(log_history.begin());
		}
	}
}GameEventLogger;