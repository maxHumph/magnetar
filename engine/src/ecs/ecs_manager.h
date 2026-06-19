/**
 * @file ecs_manager.h
 */

#pragma once

#include "ecs/ecs_types.h"

MGAPI b8 scene_load(Scene* scene);

MGAPI b8 scene_unload(Scene* scene);

MGAPI b8 scene_load_text(const char* path);

static b8 entity_load(Handle32 entity_handle);

static b8 entity_unload(Handle32 entity_handle);

static b8 cmesh_load(Handle32 mesh_handle);

static b8 cmesh_unload(Handle32 mesh_handle);

static b8 cmaterial_load(Handle32 material_handle);

static b8 cmaterial_unload(Handle32 material_handle);
