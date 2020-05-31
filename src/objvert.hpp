#pragma once
#include "cgmath.h"
#include "cgut.h"

static const uint	large_sphere_latitude_segment_num = 128;	// horizontal lines
static const uint	large_sphere_longitude_segment_num = 64;	// vertical lines

static const uint	small_sphere_latitude_segment_num = 24;		// horizontal lines
static const uint	small_sphere_longitude_segment_num = 48;	// vertical lines

static const uint	tiny_sphere_latitude_segment_num = 2;		// horizontal lines
static const uint	tiny_sphere_longitude_segment_num = 4;		// vertical lines

static const uint	orbit_segment_num = 64;

using namespace std;

typedef class object_vertex_property
{
public:
	uint			vertex_array_ID = 0;
	uint			mesh_count = 0;
	object_vertex_property();
	object_vertex_property(uint id, uint count);
	uint get_triangles_num();
}ObjectVertexProperty;


// Circle vertex property
ObjectVertexProperty create_circle_vertex_property();
// Large Sphere vertex property
ObjectVertexProperty create_large_sphere_vertex_property();
// Small Sphere vertex property
ObjectVertexProperty create_small_sphere_vertex_property();
// Tiny Sphere vertex property
ObjectVertexProperty create_tiny_sphere_vertex_property();