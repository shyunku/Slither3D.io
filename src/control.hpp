#pragma once
#include "cgmath.h"
#include "cgut.h"


typedef struct track_keypress 
{
	bool KEY_W;
	bool KEY_A;
	bool KEY_S;
	bool KEY_D;
}KeypressTracker;

void mouse_wheel_event_listener(GLFWwindow* window, double xoffset, double yoffset);
void mouse_click_event_listener(GLFWwindow* window, int button, int action, int mods);
void mouse_motion_event_listener(GLFWwindow* window, double x, double y);
void key_type_event_listener(GLFWwindow* window, GLuint codepoint);
void keyboard_event_listener(GLFWwindow* window, int key, int scancode, int action, int mods);
void window_reshape_listener(GLFWwindow* window, int width, int height);