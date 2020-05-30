#pragma once
#include "cgut.h"
#include "cgmath.h"

static const uint	large_sphere_latitude_segment_num = 64;		// horizontal lines
static const uint	large_sphere_longitude_segment_num = 128;	// vertical lines

static const uint	small_sphere_latitude_segment_num = 32;		// horizontal lines
static const uint	small_sphere_longitude_segment_num = 64;	// vertical lines

static const uint	orbit_segment_num = 64;

using namespace std;

typedef class object_vertex_property
{
public:
	uint			vertex_array_ID = 0;
	uint			mesh_count = 0;
	object_vertex_property()
	{
		this->vertex_array_ID = 0;
		this->mesh_count = 0;
	}
	object_vertex_property(uint id, uint count)
	{
		this->vertex_array_ID = id;
		this->mesh_count = count;
	}
	uint get_triangles_num()
	{
		return mesh_count * 3;
	}
}ObjectVertexProperty;


// Circle vertex property
ObjectVertexProperty create_circle_vertex_property()
{
	vector<vertex> vertices;

	for (size_t i = 0; i < orbit_segment_num; i++)
	{
		float angle = (float)i * (2 * PI) / (float)orbit_segment_num;
		vertex dots = {
			vec3(cosf(angle), 0, sinf(angle)),
			vec3(cosf(angle), 0, sinf(angle)),
			vec2(0,1) };
		vertices.push_back(dots);
	}

	// indexing vertex buffer
	vector<uint>	indices;
	uint			triangle = 0;
	uint			vertex_array_ID;
	uint			vertex_buffer = 0;
	uint			index_buffer = 0;

	for (int i = 0; i < orbit_segment_num; i++)
	{
		indices.push_back(i);
		indices.push_back((i + 1) % orbit_segment_num);
		indices.push_back((i + 2) % orbit_segment_num);
		triangle++;
	}

	// generation of vertex buffer: use verticess as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	vertex_array_ID = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_ID) { printf("%s(): failed to create vertex aray\n", __func__); exit(-1); }

	return ObjectVertexProperty(vertex_array_ID, triangle);
}

// Large Sphere vertex property
ObjectVertexProperty create_large_sphere_vertex_property()
{
	vertex upper_pole = { vec3(0,1.f,0), vec3(0,1.f,0), vec2(0, 1.f) };
	vertex lower_pole = { vec3(0,-1.f,0), vec3(0,-1.f,0), vec2(0, -1.f) };
	vector<vertex> vertices;

	// exclude pole division
	for (size_t i = 1; i < large_sphere_latitude_segment_num; i++)
	{
		// xy - z
		float lat_angle = PI / 2 - PI * (float)i / float(large_sphere_latitude_segment_num);
		float cl = cosf(lat_angle);
		float sl = sinf(lat_angle);
		for (size_t j = 0; j < large_sphere_longitude_segment_num; j++)
		{
			float long_angle = 2.f * PI * j / float(large_sphere_longitude_segment_num);
			float co = cosf(long_angle);
			float so = sinf(long_angle);
			float x = cl * co;
			float y = sl;
			float z = -cl * so;

			float rate_hor = (float)j / (float)(large_sphere_longitude_segment_num - 1);
			float rate_ver = 1 - (float)i / (float)(large_sphere_latitude_segment_num - 1);
			vec2 texcoord(rate_hor, rate_ver);

			vertices.push_back({ vec3(x,y,z), vec3(x, y, z), texcoord });
		}
	}
	vertices.push_back(upper_pole);
	vertices.push_back(lower_pole);

	// indexing vertex buffer
	vector<uint>	indices;
	uint			vertices_num = vertices.size();
	uint			upper_pole_index = vertices_num - 2;
	uint			lower_pole_index = vertices_num - 1;
	uint			middle_vertex_final_index = vertices_num - 3;
	uint			triangle = 0;
	uint			vertex_array_ID;

	uint			vertex_buffer = 0;
	uint			index_buffer = 0;

	// upper-pole vertex indexing
	for (size_t i = 0; i < large_sphere_longitude_segment_num - 1; i++)
	{
		indices.push_back(upper_pole_index);
		indices.push_back(i);
		indices.push_back(i + 1);
		triangle++;
	}
	// end of index
	indices.push_back(upper_pole_index);
	indices.push_back(large_sphere_longitude_segment_num - 1);
	indices.push_back(0);
	triangle++;

	// medival vertices indexing
	uint buffer_index;
	for (size_t i = 1; i < large_sphere_latitude_segment_num - 1; i++)
	{
		for (size_t j = 0; j < large_sphere_longitude_segment_num - 1; j++)
		{
			buffer_index = large_sphere_longitude_segment_num * (i - 1) + j;
			// upper triangle
			indices.push_back(buffer_index);
			indices.push_back(buffer_index + large_sphere_longitude_segment_num);
			indices.push_back(buffer_index + 1);
			triangle++;

			// lower triangle
			indices.push_back(buffer_index + 1);
			indices.push_back(buffer_index + large_sphere_longitude_segment_num);
			indices.push_back(buffer_index + large_sphere_longitude_segment_num + 1);
			triangle++;
		}

		// end of index at each of ring
		uint horizontal_max_index = large_sphere_longitude_segment_num * i - 1;

		// upper triangle
		indices.push_back(horizontal_max_index);
		indices.push_back(horizontal_max_index + large_sphere_longitude_segment_num);
		indices.push_back(horizontal_max_index + 1 - large_sphere_longitude_segment_num);
		triangle++;

		// lower triangle
		indices.push_back(horizontal_max_index + 1 - large_sphere_longitude_segment_num);
		indices.push_back(horizontal_max_index + large_sphere_longitude_segment_num);
		indices.push_back(horizontal_max_index + 1);
		triangle++;
	}

	// lower-pole vertex indexing
	for (size_t i = 0; i < large_sphere_longitude_segment_num - 1; i++)
	{
		indices.push_back(lower_pole_index);
		indices.push_back(middle_vertex_final_index - i);
		indices.push_back(middle_vertex_final_index - i - 1);
		triangle++;
	}
	// end of index
	indices.push_back(lower_pole_index);
	indices.push_back(middle_vertex_final_index - large_sphere_longitude_segment_num + 1);
	indices.push_back(middle_vertex_final_index);
	triangle++;

	// generation of vertex buffer: use verticess as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//if (vertex_array_ID) glDeleteVertexArrays(1, &vertex_array_ID);
	vertex_array_ID = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_ID) { printf("%s(): failed to create vertex aray\n", __func__); exit(-1); }

	return ObjectVertexProperty(vertex_array_ID, triangle);
}

