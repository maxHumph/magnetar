/**
 * @file matrix.h
 */

#pragma once

#include "define.h"
#include "maths/vector.h"
#include "maths/transform.h"
#include "core/log.h"

// MAT 2

typedef union Mat2f32 {
  f32 index[4];
  struct {
    Vec2f32 r1;
    Vec2f32 r2;
  };
  struct {
    f32 e11, e12;
    f32 e21, e22;
  };
} Mat2f32;

typedef Mat2f32 Mat2;

MGINLINE Mat2 mat2_iden() {
  return (Mat2){1.0f, 0.0f,   // r1
                0.0f, 1.0f};  // r2
}

MGINLINE Mat2 mat2_add(Mat2 mat_1, Mat2 mat_2) {
  return (Mat2){mat_1.e11 + mat_2.e11, mat_1.e12 + mat_2.e12, mat_1.e21 + mat_2.e21,
                mat_1.e22 + mat_2.e22};
}

MGINLINE Mat2 mat2_sub(Mat2 mat_1, Mat2 mat_2) {
  return (Mat2){mat_1.e11 - mat_2.e11, mat_1.e12 - mat_2.e12, mat_1.e21 - mat_2.e21,
                mat_1.e22 - mat_2.e22};
}

// MAT 3

typedef union Mat3f32 {
  f32 index[9];
  struct {
    Vec3f32 r1;
    Vec3f32 r2;
    Vec3f32 r3;
  };
  struct {
    f32 e11, e12, e13;
    f32 e21, e22, e23;
    f32 e31, e32, e33;
  };
} Mat3f32;

typedef Mat3f32 Mat3;

MGINLINE Mat3 mat3_iden() {
  return (Mat3){1.0f, 0.0f, 0.0f,   // r1
                0.0f, 1.0f, 0.0f,   // r2
                0.0f, 0.0f, 1.0f};  // r3
}

MGINLINE Mat3 mat3_add(Mat3 mat_1, Mat3 mat_2) {
  return (Mat3){
      mat_1.e11 + mat_2.e11, mat_1.e12 + mat_2.e12,
      mat_1.e13 + mat_2.e13,  // r1
      mat_1.e21 + mat_2.e21, mat_1.e22 + mat_2.e22,
      mat_1.e23 + mat_2.e23,  // r2
      mat_1.e31 + mat_2.e31, mat_1.e32 + mat_2.e32,
      mat_1.e33 + mat_2.e33  // r3
  };
}

MGINLINE Mat3 mat3_sub(Mat3 mat_1, Mat3 mat_2) {
  return (Mat3){
      mat_1.e11 - mat_2.e11, mat_1.e12 - mat_2.e12,
      mat_1.e13 - mat_2.e13,  // r1
      mat_1.e21 - mat_2.e21, mat_1.e22 - mat_2.e22,
      mat_1.e23 - mat_2.e23,  // r2
      mat_1.e31 - mat_2.e31, mat_1.e32 - mat_2.e32,
      mat_1.e33 - mat_2.e33  // r3
  };
}

// MAT 4

typedef union Mat4f32 {
  f32 index[16];
  struct {
    f32 e11, e12, e13, e14;
    f32 e21, e22, e23, e24;
    f32 e31, e32, e33, e34;
    f32 e41, e42, e43, e44;
  };
} Mat4f32;

typedef Mat4f32 Mat4;


MGINLINE Mat4 mat4_iden() {
  return (Mat4){
      1.0f, 0.0f, 0.0f, 0.0f,  // r1
      0.0f, 1.0f, 0.0f, 0.0f,  // r2
      0.0f, 0.0f, 1.0f, 0.0f,  // r3
      0.0f, 0.0f, 0.0f, 1.0f,  // r4
  };
}

MGINLINE Mat4 mat4_add(Mat4 mat_1, Mat4 mat_2) {
  return (Mat4){
      mat_1.e11 + mat_2.e11, mat_1.e12 + mat_2.e12, mat_1.e13 + mat_2.e13,
      mat_1.e14 + mat_2.e14,  // r1
      mat_1.e21 + mat_2.e21, mat_1.e22 + mat_2.e22, mat_1.e23 + mat_2.e23,
      mat_1.e24 + mat_2.e24,  // r2
      mat_1.e31 + mat_2.e31, mat_1.e32 + mat_2.e32, mat_1.e33 + mat_2.e33,
      mat_1.e34 + mat_2.e34,  // r3
      mat_1.e41 + mat_2.e41, mat_1.e42 + mat_2.e42, mat_1.e43 + mat_2.e43,
      mat_1.e44 + mat_2.e44,  // r4
  };
}

