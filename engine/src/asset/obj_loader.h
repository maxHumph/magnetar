/**
 * @file obj_loader.h
 * @brief Contains functions for extracting vertex and index data from obj files.
 */

#pragma once

#include "define.h"
#include "renderer/vulkan/vulkan_defines.h"

#include <stdio.h>

typedef enum ObjLineType {
  OBJ_BLOAT,
  OBJ_VPOS,
  OBJ_VNORM,
  OBJ_VTEX,
  OBJ_FACE,
} ObjLineType;

MGAPI b8 obj_load(const char* path, Vec3** vpos, Vec2** vtex, u32** ipos, u32** itex);

static b8 obj_load_vpos(Vec3** vpos);

static b8 obj_load_vtex(Vec2** vtex);

static b8 obj_load_face(u32** ipos, u32** itex);

static b8 obj_get_line(FILE* file);
