/**
 * @file renderer_types.h
 */

#pragma once

#include "define.h"
#include "maths/vector.h"
#include "maths/matrix.h"

typedef enum VertexType {
  VERTEX_TYPE_MESH,
  VERTEX_TYPE_UI,

  MAX_VERTEX_TYPE,
} VertexType;

/**
 * @brief Stores vertex data for use in vertex buffers.
 */
typedef struct Vertex {
  Vec3 position;
  Vec3 colour;
  Vec2 texture_coord;

} Vertex;

typedef struct UiVertex {
  Vec2 pos;
  Vec2 uv;
  Vec3 color;
} UiVertex;

typedef struct MVPMat {
  Mat4 model;
  Mat4 view;
  Mat4 proj;
} MVPMat;
