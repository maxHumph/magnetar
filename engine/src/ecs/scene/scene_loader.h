/**
 * @file scene_loader.h
 */

#pragma once

#include "define.h"
#include "data/mg_string.h"
#include "ecs/ecs_types.h"

MGAPI b8 scene_load(StringView name);

b8 scene_load_assets(SceneData* data);

b8 scene_load_mesh_assets(SceneData* data);

b8 scene_load_texture_assets(SceneData* data);

static b8 scene_get_drawable_handles(SceneData* data);