MGINLINE Mat4 mat4_mul(Mat4 mat_1, Mat4 mat_2) {
  return (Mat4) {
    (mat_1.e11 * mat_2.e11) + (mat_1.e12 * mat_2.e21) + (mat_1.e13 * mat_2.e31) + (mat_1.e14 * mat_2.e41),
    (mat_1.e11 * mat_2.e12) + (mat_1.e12 * mat_2.e22) + (mat_1.e13 * mat_2.e32) + (mat_1.e14 * mat_2.e42),
    (mat_1.e11 * mat_2.e13) + (mat_1.e12 * mat_2.e23) + (mat_1.e13 * mat_2.e33) + (mat_1.e14 * mat_2.e43),
    (mat_1.e11 * mat_2.e14) + (mat_1.e12 * mat_2.e24) + (mat_1.e13 * mat_2.e34) + (mat_1.e14 * mat_2.e44),

    (mat_1.e21 * mat_2.e11) + (mat_1.e22 * mat_2.e21) + (mat_1.e23 * mat_2.e31) + (mat_1.e24 * mat_2.e41),
    (mat_1.e21 * mat_2.e12) + (mat_1.e22 * mat_2.e22) + (mat_1.e23 * mat_2.e32) + (mat_1.e24 * mat_2.e42),
    (mat_1.e21 * mat_2.e13) + (mat_1.e22 * mat_2.e23) + (mat_1.e23 * mat_2.e33) + (mat_1.e24 * mat_2.e43),
    (mat_1.e21 * mat_2.e14) + (mat_1.e22 * mat_2.e24) + (mat_1.e23 * mat_2.e34) + (mat_1.e24 * mat_2.e44),

    (mat_1.e31 * mat_2.e11) + (mat_1.e32 * mat_2.e21) + (mat_1.e33 * mat_2.e31) + (mat_1.e34 * mat_2.e41),
    (mat_1.e31 * mat_2.e12) + (mat_1.e32 * mat_2.e22) + (mat_1.e33 * mat_2.e32) + (mat_1.e34 * mat_2.e42),
    (mat_1.e31 * mat_2.e13) + (mat_1.e32 * mat_2.e23) + (mat_1.e33 * mat_2.e33) + (mat_1.e34 * mat_2.e43),
    (mat_1.e31 * mat_2.e14) + (mat_1.e32 * mat_2.e24) + (mat_1.e33 * mat_2.e34) + (mat_1.e34 * mat_2.e44),

    (mat_1.e41 * mat_2.e11) + (mat_1.e42 * mat_2.e21) + (mat_1.e43 * mat_2.e31) + (mat_1.e44 * mat_2.e41),
    (mat_1.e41 * mat_2.e12) + (mat_1.e42 * mat_2.e22) + (mat_1.e43 * mat_2.e32) + (mat_1.e44 * mat_2.e42),
    (mat_1.e41 * mat_2.e13) + (mat_1.e42 * mat_2.e23) + (mat_1.e43 * mat_2.e33) + (mat_1.e44 * mat_2.e43),
    (mat_1.e41 * mat_2.e14) + (mat_1.e42 * mat_2.e24) + (mat_1.e43 * mat_2.e34) + (mat_1.e44 * mat_2.e44),
  };
}

MGINLINE Mat4 mat4_transpose(Mat4 m)
{
    Mat4 r;

    r.e11 = m.e11; r.e12 = m.e21; r.e13 = m.e31; r.e14 = m.e41;
    r.e21 = m.e12; r.e22 = m.e22; r.e23 = m.e32; r.e24 = m.e42;
    r.e31 = m.e13; r.e32 = m.e23; r.e33 = m.e33; r.e34 = m.e43;
    r.e41 = m.e14; r.e42 = m.e24; r.e43 = m.e34; r.e44 = m.e44;

    return r;
}

MGINLINE Vec4 mat4_transform(Mat4 mat, Vec4 vec) {
  return (Vec4) {
    (mat.e11 * vec.e1) + (mat.e12 * vec.e2) + (mat.e13 * vec.e3) + (mat.e14 * vec.e4),
    (mat.e21 * vec.e1) + (mat.e22 * vec.e2) + (mat.e23 * vec.e3) + (mat.e24 * vec.e4),
    (mat.e31 * vec.e1) + (mat.e32 * vec.e2) + (mat.e33 * vec.e3) + (mat.e34 * vec.e4),
    (mat.e41 * vec.e1) + (mat.e42 * vec.e2) + (mat.e43 * vec.e3) + (mat.e44 * vec.e4),
  };
}

