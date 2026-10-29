/**
 * @file scene_loader.
 *
 * @brief Contains functions for loading compiled scene data and assets.
 */

#pragma once

#include "define.h"
#include "data/mg_string.h"
#include "ecs/ecs_types.h"

/**
 * Exact implementation undecided.
 */
MGAPI b8 scene_load(StringView name);

/**
 * @brief Calls the function for loading each type of asset data into the
 * scene.
 *
 * @param data, A pointer to the partially loaded SceneData object.
 * @return TRUE if the data was loaded successfully, otherwise FALSE.
 */
b8 scene_load_assets(SceneData* data);

/**
 * @brief Iterates through all the AMesh objects in scene data and loads
 * their corresponding data into them.
 *
 * @param data, A pointer to the scene data.
 * @return TRUE if the meshes were loaded successfully, otherwise FALSE.
 */
b8 scene_load_mesh_assets(SceneData* data);

/**
 * @brief Iterates through all the ATexture objects in scene data and loads
 * their corresponding data into them.
 *
 * @param data, A pointer to the scene data.
 * @return TRUE if tjhe textures were loaded successfully, otherwise FALSE.
 */
b8 scene_load_texture_assets(SceneData* data);

/**
 * @brief Filled the drawable handles darray in SceneData with entities
 * that contain meshes to be drawn.
 *
 * @param data, A pointer to the scene data.
 * @return TRUE (not used atm)
 */
static b8 scene_get_drawable_handles(SceneData* data);
