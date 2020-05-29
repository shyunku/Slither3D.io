#include "control.hpp"
#include "game_moderator.hpp"
#include "camera.hpp"
#include "objvert.hpp"
#include "util.hpp"
#include "player.hpp"
#include "text_manager.hpp"
#include "command.hpp"
#include "log_manager.hpp"


/* --------------------------- Global Objects  --------------------------- */
GLFWwindow*						window = nullptr;
ivec2							window_size = ivec2(1080, 720);

uint							frame_count = 0;
float							elapsed_time = 0;
float							time_tick = 0;
ivec3							background_color = ivec3(0);

GameModerator					game_moderator;
KeypressTracker					keypress_tracker;

AppInfoDrawer					aid;
StaticValueDrawer				svd;
CommandFrameDrawer				cfd;
GameLogDrawer					gld;


/* --------------------------- Global Shader Programs  --------------------------- */
GLuint							default_program;
GLuint							text_program;

/* --------------------------- Scene Objects  --------------------------- */
Player*							player = NULL;

/* --------------------------- Vertex Properties  --------------------------- */
ObjectVertexProperty			sphere_vertex_property;
ObjectVertexProperty			circle_vertex_property;


void update()
{
	static float last_update_flag = 0;

	// Update Camera property
	set_main_camera(player->camera);

	// Update time
	elapsed_time = float(glfwGetTime());
	time_tick = elapsed_time - last_update_flag;
	last_update_flag = elapsed_time;

	// Update Objects
	player->update(time_tick);
	//if (keypress_tracker.KEY_W)
	//{

	//}
	game_moderator.update(time_tick);

	//Sleep((int)(1000.f / FPS_LIMIT));
}

void render()
{
	// Clear screen and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

	float fFPS = 1.f / time_tick;

	// App Information Draw
	aid.draw(app_name + " " + version_str);

	// Static Value Drawer
	svd.draw("FrameCount: " + to_string(frame_count));
	svd.draw(format_string("Elapsed time: %.2fs", elapsed_time));
	svd.draw(format_string("Update time tick: %.4fs", time_tick));
	svd.draw(format_string("FPS: %d", int(fFPS)));

	// Game Event Log Panel
	gld.draw_all();
	cfd.draw_all();

	// Clear Stacked Logs
	aid.clear_stack();
	svd.clear_stack();

	// Render Objects
	game_moderator.render();

	// Swap Front and Back Buffers and display to screen
	glfwSwapBuffers(window);
}

void initialize_environment()
{
	// Generate Window
	window = cg_create_window(window_name, window_size.x, window_size.y);	
	if (!window) terminate_with_code(1);
	if (!cg_init_extensions(window)) terminate_with_code(2);
	
	// Shader Setting
	default_program = create_shader_program("default");

	// GL State Setting
	set_gl_background_color(background_color);
	glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// EventListener Setting
	glfwSetWindowSizeCallback(window, window_reshape_listener);
	glfwSetMouseButtonCallback(window, mouse_click_event_listener);
	glfwSetKeyCallback(window, keyboard_event_listener);
	glfwSetCursorPosCallback(window, mouse_motion_event_listener);
	glfwSetCharCallback(window, key_type_event_listener);

	/* ------------------ User Method starts ------------------ */
	// Text Setting
	text_initial_setting();

	// Vertex Property Setting
	sphere_vertex_property = create_sphere_vertex_property();
	circle_vertex_property = create_circle_vertex_property();

	// Game Moderator
	game_moderator = GameModerator(sphere_vertex_property, circle_vertex_property);

	player = new Player(game_moderator.ingame_object_manager.get_player());

	// Initial User Action
	print_version_of_app();
	keypress_tracker =
	{
		false,
		false,
		false,
		false,
	};
}

void main_process()
{
	for (; !glfwWindowShouldClose(window); frame_count++)
	{
		glfwPollEvents();
		update();
		render();
	}
}

int main(int argc, char* argv[])
{
	initialize_environment();

	main_process();

	return 0;
}