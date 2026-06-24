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

MGINLINE Vec3 vec3_scale(Vec3 vec, f32 scale) {
  return (Vec3){vec.x * scale, vec.y * scale, vec.z * scale};
}

MGINLINE Vec3 vec3_dot_prod(Vec3 vec_1, Vec3 vec_2) {
  return (Vec3){vec_1.x * vec_2.x, vec_1.y * vec_2.y, vec_1.z * vec_2.z};
}
MGINLINE Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 r;

    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;

    return r;
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

MGINLINE Quat quat_mul(Quat a, Quat b) {
    Quat r;

    r.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
    r.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
    r.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
    r.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;

    f32 len = msqrt(r.x*r.x + r.y*r.y + r.z*r.z + r.w*r.w);
    if (len > 0.0f) {
        f32 inv = 1.0f / len;
        r.x *= inv; r.y *= inv; r.z *= inv; r.w *= inv;
    }

    return r;
}

MGINLINE Quat quat_from_euler(Vec3 euler) {
  f32 cx = mcos(euler.x * 0.5f);
  f32 cy = mcos(euler.y * 0.5f);
  f32 cz = mcos(euler.z * 0.5f);
  f32 sx = msin(euler.x * 0.5f);
  f32 sy = msin(euler.y * 0.5f);
  f32 sz = msin(euler.z * 0.5f);

  return (Quat){
    (sx * cy * cz) - (cx * sy * sz),
    (cx * sy * cz) + (sx * cy * sz),
    (cx * cy * sz) - (sx * sy * cz),
    (cx * cy * cz) + (sx * sy * sz),
  };
}

MGINLINE Vec3 quat_rotate(Quat q, Vec3 v) {
    Vec3 qv = { q.x, q.y, q.z };

    Vec3 t = vec3_scale(vec3_cross(qv, v), 2.0f);
    Vec3 t2 = vec3_cross(qv, t);

    Vec3 result;

    result.x = v.x + q.w * t.x + t2.x;
    result.y = v.y + q.w * t.y + t2.y;
    result.z = v.z + q.w * t.z + t2.z;

    return result;
}
