#include "global_constants.hpp"
#include "control.hpp"
#include "game_moderator.hpp"
#include "camera.hpp"
#include "objvert.hpp"
#include "util.hpp"

/* --------------------------- Property Offsets --------------------------- */

/* --------------------------- Global Objects  --------------------------- */
GLFWwindow*						window = nullptr;
ivec2							window_size = ivec2(960, 540);

GLuint							program_id = 0;
uint							frame_count = 0;
ivec3							background_color = ivec3(0);

GameModerator					game_moderator;

/* --------------------------- Scene Objects  --------------------------- */
Camera							camera;

/* --------------------------- Vertex Properties  --------------------------- */
ObjectVertexProperty			sphere_vertex_property = ObjectVertexProperty();


void update()
{
	GLint uloc;

	// Update aspect ratio of camera in terms of window size (resizing)
	// if window wouldn't be resized, please declare static method
	camera.set_aspect_ratio(window_size.x / float(window_size.y));
	camera.revalidate_projection_matrix();

	// Update uniform variables in shaders
	uloc = glGetUniformLocation(program_id, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, camera.view_matrix);
	uloc = glGetUniformLocation(program_id, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, camera.projection_matrix);

	// Update Objects

}

void render()
{
	GLint uloc = 0;

	// Time Usage : float time = get_glfw_curtime();

	// Clear screen and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

	// Notify our program id to GL
	glUseProgram(program_id);			

	// Render Objects
	game_moderator.render(program_id);

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
	string shader_frag_path = string(root_path_str) + string(frag_shader_path_str);
	string shader_vert_path = string(root_path_str) + string(vert_shader_path_str);
	program_id = cg_create_program(shader_vert_path.c_str(), shader_frag_path.c_str());
	if (!program_id) terminate_with_code(3);

	// GL State Setting
	set_gl_background_color(background_color);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// EventListener Setting
	glfwSetWindowSizeCallback(window, window_reshape_listener);
	glfwSetMouseButtonCallback(window, mouse_click_event_listener);
	glfwSetKeyCallback(window, keyboard_event_listener);
	glfwSetCursorPosCallback(window, mouse_motion_event_listener);

	// Vertex Property Setting
	sphere_vertex_property = create_sphere_vertex_property();

	// Game Moderator
	game_moderator = GameModerator(sphere_vertex_property);

	// Initial User Action
	print_version_of_app();
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