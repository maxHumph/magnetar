#include "obj_loader.h"

#include "data_structures/darray.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define OBJ_LINE_BUFFER_LENGTH 100
#define OBJ_LOADER_TOKEN_BUFFER_LENGTH 16

static char* line = NULL_PTR;
static size_t line_buf_len = OBJ_LINE_BUFFER_LENGTH;
static u32 line_len = 0;
static ObjLineType type = OBJ_BLOAT;
static FILE* file = NULL_PTR;
static char buf[OBJ_LOADER_TOKEN_BUFFER_LENGTH] = {};

b8 obj_load(const char* path, Vec3** vpos, Vec2** vtex, u32** ipos, u32** itex) {

  file = fopen(path, "r");
  *vpos = darray_create(Vec3);
  *vtex = darray_create(Vec2);
  *ipos = darray_create(u32);
  *itex = darray_create(u32);

  if (file == NULL_PTR) {
    MERROR_CORE("Failed to open .obj file: %s", path);
    return FALSE;
  }

  fseek(file, 0, SEEK_SET);

  while (obj_get_line(file)) {
    switch(type) {

    case OBJ_BLOAT:
      break;

    case OBJ_VPOS:
      obj_load_vpos(vpos);
      break;

    case OBJ_VTEX:
      obj_load_vtex(vtex);
      break;

    case OBJ_FACE:
      obj_load_face(ipos, itex);
      break;

    default:
      break;
    }
  }
  fclose(file);
  return TRUE;

}

static b8 obj_load_vpos(Vec3** vpos) {
  b8 in_token = FALSE;
  buf[0] = NULL_TERM;
  u32 buf_idx = 0;
  Vec3 pos;
  u32 pos_idx = 0;
  for (u32 i = 0; i < line_len; i++) {
    char chr = line[i];
    if (isdigit(chr) || chr == '-' || chr == '.') {
      if (!in_token) {
	in_token = TRUE;
      }
      buf[buf_idx] = chr;
      buf_idx++;
    } else {
      if (in_token) {
	buf[buf_idx] = NULL_TERM;
	pos.index[pos_idx] = atof(buf);
	pos_idx++;
	buf_idx = 0;
      }
    }
  }
  darray_push(*vpos, pos);
  return TRUE;
}

static b8 obj_load_vtex(Vec2** vtex) {
  b8 in_token = FALSE;
  buf[0] = NULL_TERM;
  u32 buf_idx = 0;
  Vec2 tex;
  u32 tex_idx = 0;
  for (u32 i = 0; i < line_len; i++) {
    char chr = line[i];
    if (isdigit(chr) || chr == '.') {
      if (!in_token) {
	in_token = TRUE;
      }
      buf[buf_idx] = chr;
      buf_idx++;
    } else {
      if (in_token) {
	in_token = FALSE;
	buf[buf_idx] = NULL_TERM;
	tex.index[tex_idx] = atof(buf);
	tex_idx++;
	buf_idx = 0;
      }
    }
  }
  darray_push(*vtex, tex);
  return TRUE;
}

static b8 obj_load_face(u32** ipos, u32** itex) {
  b8 in_token = FALSE;
  buf[0] = NULL_TERM;
  u32 buf_idx = 0;
  u32 face_attr = 0;

  for (u32 i = 0; i < line_len; i++) {
    char chr = line[i];
    if (isdigit(chr)) {
      if (!in_token) {
	in_token = TRUE;
      }
      buf[buf_idx] = chr;
      buf_idx++;
    } else {
      if (in_token) {
	in_token = FALSE;
	buf[buf_idx] = NULL_TERM;
	buf_idx = 0;
	switch (face_attr) {
	case 0:
	  darray_push(*ipos, (u32)atol(buf));
	  break;
	case 1:
	  darray_push(*itex, (u32)atol(buf));
	  break;
	case 2:
	  break;
	default:
	  MERROR_CORE("Invalid face in .obj file: %s", line);
	  return FALSE;
	  break;
	}
	face_attr++;
	if (chr != '/') {
	  face_attr = 0;
	}
      }
    }
  }
  

  return TRUE;
}

static b8 obj_get_line(FILE* file) {
  static size_t len = 100;
  if ((line_len = getline(&line, &line_buf_len, file)) != -1) {
    if (line[0] == 'v' && line[1] == ' ') {
      type = OBJ_VPOS;
      return TRUE;
    } else if (line[0] == 'f' && line[1] == ' ') {
      type = OBJ_FACE;
      return TRUE;
    } else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
      type = OBJ_VTEX;
      return TRUE;
    } else {
      type = OBJ_BLOAT;
      return TRUE;
    }
  }
  return FALSE;
}

