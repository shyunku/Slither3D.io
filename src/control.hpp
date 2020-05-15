#pragma once
#include "cgmath.h"
#include "cgut.h"
#include "util.hpp"
#include "glutil.hpp"

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
	//logger("Mouse Motion: (" + to_string(x) + ", " + to_string(y) + ")");
}

void keyboard_event_listener(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static bool use_wireframe = false;
	static bool use_mouselock = false;

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_F1:
			use_wireframe = !use_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, use_wireframe ? GL_LINE : GL_FILL);
			break;
		case GLFW_KEY_F2:
			use_mouselock = !use_mouselock;
			glfwSetInputMode(window, GLFW_CURSOR, use_mouselock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
			break;
		case GLFW_KEY_ESCAPE:
			exit(0);
			break;
		}
	}
}

void window_reshape_listener(GLFWwindow* window, int width, int height)
{
	extern ivec2 window_size;
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}