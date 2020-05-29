#pragma once
#include "global_constants.hpp"
#include "cgmath.h"
#include "cgut.h"

using namespace std;
extern void draw_string(string text, GLint _x, GLint _y, GLfloat scale, vec4 color);
extern float get_string_width(string str, float scale);
extern string get_current_time_string();

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
	const uint MAX_LOG_LIMIT = 30;
	const uint X_MIN_OFFSET = 10;
	const uint Y_MIN_OFFSET = 20;
	const uint LINE_GAP_HEIGHT = 15;
	vector<string> log_history;
	float text_size;
	bool preserve;
public:
	reversed_history_log_manager_(bool ipreserve) : preserve(ipreserve)
	{
		text_size = 0.3f;
	}
	virtual inline void draw_all() = 0;
	inline void add(string str)
	{
		log_history.push_back(get_current_time_string() + " " + str);
		if (!preserve && log_history.size() > MAX_LOG_LIMIT)
		{
			log_history.erase(log_history.begin());
		}
	}
}ReversedHistoryLogManager;


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
}AppInfoDrawer;

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
}StaticValueDrawer;

/*
	Display Command History or console
	Bottom Left Log Panel
*/
typedef class command_virtual_window_drawer_ : public ReversedHistoryLogManager
{
public:
	command_virtual_window_drawer_() : reversed_history_log_manager_(false) {}
	virtual inline void draw_all()
	{
		extern ivec2 window_size;
		for (uint i = log_history.size() - 1, j = 0; i >= 0; i--, j++)
		{
			draw_string
			(
				get_current_time_string() + " " + log_history.at(i),
				X_MIN_OFFSET,
				window_size.y - Y_MIN_OFFSET - LINE_GAP_HEIGHT * j,
				text_size,
				ucol::standard_log
			);
		}
	}
}CommandFrameDrawer;


/*
	Display Game Event Log
	Bottom Right Log Panel
*/
typedef class game_log_drawer_ : public ReversedHistoryLogManager
{
public:
	game_log_drawer_() : reversed_history_log_manager_(true) {}
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
}GameLogDrawer;