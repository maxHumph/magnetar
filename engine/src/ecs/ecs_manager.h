/**
 * @file ecs_manager.h
 */

#pragma once

#include "ecs/ecs_types.h"

MGAPI b8 scene_load(Scene* scene);

MGAPI b8 scene_unload(Scene* scene);

static b8 entity_load(Entity* entity);

static b8 entity_unload(Entity* entity);

static b8 cmesh_load(CMesh* mesh);

static b8 cmesh_unload(CMesh* mesh);

static b8 cmaterial_load(CMaterial* material);

static b8 cmaterial_unload(CMaterial* material);
