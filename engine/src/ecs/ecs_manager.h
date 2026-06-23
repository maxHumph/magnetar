/**
 * @file ecs_manager.h
 */

#pragma once

#include "ecs/ecs_types.h"

MGAPI b8 scene_load(Scene* scene);

MGAPI b8 scene_unload(Scene* scene);

MGAPI b8 scene_load_text(const char* path);

MGAPI SceneData* get_scene_data_ptr();

MGAPI b8 camera_set_active(Handle32 entity_handle);

static b8 entity_load(Handle32 entity_handle);

static b8 entity_unload(Handle32 entity_handle);

static b8 cmesh_load(Handle32 mesh_handle);

static b8 cmesh_unload(Handle32 mesh_handle);

static b8 cmaterial_load(Handle32 material_handle);

static b8 cmaterial_unload(Handle32 material_handle);

MGINLINE Handle32 entity_get_mesh_handle(Entity* p_entity) {
  return p_entity->components[COMPONENT_INDEX_MESH];
}

MGAPI CMesh* entity_get_mesh(Entity* p_entity);

MGINLINE Handle32 entity_get_material_handle(Entity* p_entity) {
  return p_entity->components[COMPONENT_INDEX_MATERIAL];
}

MGAPI CMaterial* entity_get_material(Entity* p_entity);

MGINLINE Handle32 material_get_texture_handle(CMaterial* p_material) {
  return p_material->texture_handle;
}

MGAPI CCTexture* material_get_texture(CMaterial* p_material);

MGINLINE Handle32 entity_get_camera_handle(Entity* p_entity) {
  return p_entity->components[COMPONENT_INDEX_CAMERA];
}

MGAPI CCamera* entity_get_camera(Entity* p_entity);
