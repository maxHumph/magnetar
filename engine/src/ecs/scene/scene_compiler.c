#include "scene_compiler.h"

#include "ecs/ecs_manager.h"
#include "ecs/scene/scene_parser.h"
#include "ecs/scene/scene_loader.h"
#include "data/darray.h"
#include "core/mmemory.h"

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

b8 scene_compile(SceneData* scene_data, StringView name) {
  
  String path = alloc_string(FILENAME_MAX);
  i32 path_length = snprintf(path, string_get_length(path), "%s/scene/%s.scene",
			     getcwd(NULL, 0), name);


  SceneIdentifiers* ids = create_scene_identifiers();
  if (!scene_open_from_text(scene_data, ids, path)) {
    MERROR_CORE("Failed to open: %s", path);
    return FALSE;
  }


  u32 size;
  char* data = NULL_PTR;

  if (!scene_compiler_generate_data(scene_data, &data, &size)) {
    MERROR_CORE("Failed to generate scene data.");
    return FALSE;
  }

  if (!scene_compiler_write_file(path, data, size)) {
    MERROR_CORE("Failed to write %s.scenebin.", path);
    return FALSE;
  }

  mfree(data, size, MEMORY_TAG_SCENE);

  if (!scene_data_clean(scene_data)) {
    MERROR_CORE("Failed to clean scene data.");
    return FALSE;
  }

  if (!scene_unload()) {
    MERROR_CORE("Failed to unload scene.");
    return FALSE;
  }

  return TRUE;
}

b8 scene_compiler_generate_data(SceneData* scene_data, char** data, u32* size) {

  // @TODO_MAYBE: Find a way to make this funciton less verbose.
  
  scene_bin_header_t header = {
    .signature = SCENE_BIN_SIG,
  };

  String mesh_paths = NULL_PTR;
  String texture_paths = NULL_PTR;

  if (!scene_compiler_generate_header(scene_data, &header)) {
    MERROR_CORE("Failed to generate file header.");
    return FALSE;
  }

  if (!scene_compiler_calculate_size(scene_data, size, &header)) {
    MERROR_CORE("Failed to calculate output file size.");
    return FALSE;
  }

  *data = mallocate(*size, MEMORY_TAG_SCENE); // @TODO Free

  if (!scene_compiler_copy_data(scene_data, *data, &header)) {
    MERROR_CORE("Failed to copy data.");
    return FALSE;
  }


  return TRUE;
}

b8 scene_compiler_generate_header(SceneData* scene_data, scene_bin_header_t* header) {

  header->obj_counts[SCENE_OBJ_INDEX_ENTITY] = darray_get_length(scene_data->entities);
  header->obj_counts[SCENE_OBJ_INDEX_CODE] = darray_get_length(scene_data->codes);
  header->obj_counts[SCENE_OBJ_INDEX_MESH] = darray_get_length(scene_data->meshes);
  header->obj_counts[SCENE_OBJ_INDEX_MATERIAL] = darray_get_length(scene_data->materials);
  header->obj_counts[SCENE_OBJ_INDEX_TEXTURE] = darray_get_length(scene_data->textures);
  header->obj_counts[SCENE_OBJ_INDEX_CAMERA] = darray_get_length(scene_data->cameras);
  header->obj_counts[SCENE_OBJ_INDEX_BOX_COLLIDER_3D] =
    darray_get_length(scene_data->box_colliders_3d);
  header->obj_counts[SCENE_OBJ_INDEX_BALL_COLLIDER_3D] =
    darray_get_length(scene_data->ball_colliders_3d);

  header->obj_counts[SCENE_OBJ_INDEX_MESH_ASSET] = darray_get_length(scene_data->mesh_assets);
  header->obj_counts[SCENE_OBJ_INDEX_TEXTURE_ASSET] =
    darray_get_length(scene_data->texture_assets);

  
  return TRUE;
}

