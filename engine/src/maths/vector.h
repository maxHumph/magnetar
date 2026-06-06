/**
 * @file vector.h
 */

#pragma once

#include "define.h"
#include "maths/maths.h"

// VEC 2

typedef union Vec2i16 {
  i16 index[2];
  struct {
    union {
      i16 x, r;
    };
    union {
      i16 y, g;
    };
  };
} Vec2i16;

typedef union Vec2i32 {
  i32 index[2];
  struct {
    union {
      i32 x, r;
    };
    union {
      i32 y, g;
    };
  };

} Vec2i32;

typedef union Vec2f32 {
  f32 index[2];
  struct {
    union {
      f32 x, r;
    };
    union {
      f32 y, g;
    };
  };
} Vec2f32;

typedef Vec2f32 Vec2;

MGINLINE Vec2 vec2_create(f32 x, f32 y) { return (Vec2){x, y}; }

MGINLINE Vec2 vec2_zero() { return (Vec2){0.0f, 0.0f}; }

MGINLINE Vec2 vec2_one() { return (Vec2){1.0f, 1.0f}; }

MGINLINE Vec2 vec2_up() { return (Vec2){0.0f, 1.0f}; }

MGINLINE Vec2 vec2_down() { return (Vec2){0.0f, -1.0f}; }

MGINLINE Vec2 vec2_left() { return (Vec2){-1.0f, 0.0f}; }

MGINLINE Vec2 vec2_right() { return (Vec2){1.0f, 0.0f}; }

MGINLINE Vec2 vec2_add(Vec2 vec_1, Vec2 vec_2) {
  return (Vec2){vec_1.x + vec_2.x, vec_1.y + vec_2.y};
}

MGINLINE Vec2 vec2_sub(Vec2 vec_1, Vec2 vec_2) {
  return (Vec2){vec_1.x - vec_2.x, vec_1.y - vec_2.y};
}

MGINLINE Vec2 vec2_dot_prod(Vec2 vec_1, Vec2 vec_2) {
  return (Vec2){vec_1.x * vec_2.x, vec_1.y * vec_2.y};
}

MGINLINE f32 vec2_mag_squared(Vec2 vec) { return (vec.x * vec.x) + (vec.y * vec.y); }

MGINLINE f32 vec2_mag(Vec2 vec) { return msqrt(vec2_mag_squared(vec)); }

// VEC 3

typedef union Vec3f32 {
  f32 index[3];
  struct {
    union {
      f32 x, r;
    };
    union {
      f32 y, g;
    };
    union {
      f32 z, b;
    };
  };
} Vec3f32;

typedef Vec3f32 Vec3;

// VEC 4

typedef union Vec4f32 {
  f32 index[4];
  struct {
    union {
      f32 x, r;
    };
    union {
      f32 y, g, i;
    };
    union {
      f32 z, b, j;
    };
    union {
      f32 w, a, k;
    };
  };
} Vec4f32;

typedef Vec4f32 Vec4;

typedef Vec4f32 Quat;
