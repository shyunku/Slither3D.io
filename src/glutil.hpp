#pragma once
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility

extern const char* root_path_str;

using namespace std;

dvec2 get_glfw_get_cursor_position(GLFWwindow* window);
float get_glfw_curtime();
void terminate_with_code(uint code);
void set_gl_background_color(ivec3 background_color);
void set_shader_vertex_project(GLuint shader);
void set_main_shader(GLuint shader);
GLuint create_shader_program(string name);
GLuint get_validated_uniform_location(GLuint shader_program, const char* name);