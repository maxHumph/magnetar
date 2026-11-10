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
 * @brief Loads a .scenebin file into SceneData.
 *
 * @param name, The name of the scene.
 * @return TRUE if loading was successdul, otherwise FALSE.
 */
MGAPI b8 scene_load(StringView name);

/* Unimplemented */
// MGAPI b8 scene_unload(StringView name);

/**
 * @brief Removes and deallocates any data found in a SceneData object.
 *
 * @param data, A pointer to the SceneData.
 * @return TRUE if cleaning was successful, otherwise FALSE.
 */
b8 scene_data_clean(SceneData* data);

/**
 * @brief Loads entitys and components from a .scenebin file into SceneData
 *
 * @param data, A pointer to the SceneData.
 * @param name, The path of the .scenebin file.
 */
b8 scene_load_binary(SceneData* data, StringView path);

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

/**
 * @brief Fills a char buffer with bytes read from a .scenebin file.
 *
 * @param path, The absolute path to the file.
 * @param data, A pointer to an unallocated char buffer.
 * @param size, A pointer to an int for the size of the buffer to be stored in.
 * @return TRUE if the data was read into the buffer, otherwise FALSE.
 */
static b8 scene_loader_get_file_data(StringView path, char** data, u64* size);
