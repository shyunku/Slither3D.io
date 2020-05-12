#pragma once
#include "cgmath.h"
#include "cgut.h"
#include "util.hpp"

#include "camera.hpp"


/* --------------------------- Constant Strings  --------------------------- */
static const char*			window_name = "CosmicExplorer";

static const char*			root_path_str = "../bin/";
static const char*			frag_shader_path_str = "shaders/shader.frag";
static const char*			vert_shader_path_str = "shaders/shader.vert";


/* --------------------------- Global Objects  --------------------------- */
GLFWwindow*		window = nullptr;
ivec2			window_size = cg_default_window_size();

GLuint			program_id = 0;
uint			frame_count = 0;

/* --------------------------- Scene Objects  --------------------------- */
Camera			camera;


/* --------------------------- Global Functions  --------------------------- */
void terminate_with_code(uint code)
{
	glfwTerminate();
	exit(code);
}