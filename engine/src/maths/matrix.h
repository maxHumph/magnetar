/**
 * @file matrix.h
 */

#pragma once

#include "define.h"

#define MAT2F32_IDEN {.aa = 1.0f, .ab = 0.0f, .ba = 0.0f, .bb = 1.0f}

typedef struct Mat2i32 {
  i32 aa, ab;
  i32 ba, bb;
} Mat2i32;

typedef struct Mat2f32 {
  f32 aa, ab;
  f32 ba, bb;
} Mat2f32;

Mat2f32 mat2_f32_add(Mat2f32 mat_1, Mat2f32 mat_2) {
  Mat2f32 new_mat;
  new_mat.aa = mat_1.aa + mat_2.aa;
  new_mat.ab = mat_1.ab + mat_2.ab;
  new_mat.ba = mat_1.ba + mat_2.ba;
  new_mat.bb = mat_1.bb + mat_2.bb;
  return new_mat;
}

typedef struct Mat3i32 {
  i32 aa, ab, ac;
  i32 ba, bb, bc;
  i32 ca, cb, cc;
} Mat3i32;

typedef struct Mat3f32 {
  f32 aa, ab, ac;
  f32 ba, bb, bc;
  f32 ca, cb, cc;
} Mat3f32;

typedef struct Mat4i32 {
  i32 aa, ab, ac, ad;
  i32 ba, bb, bc, bd;
  i32 ca, cb, cc, cd;
  i32 da, db, dc, dd;
} Mat4i32;

typedef struct Mat4f32 {
  f32 aa, ab, ac, ad;
  f32 ba, bb, bc, bd;
  f32 ca, cb, cc, cd;
  f32 da, db, dc, dd;
} Mat4f32;
