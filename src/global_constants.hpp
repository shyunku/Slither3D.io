#pragma once
#include <iostream>
#include "cgmath.h"

/* --------------------------- Constant Strings  --------------------------- */
static const char*				window_name = "CosmicExplorer";

static const char*				root_path_str = "../bin/";
static const char*				default_frag_path = "shaders/default/default.frag";
static const char*				default_vert_path = "shaders/default/default.vert";

static const char*				font_path = "../bin/resources/fonts/consola.ttf";
static const char*				text_frag_path = "shaders/text/text.frag";
static const char*				text_vert_path = "shaders/text/text.vert";

static const std::string		app_name = "CosmicExplorer";
static const std::string		version_str = "0.1.0v";

/* --------------------------- Constant Colors  --------------------------- */
namespace ucol
{
	vec4 white = vec4(1.f, 1.f, 1.f, 1.f);
}