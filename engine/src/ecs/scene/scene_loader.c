#include "scene_loader.h"

#include "renderer/renderer_frontend.h"
#include "scene_parser.h"
#include "data/darray.h"
#include "asset/mg3d.h"
#include "asset/texture.h"
#include "ecs/ecs_manager.h"

MGAPI b8 scene_load(StringView name) {

  // @TODO: Remove text loading from this function when scene_compiler.h has been implemented.
  SceneData* scene_data = get_scene_data_ptr();
  SceneIdentifiers* ids = create_scene_identifiers();
  if (!scene_open_from_text(scene_data, &ids,
			    "../engine/src/ecs/scene/sample.scene")) {
    MERROR_CORE("Failed to open scene text file: %s", name);
    return FALSE;
  }



  if (!scene_load_assets(scene_data)) {
    MERROR_CORE("Failed to load assets");
    return FALSE;
  }
  scene_data->scene_loaded = TRUE;

  if (!renderer_load_scene()) {
    MERROR_CORE("Renderer failed to load scene: %s", name);
    return FALSE;
  }
  
  return TRUE;
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
