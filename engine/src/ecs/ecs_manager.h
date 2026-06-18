/**
 * @file ecs_manager.h
 */

#pragma once

#include "ecs/ecs_types.h"

MGAPI b8 scene_load(Scene* scene);

MGAPI b8 scene_unload(Scene* scene);

static b8 entity_load(Entity* entity);

static b8 entity_unload(Entity* entity);
