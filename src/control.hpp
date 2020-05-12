#pragma once
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "util.hpp"
#include "glutil.hpp"

void mouse_event_listener(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			dvec2 pos = get_glfw_get_cursor_position();
			logger("Mouse Left Click: (" + to_string(pos.x) + ", " + to_string(pos.y) + ")");
		}
	}
}