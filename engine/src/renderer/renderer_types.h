/**
 * @file renderer_types.h
 */

#pragma once

#include "define.h"
#include "maths/vector.h"
#include "maths/matrix.h"

/**
 * @brief Stores vertex data for use i vertex buffers.
 */
typedef struct Vertex {
  Vec3 position;
  Vec3 colour;
  Vec2 texture_coord;

} Vertex;

typedef struct MVPMat {
  Mat4 model;
  Mat4 view;
  Mat4 proj;
} MVPMat;
