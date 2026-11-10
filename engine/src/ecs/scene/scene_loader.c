#include "scene_loader.h"

#include "define.h"
#include "renderer/renderer_frontend.h"
#include "scene_parser.h"
#include "data/darray.h"
#include "asset/mg3d.h"
#include "asset/texture.h"
#include "ecs/ecs_manager.h"

#include <stdio.h>

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

b8 scene_load_binary(SceneData* data, StringView path) {

  data->entities = darray_create(Entity);
  
  data->meshes = darray_create(CMesh);
  data->materials = darray_create(CMaterial);
  data->textures = darray_create(CCTexture);
  data->cameras = darray_create(CCamera);
  data->box_colliders_3d = darray_create(CBoxCollider3D);
  data->ball_colliders_3d = darray_create(CBallCollider3D);
  
  data->mesh_assets = darray_create(AMesh);
  data->texture_assets = darray_create(ATexture);

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
