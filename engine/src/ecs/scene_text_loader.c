#include "scene_text_loader.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "data_structures/darray.h"
#include "core/mmemory.h"
#include "asset/mg3d.h"
#include "asset/texture.h"

static SceneData* scene_data = NULL_PTR;
static FILE* scene_file = NULL_PTR;

static char* line = NULL_PTR;
static size_t line_len = 0;

static char** tokens = NULL_PTR;


b8 parse_scene(SceneData* data, FILE* file) {
  MTRACE("Parsing scene");
  scene_data = data;
  scene_file = file;

  while ((line_len = getline(&line, &line_len, scene_file)) != -1) {
    get_tokens();
    if (strcmp(tokens[0], "END_SCENE") == 0) {
      break;
    } else if (strcmp(tokens[0], "ENTITY") == 0) {
      parse_entity((Handle32)atol(tokens[1]), tokens[2]);
    } else if (strcmp(tokens[0], "MESH") == 0) {
      parse_mesh((Handle32)atol(tokens[1]), tokens[2]);
    } else if (strcmp(tokens[0], "MATERIAL") == 0) {
      parse_material((Handle32)atol(tokens[1]), tokens[2]);
    } else if (strcmp(tokens[0], "TEXTURE") == 0) {
      parse_texture((Handle32)atol(tokens[1]), tokens[2]);
    }
  }

  if (!get_drawable_handles()) {
    MERROR_CORE("Failed to get handles of drawable entities");
    return FALSE;
  }

  return TRUE;
}

static b8 parse_entity(Handle32 handle, char* name) {
  MTRACE("Parsing entity: %u, %s", handle, name);
  u64 name_len = strlen(name) + 1;
  char* name_copy = mallocate(name_len, MEMORY_TAG_ENTITY);
  mcopy_memory(name_copy, name, name_len);
  Entity new_entity = {
    .name = name_copy,
    .component_mask = NULL_HANDLE_64,
  };
  darray_push(scene_data->entities, new_entity);
  Entity* p_entity = &scene_data->entities[handle];
  u32 component_idx = 0;
  while ((line_len = getline(&line, &line_len, scene_file)) != -1) {
    get_tokens();
    if (strcmp(tokens[0], "TRANSFORM") == 0) {
      MTRACE("Parsing transform");
      while ((line_len = getline(&line, &line_len, scene_file)) != -1) {
	get_tokens();
	if (strcmp(tokens[0], "END_TRANSFORM") == 0) {
	  break;
	} else if (strcmp(tokens[0], "position") == 0) {
	  p_entity->transform.position = (Vec3){
	    .x = atof(tokens[1]),
	    .y = atof(tokens[2]),
	    .z = atof(tokens[3]),
	  };
	} else if (strcmp(tokens[0], "rotation") == 0) {
	  p_entity->transform.rotation = (Quat){
	    .x = atof(tokens[1]),
	    .y = atof(tokens[2]),
	    .z = atof(tokens[3]),
	    .a = atof(tokens[4]),
	  };
	} else if (strcmp(tokens[0], "scale") == 0) {
	  p_entity->transform.scale = (Vec3){
	    .x = atof(tokens[1]),
	    .y = atof(tokens[2]),
	    .z = atof(tokens[3]),
	  };
	}
      }
    } else if (strcmp(tokens[0], "component_count") == 0) {
      p_entity->component_count = (u32)atol(tokens[1]);
      p_entity->components =
	mallocate(p_entity->component_count * sizeof(Component),
		  MEMORY_TAG_ENTITY); // @TODO: Free mem

    } else if (strcmp(tokens[0], "component") == 0){
      // SET COMPONENTS
      if (strcmp(tokens[1], "mesh") == 0) {
	p_entity->component_mask = p_entity->component_mask |
	  COMPONENT_TYPE_MESH;

	p_entity->components[component_idx].type = COMPONENT_TYPE_MESH;

	p_entity->components[component_idx].handle =
	  (u32)atol(tokens[2]);

	component_idx++;
      } else if (strcmp(tokens[1], "material") == 0) {
	p_entity->component_mask = p_entity->component_mask |
	  COMPONENT_TYPE_MATERIAL;

	p_entity->components[component_idx].type =
	  COMPONENT_TYPE_MATERIAL;

	p_entity->components[component_idx].handle =
	  (u32)atol(tokens[2]);

	component_idx++;
      }
      
    } else if (strcmp(tokens[0], "END_ENTITY") == 0) {
      break;
    }
  }

  return TRUE;
}

