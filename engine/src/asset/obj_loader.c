#include "obj_loader.h"

#include "data_structures/darray.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static u64 curr_line_len = 0;
static char curr_line[100];
static ObjLineType curr_line_type = ZERO;

static u32 obj_vertex_count = 0;
static u32 obj_index_count = 0;

b8 obj_load(const char* path, Vertex** vertices, u32* vertex_count, u32** indices, u32* index_count) {

  FILE* file = fopen(path, "r");
  *vertices = darray_create(Vertex);
  *indices = darray_create(u32);

  if (file == NULL_PTR) {
    MERROR_CORE("Failed to open .obj file: %s", path);
    return FALSE;
  }

  fseek(file, 0, SEEK_SET);

  while (obj_get_line(curr_line, &curr_line_len, file, &curr_line_type)) {
    switch(curr_line_type) {

    case OBJ_BLOAT:
      break;

    case OBJ_VERTEX:
      obj_load_vertex(vertices);
      break;

    case OBJ_INDEX:
      obj_load_index(indices);
      break;

    default:
      break;
    }
  }
  fclose(file);
  *vertex_count = obj_vertex_count;
  *index_count = obj_index_count;
  obj_vertex_count = 0;
  obj_index_count = 0;
  return TRUE;

}
static b8 obj_load_vertex(Vertex** vertices){
  obj_vertex_count++;
  char val[10];
  u32 c = 0;
  u32 i = 0;
  b8 in_val = FALSE;
  u32 axis = 0;
  Vertex vertex = {};
  while (c < curr_line_len) {
    if (!in_val && isdigit(curr_line[c]) || !in_val && (curr_line[c] == '-')) {
      val[i] = curr_line[c];
      i++;
      in_val = TRUE;
    } else if (curr_line[c] == '.' || isdigit(curr_line[c])) {
      val[i] = curr_line[c];
      i++;
    } else if ((in_val && !isdigit(curr_line[c])) && (in_val && curr_line[c] != '.') && (in_val && curr_line[c] != '-')
	       && (i < curr_line_len)) {
      in_val = FALSE;
      val[i] = '\0';
      vertex.position.index[axis] = (f32)atof(val);
      axis++;
      val[0] = '\0';
      i = 0;
    } else {
      in_val = FALSE;
      i = 0;
    }
    c++;
  }
  darray_push(*vertices, vertex);
  return TRUE;
}

static b8 obj_load_index(u32** indices) {
  obj_index_count += 3;
  u32 c = 0;
  u32 i = 0;

  u32 prop = 0;
  b8 in_val = FALSE;
  char val[8];
  while (c < curr_line_len) {
    if (!in_val && isdigit(curr_line[c])) {
      i = 0;
      in_val = TRUE;
      val[i] = curr_line[c];
      i++;
    } else if (in_val && isdigit(curr_line[c])) {
      val[i] = curr_line[c];
      i++;
    } else if (curr_line[c] == '/') {
      val[i] = '\0';
      if (prop == 0) {
	darray_push(*indices, (u32)atoi(val) - 1);
      }
      val[0] = '\0';
      i = 0;
      prop++;
    } else if (in_val && !isdigit(curr_line[c])) {
      in_val = FALSE;
      val[i] = '\0';
      if (prop == 0) {
	darray_push(*indices, (u32)atoi(val) - 1);
      }
      val[0] = '\0';
      i = 0;
      prop = 0;
    }
    c++;
  }

  return TRUE;
}

static b8 obj_get_line(char* line, u64* line_len, FILE* file, ObjLineType* type) {
  static size_t len = 100;
  if ((*line_len = getline(&line, &len, file)) != -1) {
    if (line[0] == 'v' && line[1] == ' ') {
      *type = OBJ_VERTEX;
      return TRUE;
    } else if (line[0] == 'f' && line[1] == ' ') {
      *type = OBJ_INDEX;
      return TRUE;
    } else {
      *type = OBJ_BLOAT;
      return TRUE;
    }
  }
  return FALSE;
}

