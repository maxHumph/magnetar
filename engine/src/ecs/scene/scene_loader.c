#include "scene_loader.h"

#include "define.h"
#include "core/mmemory.h"
#include "ecs/scene/scene_compiler.h"
#include "renderer/renderer_frontend.h"
#include "scene_parser.h"
#include "data/darray.h"
#include "asset/mg3d.h"
#include "asset/texture.h"
#include "ecs/ecs_manager.h"

#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

#if MPLATOFRM_UNIX || MPLATFORM_APPLE
#include <unistd.h>
#elif MPLATFORM_WINDOWS
#include <direct.h>
#define getcwd(a, b) _getcwd(a, b)
#else
#error "Unsupported platform"
#endif

MGAPI b8 scene_load(StringView name) {

  /*

  // @TODO: Remove text loading from this function when scene_compiler.h has been implemented.
  SceneData* data = get_scene_data_ptr();
  SceneIdentifiers* ids = create_scene_identifiers();
  if (!scene_open_from_text(data, ids,
			    "../engine/src/ecs/scene/sample.scene")) {
    MERROR_CORE("Failed to open scene text file: %s", name);
    return FALSE;
  }

  */

  String path = alloc_string(FILENAME_MAX);
  i32 path_length = snprintf(path, string_get_length(path), "%s/scene/%s.scenebin",
			     getcwd(NULL, 0), name);
  if (path_length < 0) {
    MERROR_CORE("Failed to format scene path. Returned: %d", path_length);
    return FALSE;
  }

  if (path_length > FILENAME_MAX) {
    MERROR_CORE("Scene path exceeds FILENAME_MAX (%d): %d", FILENAME_MAX, path_length);
    return FALSE;
  }

  SceneData* data = get_scene_data_ptr();

  if (!scene_load_binary(data, path)) {
    MERROR_CORE("Failed to load scene binary");
    return FALSE;
  }

  if (!scene_load_assets(data)) {
    MERROR_CORE("Failed to load assets");
    return FALSE;
  }

  data->scene_loaded = TRUE;

  if (!scene_get_drawable_handles(data)) {
    MERROR_CORE("Failed to get drawable handles.");
    return FALSE;
  }

  /*

  MTRACE("%u", darray_get_length(ids->entities));
  for (u32 i = 0; i < darray_get_length(ids->entities); i++) {
    MTRACE("%s", ids->entities[i]);
  }

  for (u32 i = 0; i < darray_get_length(ids->meshes); i++) {
    MTRACE("%s", ids->meshes[i]);
  }

  for (u32 i = 0; i < darray_get_length(ids->textures); i++) {
    MTRACE("%s", ids->textures[i]);
  }

  MTRACE("%u", data->entities[1].components[COMPONENT_INDEX_MATERIAL]);
  MTRACE("%u", data->materials[0].texture_handle);

  MTRACE("%s", ids->textures[0]);
  */

  if (!renderer_load_scene()) {
    MERROR_CORE("Renderer failed to load scene: %s", name);
    return FALSE;
  }

  return TRUE;
}
/*
b8 scene_unload(StringView) {
  return FALSE;
}
*/

b8 scene_data_clean(SceneData* data) {
  data->scene_loaded = FALSE;

  if (data->entities) darray_destroy(data->entities);
  if (data->drawable_handles) darray_destroy(data->drawable_handles);
  
  data->active_camera_entity = NULL_HANDLE_32;

  if (data->codes) darray_destroy(data->codes);
  if (data->meshes) darray_destroy(data->meshes);
  if (data->materials) darray_destroy(data->materials);
  if (data->textures) darray_destroy(data->textures);
  if (data->cameras) darray_destroy(data->cameras);
  if (data->box_colliders_3d) darray_destroy(data->box_colliders_3d);
  if (data->ball_colliders_3d) darray_destroy(data->ball_colliders_3d);

  if (data->mesh_assets) {
    for (u32 i = 0; i < darray_get_length(data->mesh_assets); i++) {
      if (data->mesh_assets[i].model_path) {
	destroy_string(data->mesh_assets[i].model_path);
      }
    }
    darray_destroy(data->mesh_assets);
  }

  if (data->texture_assets) {
    for (u32 i = 0; i < darray_get_length(data->texture_assets); i++) {
      if (data->texture_assets[i].image_path) {
	destroy_string(data->texture_assets[i].image_path);
      }
    }
    darray_destroy(data->texture_assets);
  }
}

