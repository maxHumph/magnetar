/**
 * @file ecs_types.h
 */

#pragma once

#include "maths/vector.h"
#include "renderer/renderer_types.h"

typedef Mask64 ComponentTypeMask;
typedef Bit64 ComponentTypeBit;

static const ComponentTypeBit COMPONENT_TYPE_NONE      = 0x00000000ULL;
static const ComponentTypeBit COMPONENT_TYPE_MESH      = 0x00000001ULL;
static const ComponentTypeBit COMPONENT_TYPE_MATERIAL  = 0x00000002ULL;
static const ComponentTypeBit COMPONENT_TYPE_CODE      = 0x00000004ULL;

typedef struct Entity Entity;
typedef struct Component Component;


typedef struct Scene {

  const char* name;

  u32 entity_count;
  Handle32* entity_handles;

} Scene;

typedef struct Transform {
  Vec3 position;
  Quat rotation;
  Vec3 scale;
} Transform;

typedef struct Entity {

  const char* name;
  Transform transform;

  ComponentTypeMask component_mask;

  Entity* parent;

  u32 child_count;
  Entity* children;

  u32 component_count;

  Component* components;

} Entity;

typedef struct Component {
  ComponentTypeBit type;
  Handle32 handle;
} Component;

typedef struct CCode {
  Entity* entity;
} CCode;


typedef struct CMesh {
  Entity* entity;
  const char* name;

  const char* model_path;

  u32 vertex_count;
  Vertex* vertices;

  u32 index_count;
  u32* indices;
} CMesh;

typedef struct CMaterial {
  Entity* entity;

  const char* name;

  Handle32 texture_handle;

} CMaterial;

typedef struct CCTexture {
  CMaterial* material;

  const char* name;

  const char* image_path;

  u32 width;
  u32 height;
  u32 channels;

  u8* texture_data;

} CCTexture;

typedef struct SceneData {
  Entity* entities;

  CCode* codes;
  CMesh* meshes;
  CMaterial* materials;
  CCTexture* textures;
} SceneData;