ObjectVertexProperty create_small_sphere_vertex_property()
{
	vertex upper_pole = { vec3(0,1.f,0), vec3(0,1.f,0), vec2(0, 1.f) };
	vertex lower_pole = { vec3(0,-1.f,0), vec3(0,-1.f,0), vec2(0, -1.f) };
	vector<vertex> vertices;

	// exclude pole division
	for (size_t i = 1; i < small_sphere_latitude_segment_num; i++)
	{
		// xy - z
		float lat_angle = PI / 2 - PI * (float)i / float(small_sphere_latitude_segment_num);
		float cl = cosf(lat_angle);
		float sl = sinf(lat_angle);
		for (size_t j = 0; j < small_sphere_longitude_segment_num; j++)
		{
			float long_angle = 2.f * PI * j / float(small_sphere_longitude_segment_num);
			float co = cosf(long_angle);
			float so = sinf(long_angle);
			float x = cl * co;
			float y = sl;
			float z = -cl * so;

			float rate_hor = (float)j / (float)(small_sphere_longitude_segment_num - 1);
			float rate_ver = 1 - (float)i / (float)(small_sphere_latitude_segment_num - 1);
			vec2 texcoord(rate_hor, rate_ver);

			vertices.push_back({ vec3(x,y,z), vec3(x, y, z), texcoord });
		}
	}
	vertices.push_back(upper_pole);
	vertices.push_back(lower_pole);

	// indexing vertex buffer
	vector<uint>	indices;
	uint			vertices_num = vertices.size();
	uint			upper_pole_index = vertices_num - 2;
	uint			lower_pole_index = vertices_num - 1;
	uint			middle_vertex_final_index = vertices_num - 3;
	uint			triangle = 0;
	uint			vertex_array_ID;

	uint			vertex_buffer = 0;
	uint			index_buffer = 0;

	// upper-pole vertex indexing
	for (size_t i = 0; i < small_sphere_longitude_segment_num - 1; i++)
	{
		indices.push_back(upper_pole_index);
		indices.push_back(i);
		indices.push_back(i + 1);
		triangle++;
	}
	// end of index
	indices.push_back(upper_pole_index);
	indices.push_back(small_sphere_longitude_segment_num - 1);
	indices.push_back(0);
	triangle++;

	// medival vertices indexing
	uint buffer_index;
	for (size_t i = 1; i < small_sphere_latitude_segment_num - 1; i++)
	{
		for (size_t j = 0; j < small_sphere_longitude_segment_num - 1; j++)
		{
			buffer_index = small_sphere_longitude_segment_num * (i - 1) + j;
			// upper triangle
			indices.push_back(buffer_index);
			indices.push_back(buffer_index + small_sphere_longitude_segment_num);
			indices.push_back(buffer_index + 1);
			triangle++;

			// lower triangle
			indices.push_back(buffer_index + 1);
			indices.push_back(buffer_index + small_sphere_longitude_segment_num);
			indices.push_back(buffer_index + small_sphere_longitude_segment_num + 1);
			triangle++;
		}

		// end of index at each of ring
		uint horizontal_max_index = small_sphere_longitude_segment_num * i - 1;

		// upper triangle
		indices.push_back(horizontal_max_index);
		indices.push_back(horizontal_max_index + small_sphere_longitude_segment_num);
		indices.push_back(horizontal_max_index + 1 - small_sphere_longitude_segment_num);
		triangle++;

		// lower triangle
		indices.push_back(horizontal_max_index + 1 - small_sphere_longitude_segment_num);
		indices.push_back(horizontal_max_index + small_sphere_longitude_segment_num);
		indices.push_back(horizontal_max_index + 1);
		triangle++;
	}

	// lower-pole vertex indexing
	for (size_t i = 0; i < small_sphere_longitude_segment_num - 1; i++)
	{
		indices.push_back(lower_pole_index);
		indices.push_back(middle_vertex_final_index - i);
		indices.push_back(middle_vertex_final_index - i - 1);
		triangle++;
	}
	// end of index
	indices.push_back(lower_pole_index);
	indices.push_back(middle_vertex_final_index - small_sphere_longitude_segment_num + 1);
	indices.push_back(middle_vertex_final_index);
	triangle++;

	// generation of vertex buffer: use verticess as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//if (vertex_array_ID) glDeleteVertexArrays(1, &vertex_array_ID);
	vertex_array_ID = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_ID) { printf("%s(): failed to create vertex aray\n", __func__); exit(-1); }

	return ObjectVertexProperty(vertex_array_ID, triangle);
}