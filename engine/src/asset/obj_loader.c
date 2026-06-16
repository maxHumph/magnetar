#include "obj_loader.h"

#include "data_structures/darray.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

b8 load_obj(const char* path, Vertex** vertices, u32* vertex_count) {

  FILE* file = fopen(path, "r");
  u64 curr_line_len = 0;
  char curr_line[100];
  ObjLineType curr_line_type = ZERO;

  u32 obj_vertex_count = 0;
  *vertices = darray_create(Vertex);

  if (file == NULL_PTR) {
    MERROR_CORE("Failed to open .obj file: %s", path);
    return FALSE;
  }

  fseek(file, 0, SEEK_SET);

  while (get_obj_line(curr_line, &curr_line_len, file, &curr_line_type)) {
    switch(curr_line_type) {

    case OBJ_BLOAT:
      break;

    case OBJ_VERTEX:
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
      break;

    default:
      break;
    }
  }
  fclose(file);
  return TRUE;
}

static b8 get_obj_line(char* line, u64* line_len, FILE* file, ObjLineType* type) {
  static size_t len = 100;
  if ((*line_len = getline(&line, &len, file)) != -1) {
    if (line[0] == 'v' && line[1] == ' ') {
      *type = OBJ_VERTEX;
      return TRUE;
    } else {
      *type = OBJ_BLOAT;
      return TRUE;
    }
  }
  return FALSE;
}

