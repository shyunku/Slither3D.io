#pragma once
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
	system_announce_log_(SystemAnnounceLevel level, string str);
	bool is_expired();
	float alive_time();
	float text_opacity();
	vec4 get_color_style(bool is_force_active);
}SystemAnnounceLog;

/*
	Display App's default info
	Top Left Log Panel
*/
typedef class app_info_drawer_ : public StackLogManager
{
public:
	app_info_drawer_();
	inline void draw(string str);
}AppInfoLogger;

/*
	Display static values of variables
	Top Right Log Panel
*/
typedef class static_value_drawer_ : public StackLogManager
{
public:
	static_value_drawer_();
	inline void draw(string str);
}StaticValueLogger;

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
	game_log_drawer_();
	inline void draw_all();
	inline void add(string str);
}GameEventLogger;

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
	vector<string> command_history;
	string current_command_save = "";
	uint history_cursor = -1;	// -1 indiciates current_command

	void flush();
	void fetch_command_from_history();
	void init_command_history_seeker();
	void execute_command();
public:
	bool listener_switch = false;

	command_virtual_window_drawer_();
	virtual inline void draw_all();
	void add(string content, SystemAnnounceLevel level);
	void remove_expired();
	void activate();
	void complete();
	void cancel();
	void append(char c);
	void backspace();
	void seek_command_history(bool seek_prev);
}CommandConsole;