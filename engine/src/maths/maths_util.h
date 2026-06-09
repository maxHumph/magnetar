/**
 * @file maths_util.h
 *
 * @brief Contains maths utility functions such as printing matricies and vectors.
 */

#pragma once

#include "maths/vector.h"
#include "maths/matrix.h"
#include "maths/maths.h"

#include <string.h>
#include <stdio.h>

MGINLINE char* vec4_get_str(Vec4 vec) {
  char buf[100];
  sprintf(buf, "[ %f %f %f %f ]\n", vec.e1, vec.e2, vec.e3, vec.e4);
  char* out = strdup(buf);
  return out;
}

MGINLINE void vec4_print(Vec4 vec) {
  MTRACE("Vec4: %s", vec4_get_str(vec));
}

MGINLINE char* mat4_get_str(Mat4 mat) {
  char buf[600];
  sprintf(buf,
	  "[ %f %f %f %f ]\n"
	  "[ %f %f %f %f ]\n"
	  "[ %f %f %f %f ]\n"
	  "[ %f %f %f %f ]\n",
	  mat.e11, mat.e12, mat.e13, mat.e14,
	  mat.e21, mat.e22, mat.e23, mat.e24,
	  mat.e31, mat.e32, mat.e33, mat.e34,
	  mat.e41, mat.e42, mat.e43, mat.e44);
  char* out = strdup(buf);
  return out;
}

MGINLINE void mat4_print(Mat4 mat) {
  MTRACE("Mat4:\n%s", mat4_get_str(mat));
}