static b8 parse_mesh(Handle32 handle, char* name) {
  MTRACE("Parsing mesh: %u, %s", handle, name);
  u64 name_len = strlen(name) + 1;
  char* name_copy = mallocate(name_len, MEMORY_TAG_ENTITY);
  mcopy_memory(name_copy, name, name_len);
  CMesh new_mesh = {
    .name = name_copy,
  };
  darray_push(scene_data->meshes, new_mesh);
  CMesh* p_mesh = &scene_data->meshes[handle];

  while ((line_len = getline(&line, &line_len, scene_file)) != -1) {
    get_tokens();
    if (strcmp(tokens[0], "END_MESH") == 0) {
      break;
    } else if (strcmp(tokens[0], "path") == 0) {
      u64 path_len = strlen(tokens[1]) + 1;
      char* path_copy = mallocate(path_len, MEMORY_TAG_COMPONENT);
      mcopy_memory(path_copy, tokens[1], path_len);
      p_mesh->model_path = path_copy;
    }
  }

  if (!mg3d_load(p_mesh->model_path, &p_mesh->vertices,
		 &p_mesh->vertex_count, &p_mesh->indices,
		 &p_mesh->index_count)) {
    MERROR_CORE("Failed to load mesh: %s", p_mesh->model_path);
    return FALSE;
  }

  return TRUE;
}

static b8 parse_material(Handle32 handle, char* name) {
  MTRACE("Parsing material: %u, %s", handle, name);
  u64 name_len = strlen(name) + 1;
  char* name_copy = mallocate(name_len, MEMORY_TAG_COMPONENT);
  mcopy_memory(name_copy, name, name_len);
  CMaterial new_material = {
    .name = name_copy,
  };
  darray_push(scene_data->materials, new_material);
  CMaterial* p_material = &scene_data->materials[handle];

  while ((line_len = getline(&line, &line_len, scene_file)) != -1) {
    get_tokens();
    if (strcmp(tokens[0], "END_MATERIAL")) {
      break;
    } else if (strcmp(tokens[0], "texture")) {
      p_material->texture_handle = (u32)atol(tokens[1]);
    }
  }

  return TRUE;
}

static b8 parse_texture(Handle32 handle, char* name) {
  MTRACE("Parsing texture: %u, %s", handle, name);
  u64 name_len = strlen(name) + 1;
  char* name_copy = mallocate(name_len, MEMORY_TAG_COMPONENT);
  mcopy_memory(name_copy, name, name_len);
  CCTexture new_texture = {
    .name = name_copy,
  };
  darray_push(scene_data->textures, new_texture);
  CCTexture* p_texture = &scene_data->textures[handle];

  while ((line_len = getline(&line, &line_len, scene_file)) != -1) {
    get_tokens();
    if (strcmp(tokens[0], "END_TEXTURE")) {
      break;
    } else if (strcmp(tokens[0], "path")) {
      u64 path_len = strlen(tokens[1]) + 1;
      char* path_copy = mallocate(path_len, MEMORY_TAG_COMPONENT);
      mcopy_memory(path_copy, tokens[1], path_len);
      p_texture->image_path = path_copy;
    }
  }

  if (!load_texture(p_texture->image_path, &p_texture->texture_data,
		    &p_texture->width, &p_texture->height,
		    &p_texture->channels)) {
    MERROR_CORE("Failed to load texture: %s", p_texture->image_path);
    return FALSE;
  }

  return TRUE;
}

static b8 get_drawable_handles() {

  for (u32 i = 0; i < darray_get_length(scene_data->entities); i++) {
    if (scene_data->entities[i].component_mask & COMPONENT_TYPE_MESH) {
      darray_push(scene_data->drawable_handles, i);
    }
  }

  return TRUE;
}

static b8 get_tokens() {

  // Clear previous tokens
  if (tokens != NULL_PTR) {
    for (u32 i = 0; i < darray_get_length(tokens); i++) {
      darray_destroy(tokens[i]);
    }
    darray_destroy(tokens);
    tokens = NULL_PTR;
  }


  tokens = darray_create(char*);
  char* buf = darray_create(char);

  for (u32 i = 0; i < line_len; i++) {
    if (line[i] != ' ' && line[i] != '\n') {
      darray_push(buf, line[i]);
    } else {
      darray_push(buf, NULL_TERM);
      darray_push(tokens, buf);
      buf = darray_create(char);
    }
  }
  /*
  if (line[line_len - 1] != ' ') {
    darray_push(buf, NULL_TERM);
    darray_push(tokens, buf);
  }
  */
  buf = NULL_PTR;
  return TRUE;
}
