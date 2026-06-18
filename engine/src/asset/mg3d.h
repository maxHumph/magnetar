/**
 * WIP
 * @file mg3d.h
 *
 * Byte Format:
 * Sinature (4 bytes) | Vertex count (4 bytes) | verticies (vertex count * sizeof(Vertex) bytes) | Index count (4 bytes) | Indices (index count * 4 bytes)
 * 
 */

#include "define.h"
#include "renderer/vulkan/vulkan_defines.h"

MGAPI b8 mg3d_load(const char* path, Vertex** vbuf, u32* vcount, u32** ibuf, u32* icount);

MGAPI b8 mg3d_from_obj(const char* obj_path, const char* mg3d_path);

static Vertex mg3d_build_vertex(u32 index);

static b8 mg3d_index_vertices(Vertex* src_buf, Vertex** dst_vbuf, u32** dst_ibuf);

static b8 mg3d_write_file(const char* out_path, Vertex* vbuf, u32* ibuf);
