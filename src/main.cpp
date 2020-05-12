#include "global.hpp"
#include "control.hpp"

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
	glClearColor(50 / 255.f, 10 / 255.f, 10 / 255.f, 1.f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// EventListener Setting
	glfwSetMouseButtonCallback(window, mouse_event_listener);
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