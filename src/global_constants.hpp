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

static const std::string		app_name = "Slither3D.io";
static const std::string		version_str = "0.1.4v - Beta";

static const uint				FPS_LIMIT = 144;

/* --------------------------- Constant Colors  --------------------------- */
namespace ucol
{
	const vec4 white			= vec4(1.f, 1.f, 1.f, 1.f);
	const vec4 red				= vec4(1.f, 0.f, 0.f, 1.f);
	
	const vec4 light_red		= vec4(1.f, 0.3f, 0.3f, 1.f);

	const vec4 standard_log		= vec4(1.f, 1.f, 1.f, 0.7f);
	const vec4 event_log		= vec4(1.f, 0.9f, 0.6f, 0.7f);
}

/* --------------------------- Constant Ingame  --------------------------- */
const float						WORLD_BORDER_RADIUS = 200.f;