/**
 * WIP
 * @file mg3d.h
 *
 * Byte Format:
 * Sinature (4 bytes) | Vertex count (4 bytes) | verticies (vertex count * sizeof(Vertex) bytes) | Index count (4 bytes) | Indices (index count * 4 bytes)
 * 
 */

MGAPI b8 mg3d_from_obj(const char* obj_path, const char* mg3d_path);