MGINLINE f32 mat4_determinant(Mat4f32 m) {
  return
    m.e14 * m.e23 * m.e32 * m.e41 -
    m.e13 * m.e24 * m.e32 * m.e41 -
    m.e14 * m.e22 * m.e33 * m.e41 +
    m.e12 * m.e24 * m.e33 * m.e41 +
    m.e13 * m.e22 * m.e34 * m.e41 -
    m.e12 * m.e23 * m.e34 * m.e41 -
    
    m.e14 * m.e23 * m.e31 * m.e42 +
    m.e13 * m.e24 * m.e31 * m.e42 +
    m.e14 * m.e21 * m.e33 * m.e42 -
    m.e11 * m.e24 * m.e33 * m.e42 -
    m.e13 * m.e21 * m.e34 * m.e42 +
    m.e11 * m.e23 * m.e34 * m.e42 +
    
    m.e14 * m.e22 * m.e31 * m.e43 -
    m.e12 * m.e24 * m.e31 * m.e43 -
    m.e14 * m.e21 * m.e32 * m.e43 +
    m.e11 * m.e24 * m.e32 * m.e43 +
    m.e12 * m.e21 * m.e34 * m.e43 -
    m.e11 * m.e22 * m.e34 * m.e43 -
    
    m.e13 * m.e22 * m.e31 * m.e44 +
    m.e12 * m.e23 * m.e31 * m.e44 +
    m.e13 * m.e21 * m.e32 * m.e44 -
    m.e11 * m.e23 * m.e32 * m.e44 -
    m.e12 * m.e21 * m.e33 * m.e44 +
    m.e11 * m.e22 * m.e33 * m.e44;
}

MGINLINE Mat4 mat4_inverse(Mat4 mat) {
  f32 determ = mat4_determinant(mat);
  f32 recip = 1.0f / determ;
  return (Mat4) {
    mat.e11 * recip, mat.e12 * recip, mat.e13 * recip, mat.e14 * recip,
    mat.e21 * recip, mat.e22 * recip, mat.e23 * recip, mat.e24 * recip,
    mat.e31 * recip, mat.e32 * recip, mat.e33 * recip, mat.e34 * recip,
    mat.e41 * recip, mat.e42 * recip, mat.e43 * recip, mat.e44 * recip,
  };
}
MGINLINE Mat4 mat4_inverse_transform(Mat4 m) {
    Mat4 inv;

    inv.e11 = m.e11; inv.e12 = m.e21; inv.e13 = m.e31;
    inv.e21 = m.e12; inv.e22 = m.e22; inv.e23 = m.e32;
    inv.e31 = m.e13; inv.e32 = m.e23; inv.e33 = m.e33;

    inv.e14 = -(inv.e11 * m.e14 + inv.e12 * m.e24 + inv.e13 * m.e34);
    inv.e24 = -(inv.e21 * m.e14 + inv.e22 * m.e24 + inv.e23 * m.e34);
    inv.e34 = -(inv.e31 * m.e14 + inv.e32 * m.e24 + inv.e33 * m.e34);

    inv.e41 = 0.0f;
    inv.e42 = 0.0f;
    inv.e43 = 0.0f;
    inv.e44 = 1.0f;

    return inv;
}

MGINLINE Mat4 mat4_from_quat(Quat quat) {
  return (Mat4) {
    1 - (2 * (quat.y * quat.y)) - (2 * (quat.z * quat.z)),
    (2 * quat.x * quat.y) - (2 * quat.z * quat.w),
    (2 * quat.x * quat.z) + (2 * quat.y * quat.w), 0.0f,

    (2 * quat.x * quat.y) + (2 * quat.z * quat.w),
    1 - (2 * (quat.x * quat.x)) - (2 * (quat.z * quat.z)),
    (2 * quat.y * quat.z) - (2 * quat.x * quat.w), 0.0f,

    (2 * quat.x * quat.z) - (2 * quat.y * quat.w),
    (2 * quat.y * quat.z) + (2 * quat.x * quat.w),
    1 - (2 * (quat.x * quat.x)) - (2 * (quat.y * quat.y)), 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f,
  };
}

// MGINLINE Mat4 mat4_view(

MGINLINE Mat4 mat4_perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far) {
  f32 f = 1.0f / mtan(fov * 0.5f);
  
  Mat4 m = mat4_iden();
  
  m.e11 = f / aspect_ratio;
  m.e22 = f;
  m.e33 = far / (near - far);
  m.e34 = -1.0f;
  m.e43 = -(far * near) / (far - near);
  m.e44 = 0.0f;

  return m;
}

// @TODO: Apply scale
MGINLINE Mat4 mat4_from_transform(Transform transform) {
  Mat4 translate = {
    1.0f, 0.0f, 0.0f, transform.position.x, //
    0.0f, 1.0f, 0.0f, transform.position.y, //
    0.0f, 0.0f, 1.0f, transform.position.z, //
    0.0f, 0.0f, 0.0f, 1.0f, //
  };
  /*
  Mat4 translate = {
    1.0f, 0.0f, 0.0f, 0.0f, //
    0.0f, 1.0f, 0.0f, 0.0f, //
    0.0f, 0.0f, 1.0f, 0.0f, //
    transform.position.x, transform.position.y, transform.position.z , 1.0f, //
  };
  */

  return (mat4_mul(mat4_from_quat(transform.rotation),
                   translate));

}
