/**
 * @file vector.h
 */

#pragma once

#include "define.h"
#include "maths/maths.h"
#include "core/log.h"

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

MGINLINE Vec2 vec2_normalized(Vec2 vec) {
  const f32 len = vec2_mag(vec);
  return (Vec2){vec.x / len, vec.y / len};
}

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

MGINLINE Vec3 vec3_create(f32 x, f32 y, f32 z) { return (Vec3){x, y, z}; }

MGINLINE Vec3 vec3_zero() { return (Vec3){0.0f, 0.0f, 0.0f}; }

MGINLINE Vec3 vec3_one() { return (Vec3){1.0f, 1.0f, 1.0f}; }

MGINLINE Vec3 vec3_up() { return (Vec3){0.0f, 1.0f, 0.0f}; }

MGINLINE Vec3 vec3_down() { return (Vec3){0.0f, -1.0f, 0.0f}; }

MGINLINE Vec3 vec3_left() { return (Vec3){-1.0f, 0.0f, 0.0f}; }

MGINLINE Vec3 vec3_right() { return (Vec3){1.0f, 0.0f, 0.0f}; }

MGINLINE Vec3 vec3_forward() { return (Vec3){0.0f, 0.0f, -1.0f}; }

MGINLINE Vec3 vec3_backward() { return (Vec3){0.0f, 0.0f, 1.0f}; }

MGINLINE Vec3 vec3_add(Vec3 vec_1, Vec3 vec_2) {
  return (Vec3){vec_1.x + vec_2.x, vec_1.y + vec_2.y, vec_1.z + vec_2.z};
}

MGINLINE Vec3 vec3_sub(Vec3 vec_1, Vec3 vec_2) {
  return (Vec3){vec_1.x - vec_2.x, vec_1.y - vec_2.y, vec_1.z - vec_2.z};
}

MGINLINE Vec3 vec3_dot_prod(Vec3 vec_1, Vec3 vec_2) {
  return (Vec3){vec_1.x * vec_2.x, vec_1.y * vec_2.y, vec_1.z * vec_2.z};
}

MGINLINE f32 vec3_mag_squared(Vec3 vec) {
  return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
}

MGINLINE f32 vec3_mag(Vec3 vec) { return msqrt(vec3_mag_squared(vec)); }

MGINLINE Vec3 vec3_normalized(Vec3 vec) {
  const f32 len = vec3_mag(vec);
  return (Vec3){vec.x / len, vec.y / len, vec.z / len};
}

// VEC 4

typedef union Vec4f32 {
  f32 index[4];
  struct {
    union {
      f32 x, r, e1;
    };
    union {
      f32 y, g, i, e2;
    };
    union {
      f32 z, b, j, e3;
    };
    union {
      f32 w, a, k, e4;
    };
  };
} Vec4f32;

typedef Vec4f32 Vec4;


MGINLINE Vec4 vec4_create(f32 x, f32 y, f32 z, f32 w) { return (Vec4){x, y, z, w}; }

MGINLINE Vec4 vec4_zero() { return (Vec4){0.0f, 0.0f, 0.0f, 0.0f}; }

MGINLINE Vec4 vec4_one() { return (Vec4){1.0f, 1.0f, 1.0f, 1.0f}; }

MGINLINE Vec4 vec4_up() { return (Vec4){0.0f, 1.0f, 0.0f, 0.0f}; }

MGINLINE Vec4 vec4_down() { return (Vec4){0.0f, -1.0f, 0.0f, 0.0f}; }

MGINLINE Vec4 vec4_left() { return (Vec4){-1.0f, 0.0f, 0.0f, 0.0f}; }

MGINLINE Vec4 vec4_right() { return (Vec4){1.0f, 0.0f, 0.0f, 0.0f}; }

MGINLINE Vec4 vec4_forward() { return (Vec4){0.0f, 0.0f, -1.0f, 0.0f}; }

MGINLINE Vec4 vec4_backward() { return (Vec4){0.0f, 0.0f, 1.0f, 0.0f}; }

MGINLINE Vec4 vec4_add(Vec4 vec_1, Vec4 vec_2) {
  return (Vec4){vec_1.x + vec_2.x, vec_1.y + vec_2.y, vec_1.z + vec_2.z, vec_1.w + vec_2.w};
}

MGINLINE Vec4 vec4_sub(Vec4 vec_1, Vec4 vec_2) {
  return (Vec4){vec_1.x - vec_2.x, vec_1.y - vec_2.y, vec_1.z - vec_2.z, vec_1.w - vec_2.w};
}

MGINLINE Vec4 vec4_dot_prod(Vec4 vec_1, Vec4 vec_2) {
  return (Vec4){vec_1.x * vec_2.x, vec_1.y * vec_2.y, vec_1.z * vec_2.z, vec_1.w * vec_2.w};
}

MGINLINE f32 vec4_mag_squared(Vec4 vec) {
  return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w);
}

MGINLINE f32 vec4_mag(Vec4 vec) { return msqrt(vec4_mag_squared(vec)); }

MGINLINE Vec4 vec4_normalized(Vec4 vec) {
  const f32 len = vec4_mag(vec);
  return (Vec4){vec.x / len, vec.y / len, vec.z / len, vec.w / len};
}

typedef Vec4f32 Quat;

MGINLINE Quat quat_create(f32 x, f32 i, f32 j, f32 k) { return (Quat){x, i, j, k}; }

MGINLINE Quat quat_from_euler(Vec3 euler) {
  f32 cx = mcos(euler.x / 2.0f);
  f32 cy = mcos(euler.y / 2.0f);
  f32 cz = mcos(euler.z / 2.0f);
  f32 sx = msin(euler.x / 2.0f);
  f32 sy = msin(euler.y / 2.0f);
  f32 sz = msin(euler.z / 2.0f);

  return (Quat){
    (cx * cy * cz) + (sx * sy * sz),
    (sx * cy * cz) - (cx * sy * sy),
    (cx * sy * cz) + (sx * cy * sz),
    (cx * cy * sz) - (sx * sy * cz),
  };
}
 
