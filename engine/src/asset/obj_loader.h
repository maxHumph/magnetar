/**
 * @file obj_loader.h
 */

#pragma once

#include "define.h"
#include "renderer/vulkan/vulkan_defines.h"

#include <stdio.h>

typedef enum ObjLineType {
  OBJ_BLOAT,
  OBJ_VERTEX,
  OBJ_NORMAL,
  OBJ_TEXRURE,
  OBJ_INDEX,
} ObjLineType;

MGAPI b8 obj_load(const char* path, Vertex** vertices, u32* vertex_count, u32** indices, u32* index_count);

static b8 obj_load_vertex(Vertex** vertices);

static b8 obj_load_index(u32** vertices);

static b8 obj_get_line(char* line, u64* line_len, FILE* file, ObjLineType* type);
