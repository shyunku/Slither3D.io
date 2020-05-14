#pragma once
#include "cgmath.h"
#include "cgut.h"
#include "util.hpp"
#include "glutil.hpp"

extern ivec2 window_size;

void mouse_click_event_listener(GLFWwindow* window, int button, int action, int mods)
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

void mouse_motion_event_listener(GLFWwindow* window, double x, double y)
{

}

void keyboard_event_listener(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void window_reshape_listener(GLFWwindow* window, int width, int height)
{
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}