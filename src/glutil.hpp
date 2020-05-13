#pragma once
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility

dvec2 get_glfw_get_cursor_position(GLFWwindow* window)
{
	dvec2 position;
	glfwGetCursorPos(window, &position.x, &position.y);
	return position;
}

float get_glfw_curtime()
{
	return float(glfwGetTime());
}

void terminate_with_code(uint code)
{
	glfwTerminate();
	exit(code);
}

void set_gl_background_color(ivec3 background_color)
{
	glClearColor(background_color.x / 255.f, background_color.y / 255.f, background_color.z / 255.f, 1.f);
}