b8 scene_load_binary(SceneData* scene_data, StringView path) {

  char* file_data = NULL_PTR;
  u64 data_size = 0;

  if (!scene_loader_get_file_data(path, &file_data, &data_size)) {
    MERROR_CORE("Failed to get data from: %s", path);
    return FALSE;
  }

  u64 offset = 0;

  scene_bin_header_t header;
  mcopy_memory(&header, file_data + offset, sizeof(scene_bin_header_t));
  offset += sizeof(scene_bin_header_t);

  scene_data->entities = darray_create(Entity);
  
  scene_data->meshes = darray_create(CMesh);
  scene_data->materials = darray_create(CMaterial);
  scene_data->textures = darray_create(CCTexture);
  scene_data->cameras = darray_create(CCamera);
  scene_data->box_colliders_3d = darray_create(CBoxCollider3D);
  scene_data->ball_colliders_3d = darray_create(CBallCollider3D);
  
  scene_data->mesh_assets = darray_create(AMesh);
  scene_data->texture_assets = darray_create(ATexture);



  mfree(file_data, data_size * sizeof(char), MEMORY_TAG_SCENE);
  
  return FALSE;
}

b8 scene_load_assets(SceneData* data) {

  if (!scene_load_mesh_assets(data)) {
    MERROR_CORE("Failed to load mesh assets.");
    return FALSE;
  }

  if (!scene_load_texture_assets(data)) {
    MERROR_CORE("Failed to load texture assets.");
    return FALSE;
  }
  
  return TRUE;
}


b8 scene_load_mesh_assets(SceneData* data) {

  MTRACE_CORE("%u", darray_get_length(data->mesh_assets));
  MTRACE_CORE("%s", data->mesh_assets[0].model_path);

  for (u32 i = 0; i < darray_get_length(data->mesh_assets); i++) {
    AMesh* mesh = &data->mesh_assets[i];
    if (!mg3d_load(mesh->model_path, &mesh->vertices,
		   &mesh->vertex_count, &mesh->indices,
		   &mesh->index_count)) {
      MERROR_CORE("Failed to load mesh file: %s", mesh->model_path);
      return FALSE;
    }
  }

  return TRUE;
}

b8 scene_load_texture_assets(SceneData* data) {

  MTRACE_CORE("%u", darray_get_length(data->texture_assets));
  MTRACE_CORE("%s", data->texture_assets[0].image_path);

  for (u32 i = 0; i < darray_get_length(data->texture_assets); i++) {
    ATexture* texture = &data->texture_assets[i];
    if (!load_texture(texture->image_path, &texture->texture_data,
		      &texture->width, &texture->height,
		      &texture->channels)) {
      MERROR_CORE("Failed to load %s", texture->image_path);
      return FALSE;
    }
  }

  return TRUE;
}

static b8 scene_get_drawable_handles(SceneData* data) {

  for (u32 i = 0; i < darray_get_length(data->entities); i++) {
    if (data->entities[i].components[COMPONENT_INDEX_MESH] !=
	NULL_HANDLE_32) {

      darray_push(data->drawable_handles, i);
    }
  }

  return TRUE;
}

static b8 scene_loader_get_file_data(StringView path, char** data, u64* size) {

  FILE* file;
  file = fopen(path, "rb");
  if (file == NULL) {
    MERROR_CORE("%d: %s", errno, strerror(errno));
    return FALSE;
  }

  fseek(file, 0, SEEK_END);
  *size = ftell(file);
  *data = mallocate(*size * sizeof(char), MEMORY_TAG_SCENE);

  fseek(file, 0, SEEK_SET);
  if (*data) {
    fread(*data, sizeof(char), *size, file);
  } else {
    MERROR_CORE("%d: %s", errno, strerror(errno));
    return FALSE;
  }

  fclose(file);

  return TRUE;
}
