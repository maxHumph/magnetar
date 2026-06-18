/**
 * @file ecs_types.h
 */

#pragma once

#include "maths/vector.h"

typedef enum ComponentType {
  COMPONENT_TYPE_CODE,
  COMPONENT_TYPE_TRANSFORM,
  COMPONENT_TYPE_MESH,
  COMPONENT_TYPE_MATERIAL,

  MAX_COMPONENT_TYPE,
} ComponentType;

typedef struct Entity Entity;

typedef struct Component {
  ComponentType type;
  Entity* entity;
  void* component;
} Component;

typedef struct Entity {

  Entity* parent;

  u32 child_count;
  Entity* children;

  u32 component_count;
  Component* components;

} Entity;

typedef struct Scene {

  const char* name;

  u32 entity_count;
  Entity* entities;

} Scene;

typedef struct CTransform {
  Component* component;

  Vec3 position;
  Quat rotation;
  Vec3 scale;
} CTransform;

typedef struct CMesh {
  Component* component;

  u32 vertex_count;
  Vertex* vertices;

  u32 index_count;
  u32* indices;
} CMesh;

typedef struct CMaterial CMaterial;

typedef struct CCTexture {
  CMaterial* material;

  u32 width;
  u32 height;
  u32 channels;

  u8* texture_data;

} CCTexture;

typedef struct CMaterial {
  Component* component;

  CCTexture* texture;

} CMaterial;
