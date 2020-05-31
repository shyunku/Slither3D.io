#include "player.hpp"

#include "command.hpp"
#include "control.hpp"
#include "glutil.hpp"
#include "game_moderator.hpp"

/* --------------------------- Constant Strings  --------------------------- */
const char*				root_path_str = "../bin/";
const char*				default_frag_path = "shaders/default/default.frag";
const char*				default_vert_path = "shaders/default/default.vert";

const char*				font_path = "../bin/resources/fonts/consola.ttf";
const char*				text_frag_path = "shaders/text/text.frag";
const char*				text_vert_path = "shaders/text/text.vert";

string					app_name = "Slither3D.io";
string					version_str = "0.4.1v - Beta";

const uint				FPS_LIMIT = 144;

/* --------------------------- Constant Colors  --------------------------- */

const vec4 white = vec4(1.f, 1.f, 1.f, 1.f);
const vec4 red = vec4(1.f, 0.f, 0.f, 1.f);

const vec4 light_red = vec4(1.f, 0.3f, 0.3f, 1.f);

const vec4 standard_log = vec4(1.f, 1.f, 1.f, 0.7f);
const vec4 event_log = vec4(1.f, 0.9f, 0.6f, 0.7f);

const vec4 console_verbose = vec4(1.f, 1.f, 1.f, 1.f);
const vec4 console_warning = vec4(1.f, 1.f, .6f, 1.f);
const vec4 console_error = vec4(1.f, .5f, .5f, 1.f);
const vec4 console_highlighted = vec4(.6f, 1.f, 1.f, 1.f);

/* --------------------------- Constant Ingame  --------------------------- */
const float						WORLD_BORDER_RADIUS = 100.f;

/* --------------------------- Global Objects  --------------------------- */
GLFWwindow*						window = nullptr;
ivec2							window_size = ivec2(1080, 720);

uint							frame_count = 0;
float							elapsed_time = 0;
float							time_tick = 0;
ivec3							background_color = ivec3(0);

GameModerator					game_moderator;
KeypressTracker					keypress_tracker;

AppInfoLogger					ail;
StaticValueLogger				svl;
CommandConsole					console;
GameEventLogger					gevent;


/* --------------------------- Global Shader Programs  --------------------------- */
GLuint							default_program;
GLuint							text_program;
GLuint							glow_program;

/* --------------------------- Scene Objects  --------------------------- */
Player*							player = NULL;

/* --------------------------- Vertex Properties  --------------------------- */
ObjectVertexProperty			large_sphere_vertex_property;
ObjectVertexProperty			small_sphere_vertex_property;
ObjectVertexProperty			circle_vertex_property;


void update()
{
	static float last_update_flag = 0;

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
	ail.draw(app_name + " " + version_str);

	// Static Value Drawer
	svl.draw("FrameCount: " + to_string(frame_count));
	svl.draw(format_string("Elapsed time: %.2fs", elapsed_time));
	svl.draw(format_string("Update time tick: %.4fs", time_tick));
	svl.draw(format_string("FPS: %d", int(fFPS)));
	svl.blank(1);
	svl.draw("Console Listener: " + format_string(console.listener_switch ? "true" : "false"));
	svl.draw("Console KeyListener Disabler: " + format_string(console.key_listener_disabler ? "true" : "false"));
	svl.blank(2);
	svl.draw(format_string("Player Worm ID: %d", player->possess_worm));
	svl.draw(format_string("Alive Worms: %d", game_moderator.ingame_object_manager.worms.size()));
	svl.draw(format_string("Alive Preys: %d", game_moderator.ingame_object_manager.preys.size()));
	svl.blank(1);
	svl.draw(format_string("Intented Worm ID: %d", player->me->get_id()));
	svl.draw(format_string("Growth: %d", int(player->me->growth)));
	svl.draw(format_string("Head Pos: " + get_vec3_string(player->me->head.pos)));
	svl.draw(format_string("Look Direction: " + get_vec3_string(player->camera.look_direction)));
	svl.draw(format_string("Move Direction: " + get_vec3_string(player->me->head.direction)));
	svl.draw(format_string("AI status: %s", player->me->get_ai_status().c_str()));

	// Game Event Log Panel
	gevent.draw_all();
	console.draw_all();

	// Clear Stacked Logs
	ail.clear_stack();
	svl.clear_stack();

	// Render Objects
	game_moderator.render();

	// Swap Front and Back Buffers and display to screen
	glfwSwapBuffers(window);
}

void initialize_environment()
{
	// Generate Window
	window = cg_create_window(app_name.c_str(), window_size.x, window_size.y);	
	if (!window) terminate_with_code(1);
	if (!cg_init_extensions(window)) terminate_with_code(2);
	
	// Shader Setting
	default_program = create_shader_program("default");
	glow_program = create_shader_program("glow");

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
	large_sphere_vertex_property = create_large_sphere_vertex_property();
	small_sphere_vertex_property = create_small_sphere_vertex_property();
	circle_vertex_property = create_circle_vertex_property();

	// Player Init
	player = new Player();

	// Game Moderator
	game_moderator = GameModerator(large_sphere_vertex_property, small_sphere_vertex_property, circle_vertex_property);

	// Debug
	player->me->set_fix(true);
	player->me->set_pos(vec3(150, 0, 150));


	// Initial User Action
	srand((unsigned)time(NULL));
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