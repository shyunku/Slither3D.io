#include "text_manager.hpp"

#define STB_TRUETYPE_IMPLEMENTATION

#include "stb_truetype.h"
#include "glutil.hpp"
#include <iostream>

struct stbtt_char_t
{
	GLuint	textureID;
	ivec2	size;
	ivec2	bearing;
	GLuint	advance;
};

GLuint VAO, VBO;
std::map<GLchar, stbtt_char_t> stbtt_char_list;
stbtt_fontinfo font_info;

void create_font_texture()
{
	// Create Font Texture for each glyphs
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++)
	{
		float font_scale = stbtt_ScaleForPixelHeight(&font_info, 48);
		int width, height, offset_x, offset_y;
		unsigned char* bitmap = stbtt_GetCodepointBitmap(
			&font_info,
			0.f,
			font_scale,
			c,
			&width,
			&height,
			&offset_x,
			&offset_y
		);

		GLuint texture_text;
		glGenTextures(1, &texture_text);
		glBindTexture(GL_TEXTURE_2D, texture_text);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			width,
			height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bitmap
		);

		stbtt_FreeBitmap(bitmap, font_info.userdata);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int advance_width = 0;
		stbtt_GetCodepointHMetrics(&font_info, c, &advance_width, nullptr);
		stbtt_char_t character =
		{
			texture_text,
			ivec2(width, height),
			ivec2(offset_x, -offset_y),
			GLuint(advance_width * font_scale)
		};

		stbtt_char_list.insert(std::pair<GLchar, stbtt_char_t>(c, character));
	}
}

void text_initial_setting()
{
	FILE* font_file = fopen(font_path, "rb");

	if (font_file == NULL)
	{
		cout << "Failed to find font file!" << endl;
		exit(-1);
	}

	fseek(font_file, 0, SEEK_END);
	const uint font_file_size = ftell(font_file);
	fseek(font_file, 0, SEEK_SET);

	unsigned char* font_buffer = new unsigned char[font_file_size];
	fread(font_buffer, 1, font_file_size, font_file);
	fclose(font_file);

	if (!stbtt_InitFont(&font_info, font_buffer, 0))
	{
		cout << "Failed to initialize font!" << endl;
		exit(-1);
	}

	create_font_texture();
	free(font_buffer);

	string text_frag_fullpath = string(root_path_str) + string(text_frag_path);
	string text_vert_fullpath = string(root_path_str) + string(text_vert_path);

	text_program = create_shader_program("text");


	// Create Quad
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLfloat vertices[6][4] = {
		{0,1,0,0},
		{0,0,0,1},
		{1,0,1,1},
		{0,1,0,0},
		{1,0,1,1},
		{1,1,1,0},
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_string(string text, GLint _x, GLint _y, GLfloat scale, vec4 color)
{
	extern ivec2 window_size;
	GLfloat x = GLfloat(_x);
	GLfloat y = GLfloat(_y);
	GLint uloc = glGetUniformLocation(text_program, "textColor");

	if (uloc < 0)
	{
		cout << "textColor not defined in this scope: " << __func__ << endl;
		exit(-1);
	}

	glUseProgram(text_program);
	glUniform4f(uloc, color.r, color.g, color.b, color.a);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	mat4 text_offset =
		mat4(
			1 / (window_size.x / 2.f), 0, 0, -1,
			0, 1 / (window_size.y / 2.f), 0, 1,
			0, 0, 1, 0,
			0, 0, 0, 1
		);

	for (string::const_iterator c = text.begin(); c != text.end(); c++)
	{
		stbtt_char_t ch = stbtt_char_list[*c];
		mat4 text_size =
			mat4(
				scale * float(ch.size.x), 0, 0, 0,
				0, scale * float(ch.size.y), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			);
		mat4 text_translate =
			mat4(
				1, 0, 0, x + scale * float(ch.bearing.x),
				0, 1, 0, -y + scale * float(-(ch.size.y - ch.bearing.y)),
				0, 0, 1, 0,
				0, 0, 0, 1
			);
		mat4 text_matrix = mat4();
		text_matrix = text_offset * text_translate * text_size * text_matrix;

		if (uloc < 0)
		{
			cout << "text matrix not defined in this scope: " << __func__ << endl;
			exit(-1);
		}

		uloc = glGetUniformLocation(text_program, "text_matrix");
		glUniformMatrix4fv(uloc, 1, GL_TRUE, text_matrix);

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += ch.advance * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

float get_string_width(string str, float scale)
{
	float width = 0;
	for (string::const_iterator c = str.begin(); c != str.end(); c++)
	{
		stbtt_char_t ch = stbtt_char_list[*c];
		width += ch.advance * scale;
	}

	return width;
}