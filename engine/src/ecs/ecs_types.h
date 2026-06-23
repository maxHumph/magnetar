/**
 * @file ecs_types.h
 * @brief Contains types for the "entity component system"
 */

#pragma once

#include "maths/vector.h"
#include "maths/transform.h"
#include "renderer/renderer_types.h"

typedef Mask64 ComponentTypeMask;
typedef Bit64 ComponentTypeBit;


static const ComponentTypeBit COMPONENT_TYPE_NONE      = 0x00000000ULL;
static const ComponentTypeBit COMPONENT_TYPE_MESH      = 0x00000001ULL;
static const ComponentTypeBit COMPONENT_TYPE_MATERIAL  = 0x00000002ULL;
static const ComponentTypeBit COMPONENT_TYPE_CODE      = 0x00000004ULL;

typedef enum ComponentIndex {
  COMPONENT_INDEX_MESH,
  COMPONENT_INDEX_MATERIAL,
  COMPONENT_INDEXCODE,

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

/*
typedef struct Entity {

  const char* name;
  Transform transform;

  ComponentTypeMask component_mask;

  u32 component_count;
  EComponent* components;

} Entity;
*/

/**
 * @struct EComponent
 * @brief Contains the type and handle of a component.
 */
typedef struct EComponent {
  ComponentTypeBit type;
  Handle32 handle;
} EComponent;

/**
 * Unimplemented
 */
typedef struct CCode {
  Entity* entity;
} CCode;


/**
 * @struct CMesh
 * @brief Contains vertex and index data of a model.
 */
typedef struct CMesh {
  Entity* entity;
  const char* name;

  const char* model_path;

  u32 vertex_count;
  Vertex* vertices;

  u32 index_count;
  u32* indices;
} CMesh;

/**
 * @struct CMaterial
 * @brief Contains material data for a CMesh.
 *
 * @see CMesh
 */
typedef struct CMaterial {
  Entity* entity;

  const char* name;

  Handle32 texture_handle;

} CMaterial;

/**
 * @struct CCTexture
 * @brief Contains information and pixel data of a texture image.
 */
typedef struct CCTexture {
  CMaterial* material;

  const char* name;

  const char* image_path;

  u32 width;
  u32 height;
  u32 channels;

  u8* texture_data;

} CCTexture;

/**
 * @struct SceneData
 * @brief Contains buffers for scene data (all entities and components)
 * to be loaded into.
 */
typedef struct SceneData {
  Entity* entities;
  Handle32* drawable_handles;

  CCode* codes;
  CMesh* meshes;
  CMaterial* materials;
  CCTexture* textures;
} SceneData;
