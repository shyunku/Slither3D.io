#include "control.hpp"

#include "camera.hpp"
#include "player.hpp"
#include "glutil.hpp"
#include "util.hpp"
#include "log_manager.hpp"

extern CommandConsole console;
extern Player* player;

void mouse_click_event_listener(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			if (player->me->get_id() == player->possess_worm)
			{
				player->me->boosting = true;
			}
		}
		else if(action == GLFW_RELEASE)
		{
			if (player->me->get_id() == player->possess_worm)
			{
				player->me->boosting = false;
			}
		}
	}
}

void mouse_motion_event_listener(GLFWwindow* window, double x, double y)
{
	static double last_x = 0;
	static double last_y = 0;

	float x_diff = float(x - last_x);
	float y_diff = -float(-y + last_y);

	last_x = x;
	last_y = y;

	player->camera.look_direction_adjust(x_diff, y_diff);
	player->set_move_direction();
	//logger("Mouse Motion: (" + to_string(x_diff) + ", " + to_string(y_diff) + ")");
}

void key_type_event_listener(GLFWwindow* window, GLuint codepoint)
{
	extern CommandConsole console;
	if (console.listener_switch && !console.key_listener_disabler)
	{
		console.append(char(codepoint));
	}
}

void keyboard_event_listener(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static bool use_wireframe = false;
	static bool use_mouselock = false;
	static bool use_fullscreen = false;

	static ivec2 original_window_size;
	extern KeypressTracker keypress_tracker;

	if (console.listener_switch)
	{
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_ENTER:		// disable command_mode
				console.complete();
				break;
			case GLFW_KEY_ESCAPE:
				console.cancel();
				break;
			case GLFW_KEY_BACKSPACE:
				console.backspace();
				break;
			case GLFW_KEY_UP:
				console.seek_command_history(true);
				break;
			case GLFW_KEY_DOWN:
				console.seek_command_history(false);
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_T:
				console.enable_keylistener();
				break;
			}
		}
	}
	else if (key == GLFW_KEY_SLASH && action == GLFW_PRESS)
	{
		console.activate();
	}
	else if (action == GLFW_PRESS)
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
		case GLFW_KEY_W:
			keypress_tracker.KEY_W = true;
			break;
		case GLFW_KEY_A:
			keypress_tracker.KEY_A = true;
			break;
		case GLFW_KEY_S:
			keypress_tracker.KEY_S = true;
			break;
		case GLFW_KEY_D:
			keypress_tracker.KEY_D = true;
			break;
		case GLFW_KEY_T:
			console.activate();
			console.disable_keylistener();
			break;
		case GLFW_KEY_F11:
			use_fullscreen = !use_fullscreen;
			use_mouselock = use_fullscreen;
			glfwSetInputMode(window, GLFW_CURSOR, use_mouselock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

			extern GLFWwindow* window;
			extern ivec2 window_size;
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

			if (use_fullscreen)
			{
				original_window_size = window_size;

				glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
			}
			else
			{
				glfwSetWindowMonitor(
					window,
					NULL,
					mode->width / 2 - original_window_size.x / 2,
					mode->height / 2 - original_window_size.y / 2,
					original_window_size.x,
					original_window_size.y,
					GLFW_DONT_CARE);
			}
			break;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			keypress_tracker.KEY_W = false;
			break;
		case GLFW_KEY_A:
			keypress_tracker.KEY_A = false;
			break;
		case GLFW_KEY_S:
			keypress_tracker.KEY_S = false;
			break;
		case GLFW_KEY_D:
			keypress_tracker.KEY_D = false;
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