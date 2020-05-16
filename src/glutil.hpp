#pragma once
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "camera.hpp"

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

void set_main_camera(Camera main_camera)
{
	extern GLuint default_program;
	extern ivec2 window_size;

	// Update aspect ratio of camera in terms of window size (resizing)
	// if window wouldn't be resized, please declare static method
	main_camera.set_aspect_ratio(window_size.x / float(window_size.y));
	main_camera.revalidate_projection_matrix();

	// Update uniform variables in shaders
	GLint uloc = glGetUniformLocation(default_program, "view_matrix");
	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, main_camera.view_matrix);
	uloc = glGetUniformLocation(default_program, "projection_matrix");
	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, main_camera.projection_matrix);
}


GLuint create_shader_program(string name)
{
	string frag_fullpath = string(root_path_str) + "shaders/" + name + "/" + name + ".frag";
	string vert_fullpath = string(root_path_str) + "shaders/" + name + "/" + name + ".vert";
	GLint shader = cg_create_program(vert_fullpath.c_str(), frag_fullpath.c_str());

	if (!shader)
	{
		cout << "shader program error!" << endl;
		exit(-1);
	}

	return shader;
}

GLuint get_validated_uniform_location(GLuint shader_program, const char* name)
{
	GLint uloc = glGetUniformLocation(shader_program, name);
	if (uloc < 0)
	{
		cout << name << " is not accessible in this scope." << endl;
		exit(-1);
	}

	return uloc;
}