/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ecs_manager.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "asset/mg3d.h"
#include "renderer/renderer_frontend.h"
#include "data_structures/darray.h"
#include "scene_text_loader.h"

static SceneData scene_data = {};

b8 scene_load(Scene* scene) {
  scene_data.entities = darray_create(Entity);
  scene_data.drawable_handles = darray_create(Handle32);

  scene_data.codes = darray_create(CCode);
  scene_data.meshes = darray_create(CMesh);
  scene_data.materials = darray_create(CMaterial);
  scene_data.textures = darray_create(CCTexture);
  scene_data.cameras = darray_create(CCamera);

  scene_data.mesh_assets = darray_create(AMesh);
  scene_data.texture_assets = darray_create(ATexture);

  for (u32 i = 0; i < scene->entity_count; i++) {
    if (!entity_load(scene->entity_handles[i])) {
      MERROR_CORE("Failed to load entity");
      return FALSE;
    }
  }


  return TRUE;
};

b8 scene_unload(Scene* scene) {
  for (u32 i = 0; i < scene->entity_count; i++) {
    if (!entity_unload(scene->entity_handles[i])) {
      MERROR_CORE("Failed to unload entity");
      return FALSE;
    }
  }

  darray_destroy(scene_data.entities);

  darray_destroy(scene_data.drawable_handles);
  darray_destroy(scene_data.meshes);
  darray_destroy(scene_data.materials);
  darray_destroy(scene_data.textures);
  darray_destroy(scene_data.cameras);

  darray_destroy(scene_data.mesh_assets);
  darray_destroy(scene_data.texture_assets);

  scene_data.entities = NULL_PTR;

  scene_data.drawable_handles = NULL_PTR;
  scene_data.meshes = NULL_PTR;
  scene_data.materials = NULL_PTR;
  scene_data.textures = NULL_PTR;
  scene_data.cameras = NULL_PTR;

  scene_data.mesh_assets = NULL_PTR;
  scene_data.texture_assets = NULL_PTR;

  return TRUE;
};

b8 scene_load_text(const char* path) {

  scene_data.entities = darray_create(Entity);

  scene_data.drawable_handles = darray_create(Handle32);
  scene_data.meshes = darray_create(CMesh);
  scene_data.materials = darray_create(CMaterial);
  scene_data.textures = darray_create(CCTexture);
  scene_data.cameras = darray_create(CCamera);

  scene_data.mesh_assets = darray_create(AMesh);
  scene_data.texture_assets = darray_create(ATexture);

  FILE* file = fopen(path, "r");
  if (file == NULL_PTR) {
    MERROR_CORE("Failed to open file: %s", path);
    return FALSE;
  }

  char* line = NULL_PTR;
  size_t line_len = 0;

  while ((line_len = getline(&line, &line_len, file)) != -1) {
    char* pos = strstr(line, "SCENE");
    if (pos != NULL_PTR) {
      parse_scene(&scene_data, file);
    }
  }


  fclose(file);
  return TRUE;
}

SceneData* get_scene_data_ptr() {
  return &scene_data;
}

MGAPI b8 camera_set_active(Handle32 entity_handle) {
  if (scene_data.entities[entity_handle].components[COMPONENT_INDEX_CAMERA] != NULL_HANDLE_32) {
    scene_data.active_camera_entity = entity_handle;
    return TRUE;
  }
  return FALSE;
}

static b8 entity_load(Handle32 entity_handle) {
  return TRUE;
};

static b8 entity_unload(Handle32 entity_handle) {
  MWARN_CORE("entity_unload() memory_leak!!!");
  return TRUE;
};

static b8 cmesh_load(Handle32 mesh_handle) {
  return TRUE;
}

static b8 cmesh_unload(Handle32 mesh_handle) {
  MWARN_CORE("cmesh_unload() memory_leak!!!");
  return TRUE;
}

static b8 cmaterial_load(Handle32 material_handle) {
  return TRUE;
}

static b8 cmaterial_unload(Handle32 material_handle) {
  MWARN_CORE("cmaterial_unload() memory leak!!!");
  return TRUE;
}


CMesh* entity_get_mesh(Entity* p_entity) {
  return &scene_data.meshes[entity_get_mesh_handle(p_entity)];
}

AMesh* mesh_get_asset(CMesh* p_mesh) {
  return &scene_data.mesh_assets[p_mesh->mesh_asset_handle];
}


CMaterial* entity_get_material(Entity* p_entity) {
  return &scene_data.materials[entity_get_material_handle(p_entity)];
}

CCTexture* material_get_texture(CMaterial* p_material) {
  return &scene_data.textures[material_get_texture_handle(p_material)];
}

ATexture* texture_get_asset(CCTexture* p_texture) {
  return &scene_data.texture_assets[p_texture->texture_asset_handle];
}

CCamera* entity_get_camera(Entity* p_entity) {
  return &scene_data.cameras[entity_get_camera_handle(p_entity)];
}
