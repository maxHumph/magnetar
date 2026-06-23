/**
 * @file scene_text_loader.h
 */

#pragma once

#include "define.h"

#include "ecs_types.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief Parses and loads data from a scene definition.
 * @param data, A pointer to the scene data.
 * @param file, A handle to the scene text file.
 * @return TRUE if parsing was successful, otherwise FALSE.
 */
b8 parse_scene(SceneData* data, FILE* file);

/**
 * @brief Parses and loads data from an entity defenition.
 * @param handle, The handle of the entity.
 * @param name, The name of the entity.
 * @return TRUE if parsing was successful, otherwise FALSE.
 */
static b8 parse_entity(Handle32 handle, char* name);

/**
 * @brief Parses and loads data from a mesh definition.
 * @param handle, The handle of the mesh.
 * @param name, The name of the mesh.
 * @return TRUE if parsing was successful, otherwise FALSE.
 */
static b8 parse_mesh(Handle32 handle, char* name);

/**
 * @brief Parses and loads data from a material definition.
 * @param handle, The handle of the material.
 * @param name, The name of the material.
 * @return TRUE if parsing was successful, otherwise FALSE.
 */
static b8 parse_material(Handle32 handle, char* name);

/**
 * @brief Parses and loads data from a texture definition.
 * @param handle, The handle of the texture.
 * @param name, The name of the texture.
 * @return TRUE if the parsing was successful, otherwise FALSE.
 */
static b8 parse_texture(Handle32 handle, char* name);

static b8 parse_camera(Handle32 handle, char* name);

/**
 * @brief Fills the drawable_handles buffer in scene_data with handles
 * of the entities which can be drawn by the renderer.
 * @return TRUE if no errors were encountered, otherwise FALSE.
 */
static b8 get_drawable_handles();

/**
 * @brief Fills the static tokens array with space separated tokens from
 * the currently loaded line.
 * @return TRUE (means nothing).
 */
static b8 get_tokens();
