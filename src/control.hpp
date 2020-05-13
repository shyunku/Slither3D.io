#pragma once
#include "cgmath.h"
#include "cgut.h"
#include "util.hpp"
#include "glutil.hpp"

void mouse_event_listener(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			dvec2 pos = get_glfw_get_cursor_position(window);
			logger("Mouse Left Click: (" + to_string(pos.x) + ", " + to_string(pos.y) + ")");
		}
	}
}