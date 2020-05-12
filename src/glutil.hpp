#pragma once
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "global.hpp"

dvec2 get_glfw_get_cursor_position()
{
	dvec2 position;
	glfwGetCursorPos(window, &position.x, &position.y);
	return position;
}

float get_glfw_curtime()
{
	return float(glfwGetTime());
}