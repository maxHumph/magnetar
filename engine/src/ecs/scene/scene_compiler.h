/**
 * @file scene_compiler.h
 */

#pragma once

#include "define.h"
#include "data/mg_string.h"
#include "ecs/ecs_types.h"

#include <stdint.h>

#define SCENE_BIN_SIG {'m', 'g', 's', 'b'}

typedef enum SceneObjIndex {
  SCENE_OBJ_INDEX_ENTITY,
  
  SCENE_OBJ_INDEX_CODE,
  SCENE_OBJ_INDEX_MESH,
  SCENE_OBJ_INDEX_MATERIAL,
  SCENE_OBJ_INDEX_TEXTURE,
  SCENE_OBJ_INDEX_CAMERA,
  SCENE_OBJ_INDEX_BOX_COLLIDER_3D,
  SCENE_OBJ_INDEX_BALL_COLLIDER_3D,

  MAX_SCENE_OBJ_INDEX
} SceneObjIndex;

typedef struct scene_bin_header_t {

  char signature[4];
  int32_t obj_counts[MAX_SCENE_OBJ_INDEX];
    
} scene_bin_header_t;

b8 scene_compile(SceneData* data, StringView path);

b8 scene_compiler_generate_data(SceneData* scene_data, char** data, u32* size);

b8 scene_compiler_write_file(StringView path, char* data, u32 size);
