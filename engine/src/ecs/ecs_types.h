/**
 * @file ecs_types.h
 */

#pragma once

#include "maths/vector.h"
#include "renderer/vulkan/vulkan_defines.h"

typedef Mask64 ComponentTypeMask;
typedef Bit64 ComponentTypeBit;

static const ComponentTypeBit COMPONENT_TYPE_NONE      = 0x00000000ULL;
static const ComponentTypeBit COMPONENT_TYPE_TRANSFORM = 0x00000001ULL;
static const ComponentTypeBit COMPONENT_TYPE_MESH      = 0x00000002ULL;
static const ComponentTypeBit COMPONENT_TYPE_MATERIAL  = 0x00000004ULL;
static const ComponentTypeBit COMPONENT_TYPE_CODE      = 0x00000008ULL;

typedef struct Entity Entity;

typedef struct Component {
  ComponentTypeBit type;
  void* component;
} Component;

typedef struct Entity {

  const char* name;

  ComponentTypeMask component_mask;

  Entity* parent;

  u32 child_count;
  Entity* children;

  u32 component_count;
  Component* components;

} Entity;

typedef struct Scene {

  const char* name;

  u32 entity_count;
  Entity** entities;

} Scene;

typedef struct CTransform {
  Entity* entity;

  Vec3 position;
  Quat rotation;
  Vec3 scale;
} CTransform;

typedef struct CMesh {
  Entity* entity;

  const char* model_path;

  u32 vertex_count;
  Vertex* vertices;

  u32 index_count;
  u32* indices;
} CMesh;

typedef struct CMaterial CMaterial;

typedef struct CCTexture {
  CMaterial* material;

  const char* image_path;

  u32 width;
  u32 height;
  u32 channels;

  u8* texture_data;

} CCTexture;

typedef struct CMaterial {
  Entity* entity;

  CCTexture texture;

} CMaterial;
