/**
 * @file transform.h
 */

#pragma once

#include "maths/vector.h"

/**
 * @struct Transform
 * @brief Contain postion scale and orientation. 
 */
typedef struct Transform {
  Vec3 position;
  Quat rotation;
  Vec3 scale;
} Transform;
