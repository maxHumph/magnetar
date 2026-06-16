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
} ObjLineType;

MGAPI b8 load_obj(const char* path, Vertex** vertices, u32* vertex_count);

static b8 get_obj_line(char* line, u64* line_len, FILE* file, ObjLineType* type);
