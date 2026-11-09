#include "scene_compiler.h"

#include "data/darray.h"
#include "core/mmemory.h"

b8 scene_compile(SceneData* scene_data, StringView path) {

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

  return FALSE;
}

b8 scene_compiler_generate_data(SceneData* scene_data, char** data, u32* size) {
  
  scene_bin_header_t header = {
    .signature = SCENE_BIN_SIG,
  };

  header.obj_counts[SCENE_OBJ_INDEX_ENTITY] = darray_get_length(scene_data->entities);
  header.obj_counts[SCENE_OBJ_INDEX_CODE] = darray_get_length(scene_data->codes);
  header.obj_counts[SCENE_OBJ_INDEX_MESH] = darray_g

  *size = sizeof(scene_bin_header_t) +
    (darray_get_length(scene_data->entities) * sizeof(Entity)) +
    (darray_get_length(scene_data->codes) * sizeof(CCode)) +
    (darray_get_length(scene_data->meshes) * sizeof(CMesh)) +
    (darray_get_length(scene_data->materials) * sizeof(CMaterial)) +
    (darray_get_length(scene_data->textures) * sizeof(CCTexture)) +
    (darray_get_length(scene_data->cameras) * sizeof(CCamera)) +
    (darray_get_length(scene_data->box_colliders_3d) * sizeof(CBoxCollider3D)) +
    (darray_get_length(scene_data->ball_colliders_3d) * sizeof(CBallCollider3D));

  *data = mallocate(*size, MEMORY_TAG_SCENE); // @TODO Free

  u32 offset = 0;


  //  mcopy_memory(*data[

  return FALSE;
}

b8 scene_compiler_write_file(StringView path, char* data, u32 size) {
  return FALSE;
}
