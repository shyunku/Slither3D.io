#include "global_constants.hpp"
#include "control.hpp"
#include "game_moderator.hpp"
#include "camera.hpp"
#include "objvert.hpp"
#include "util.hpp"
#include "player.hpp"
#include "text_manager.hpp"


/* --------------------------- Global Objects  --------------------------- */
GLFWwindow*						window = nullptr;
ivec2							window_size = ivec2(960, 540);

uint							frame_count = 0;
float							elapsed_time = 0;
float							time_tick = 0;
ivec3							background_color = ivec3(0);

GameModerator					game_moderator;
KeypressTracker					keypress_tracker;

/* --------------------------- Global Shader Programs  --------------------------- */
GLuint							default_program;
GLuint							text_program;

/* --------------------------- Scene Objects  --------------------------- */
Player							player = Player();

/* --------------------------- Vertex Properties  --------------------------- */
ObjectVertexProperty			sphere_vertex_property;
ObjectVertexProperty			circle_vertex_property;


void update()
{
	static float last_update_flag = 0;

	// Update Camera property
	set_main_camera(player.camera);

	// Update time
	elapsed_time = float(glfwGetTime());
	time_tick = elapsed_time - last_update_flag;
	last_update_flag = elapsed_time;

	// Update Objects
	player.update();
	//if (keypress_tracker.KEY_W)
	//{

	//}
	game_moderator.update(time_tick);
}

void render()
{
	// Clear screen and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

	draw_string("FrameCount: "+to_string(frame_count), 20, 20, 0.3f, ucol::white);
	draw_string(format_string("Elapsed time: %.2fs", elapsed_time), 20, 35, 0.3f, ucol::white);
	draw_string(format_string("Update time tick: %.4fs", time_tick), 20, 50, 0.3f, ucol::white);
	draw_string(format_string("FPS: %d", int(1/time_tick)), 20, 65, 0.3f, ucol::white);
	draw_string(format_string("Camera Pos: (%.2f,%.2f,%.2f)", player.pos.x, player.pos.y, player.pos.z), 20, 80, 0.3f, ucol::white);
	draw_string(format_string("Camera Direction: (%.2f,%.2f,%.2f)", player.camera.look_direction.x, player.camera.look_direction.y, player.camera.look_direction.z),
		20, 95, 0.3f, ucol::white);
	draw_string(format_string("Camera Up: (%.2f,%.2f,%.2f)", player.camera.up.x, player.camera.up.y, player.camera.up.z),
		20, 110, 0.3f, ucol::white);

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

	// EventListener Setting
	glfwSetWindowSizeCallback(window, window_reshape_listener);
	glfwSetMouseButtonCallback(window, mouse_click_event_listener);
	glfwSetKeyCallback(window, keyboard_event_listener);
	glfwSetCursorPosCallback(window, mouse_motion_event_listener);

	/* ------------------ User Method starts ------------------ */
	// Text Setting
	text_initial_setting();

	// Vertex Property Setting
	sphere_vertex_property = create_sphere_vertex_property();
	circle_vertex_property = create_circle_vertex_property();

	// Game Moderator
	game_moderator = GameModerator(sphere_vertex_property, circle_vertex_property);

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

void process_thread()
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
	process_thread();

	return 0;
}