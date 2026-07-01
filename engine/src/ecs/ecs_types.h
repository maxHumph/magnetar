/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file ecs_types.h
 * @brief Contains types for the "entity component system"
 */

#pragma once

#include "maths/vector.h"
#include "maths/transform.h"
#include "renderer/renderer_types.h"

typedef Mask64 ComponentTypeMask;
typedef Bit64 ComponentTypeBit;


static const ComponentTypeMask COMPONENT_TYPE_NONE     = 0x00000000ULL;
static const ComponentTypeBit COMPONENT_TYPE_MESH      = 0x00000001ULL;
static const ComponentTypeBit COMPONENT_TYPE_MATERIAL  = 0x00000002ULL;
static const ComponentTypeBit COMPONENT_TYPE_CODE      = 0x00000004ULL;
static const ComponentTypeBit COMPONENT_TYPE_CAMERA    = 0x00000008ULL;

typedef enum ComponentIndex {
  COMPONENT_INDEX_MESH,
  COMPONENT_INDEX_MATERIAL,
  COMPONENT_INDEX_CODE,
  COMPONENT_INDEX_CAMERA,

  MAX_COMPONENT_INDEX,
} ComponentIndex;

typedef struct Entity Entity;
typedef struct EComponent EComponent;

/**
 * @struct Scene
 * @brief Contains handles to all of the entities in the scene
 */
typedef struct Scene {

  const char* name;

  u32 entity_count;
  Handle32* entity_handles;

} Scene;


/**
 * @struct Entity  
 * @brief Contains the name, transform, and component handles of the
 * entity.
 */
typedef struct Entity {
  const char* name;
  Transform transform;

  Handle32 components[MAX_COMPONENT_INDEX];
} Entity;

// ================
// COMPONENTS
// ================

/**
 * Unimplemented
 */
typedef struct CCode {
  const char* name;
  Entity* entity;
} CCode;


/**
 * @struct CMesh
 * @brief Contains handles to mesh assets
 */
typedef struct CMesh {
  const char* name;
  Handle32 mesh_asset_handle;
} CMesh;

/**
 * @struct CMaterial
 * @brief Contains material data for a CMesh.
 *
 * @see CMesh
 */
typedef struct CMaterial {
  const char* name;
  Handle32 texture_handle;
} CMaterial;

/**
 * @struct CCTexture
 * @brief Contains handles to texture assets.
 */
typedef struct CCTexture {
  const char* name;
  Handle32 texture_asset_handle;
} CCTexture;

typedef struct CCamera {
  const char* name;

  f32 fov;
  f32 near_plane;
  f32 far_plane;
} CCamera;

// ================
// ASSETS
// ================

typedef struct AMesh {
  const char* name;

  const char* model_path;

  u32 vertex_count;
  Vertex* vertices;

  u32 index_count;
  u32* indices;
} AMesh;

typedef struct ATexture {
  const char* name;

  const char* image_path;

  u32 width;
  u32 height;
  u32 channels;

  u8* texture_data;
} ATexture;

/**
 * @struct SceneData
 * @brief Contains buffers for scene data (all entities and components)
 * to be loaded into.
 */
typedef struct SceneData {
  Entity* entities;

  // IMPORTANT HANDLES
  Handle32* drawable_handles;
  Handle32 active_camera_entity;

  // COMPONENT ARRAYS
  CCode* codes;

  CMesh* meshes;

  CMaterial* materials;
  CCTexture* textures;

  CCamera* cameras;

  // ASSET ARRAYS
  AMesh* mesh_assets;
  ATexture* texture_assets;

} SceneData;
