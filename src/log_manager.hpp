#pragma once
#include "global_constants.hpp"
#include "cgmath.h"
#include "cgut.h"

using namespace std;
extern void draw_string(string text, GLint _x, GLint _y, GLfloat scale, vec4 color);
extern float get_string_width(string str);

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

typedef class command_virtual_window_drawer_ : public StackLogManager
{
public:
	command_virtual_window_drawer_() : stacking_log_manager_() {}
	virtual inline void draw(string str)
	{
		extern ivec2 window_size;
		draw_string
		(
			str, 
			X_MIN_OFFSET, 
			window_size.y - Y_MIN_OFFSET - LINE_GAP_HEIGHT * stack_index++, 
			text_size, 
			ucol::standard_log
		);
	}
}CommandFrameDrawer;

typedef class game_log_drawer_ : public StackLogManager
{
public:
	game_log_drawer_() : stacking_log_manager_() {}
	virtual inline void draw(string str)
	{
		extern ivec2 window_size;
		draw_string(
			str, 
			(GLint)(window_size.x - X_MIN_OFFSET - get_string_width(str, text_size)), 
			window_size.y - Y_MIN_OFFSET - LINE_GAP_HEIGHT * stack_index++, 
			text_size, 
			ucol::standard_log
		);
	}
}GameLogDrawer;