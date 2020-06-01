#pragma once
#include "cgmath.h"
#include "cgut.h"

using namespace std;

extern GLuint text_program;
extern const char* font_path;


void create_font_texture();
void text_initial_setting();
void draw_string(string text, GLint _x, GLint _y, GLfloat scale, vec4 color);
float get_string_width(string str, float scale);
void draw_centered_string(string text, GLint _x, GLint _y, GLfloat scale, vec4 color);