/**
 * @file vector.h
 */

#pragma once

#include "define.h"

typedef struct Vec2i32 {
  i32 x;
  i32 y;
} Vec2i;

typedef struct Vec2f32 {
  f32 x;
  f32 y;
} Vec2f32;

typedef struct Vec3i32 {
  i32 x;
  i32 y;
  i32 z;
} Vec3i32;

typedef struct Vec3f32 {
  f32 x;
  f32 y;
  f32 z;
} Vec3f32;

typedef struct Vec4i32 {
  i32 x;
  i32 y;
  i32 z;
  i32 a;
} Vec4i32;

typedef struct Vec4f32 {
  f32 x;
  f32 y;
  f32 z;
  f32 a;
} Vec4f32;
