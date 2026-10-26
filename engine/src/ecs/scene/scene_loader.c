#include "scene_loader.h"

#include "data/darray.h"
#include "asset/mg3d.h"
#include "asset/texture.h"

MGAPI b8 scene_load(StringView name) {
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

  for (u32 i = 0; i < darray_get_length(data->mesh_assets); i++) {
    AMesh* mesh = &data->mesh_assets[i];
    if (!mg3d_load(mesh->model_path, &mesh->vertices,
		   &mesh->vertex_count, &mesh->indices,
		   &mesh->index_count)) {
      MERROR_CORE("Failed to load %s", mesh->model_path);
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