b8 scene_compiler_calculate_size(SceneData* scene_data, u32* size, scene_bin_header_t* header) {

  u32 mesh_paths_length = 0;
  for (u32 i = 0; i < darray_get_length(scene_data->mesh_assets); i++) {
    mesh_paths_length += string_get_raw_size(scene_data->mesh_assets[i].model_path);
  }
  
  u32 texture_paths_length = 0;
  for (u32 i = 0; i < darray_get_length(scene_data->texture_assets); i++) {
    texture_paths_length += string_get_raw_size(scene_data->texture_assets[i].image_path);
  }
  
  *size = sizeof(scene_bin_header_t) +
    (header->obj_counts[SCENE_OBJ_INDEX_ENTITY] * sizeof(Entity)) +
    (header->obj_counts[SCENE_OBJ_INDEX_CODE] * sizeof(CCode)) +
    (header->obj_counts[SCENE_OBJ_INDEX_MESH] * sizeof(CMesh)) +
    (header->obj_counts[SCENE_OBJ_INDEX_MATERIAL] * sizeof(CMaterial)) +
    (header->obj_counts[SCENE_OBJ_INDEX_TEXTURE] * sizeof(CCTexture)) +
    (header->obj_counts[SCENE_OBJ_INDEX_CAMERA] * sizeof(CCamera)) +
    (header->obj_counts[SCENE_OBJ_INDEX_BOX_COLLIDER_3D] * sizeof(CBoxCollider3D)) +
    (header->obj_counts[SCENE_OBJ_INDEX_BALL_COLLIDER_3D] * sizeof(CBallCollider3D)) +
    mesh_paths_length + texture_paths_length;
  
  return TRUE;
}

b8 scene_compiler_copy_data(SceneData* scene_data, char* data, scene_bin_header_t* header) {
  u32 offset = 0;
  u32 copy_size = 0;

  mcopy_memory(data + offset, header, sizeof(scene_bin_header_t));
  offset += sizeof(scene_bin_header_t);

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_ENTITY] * sizeof(Entity);
  mcopy_memory(data + offset, scene_data->entities, copy_size);
  offset += copy_size;

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_CODE] * sizeof(CCode);
  mcopy_memory(data + offset, scene_data->codes, copy_size);
  offset += copy_size;

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_MESH] * sizeof(CMesh);
  mcopy_memory(data + offset, scene_data->meshes, copy_size);
  offset += copy_size;

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_MATERIAL] * sizeof(CMaterial);
  mcopy_memory(data + offset, scene_data->materials, copy_size);
  offset += copy_size;

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_TEXTURE] * sizeof(CCTexture);
  mcopy_memory(data + offset, scene_data->textures, copy_size);
  offset += copy_size;

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_CAMERA] * sizeof(CCamera);
  mcopy_memory(data + offset, scene_data->cameras, copy_size);
  offset += copy_size;

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_BOX_COLLIDER_3D] * sizeof(CBoxCollider3D);
  mcopy_memory(data + offset, scene_data->box_colliders_3d, copy_size);
  offset += copy_size;

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_BALL_COLLIDER_3D] * sizeof(CBallCollider3D);
  mcopy_memory(data + offset, scene_data->ball_colliders_3d, copy_size);
  offset += copy_size;

  // ASSETS

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_MESH_ASSET] * sizeof(AMesh);
  mcopy_memory(data + offset, scene_data->mesh_assets, copy_size);
  offset += copy_size;

  copy_size = header->obj_counts[SCENE_OBJ_INDEX_TEXTURE_ASSET] * sizeof(ATexture);
  mcopy_memory(data + offset, scene_data->texture_assets, copy_size);
  offset += copy_size;

  for (u32 i = 0; i < darray_get_length(scene_data->mesh_assets); i++) {
    copy_size = string_get_raw_size(scene_data->mesh_assets[i].model_path);
    mcopy_memory(data + offset, string_get_raw(scene_data->mesh_assets[i].model_path),
		 copy_size);
    offset += copy_size;
  }

  for (u32 i = 0; i < darray_get_length(scene_data->texture_assets); i++) {
    copy_size = string_get_raw_size(scene_data->texture_assets[i].image_path);
    mcopy_memory(data + offset, string_get_raw(scene_data->texture_assets[i].image_path),
		 copy_size);
    offset += copy_size;
  }

  return TRUE;
}

b8 scene_compiler_write_file(StringView path, char* data, u32 size) {

  FILE* file;
  file = fopen(path, "wb");
  if (file == NULL) {
    MERROR_CORE("%d: %s", errno, strerror(errno));
    return FALSE;
  }
  if (fwrite(data, sizeof(char), size, file) != size) {
    MERROR_CORE("Failed to write file: %s", path);
    return FALSE;
  }

  fclose(file);
  
  return TRUE;
}
