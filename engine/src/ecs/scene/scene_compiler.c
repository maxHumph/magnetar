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

  // @TODO_MAYBE: Find a way to make this funciton less verbose.
  
  scene_bin_header_t header = {
    .signature = SCENE_BIN_SIG,
  };

  header.obj_counts[SCENE_OBJ_INDEX_ENTITY] = darray_get_length(scene_data->entities);
  header.obj_counts[SCENE_OBJ_INDEX_CODE] = darray_get_length(scene_data->codes);
  header.obj_counts[SCENE_OBJ_INDEX_MESH] = darray_get_length(scene_data->meshes);
  header.obj_counts[SCENE_OBJ_INDEX_MATERIAL] = darray_get_length(scene_data->materials);
  header.obj_counts[SCENE_OBJ_INDEX_TEXTURE] = darray_get_length(scene_data->textures);
  header.obj_counts[SCENE_OBJ_INDEX_CAMERA] = darray_get_length(scene_data->cameras);
  header.obj_counts[SCENE_OBJ_INDEX_BOX_COLLIDER_3D] =
    darray_get_length(scene_data->box_colliders_3d);
  header.obj_counts[SCENE_OBJ_INDEX_BALL_COLLIDER_3D] =
    darray_get_length(scene_data->ball_colliders_3d);

  *size = sizeof(scene_bin_header_t) +
    (header.obj_counts[SCENE_OBJ_INDEX_ENTITY] * sizeof(Entity)) +
    (header.obj_counts[SCENE_OBJ_INDEX_CODE] * sizeof(CCode)) +
    (header.obj_counts[SCENE_OBJ_INDEX_MESH] * sizeof(CMesh)) +
    (header.obj_counts[SCENE_OBJ_INDEX_MATERIAL] * sizeof(CMaterial)) +
    (header.obj_counts[SCENE_OBJ_INDEX_TEXTURE] * sizeof(CCTexture)) +
    (header.obj_counts[SCENE_OBJ_INDEX_CAMERA] * sizeof(CCamera)) +
    (header.obj_counts[SCENE_OBJ_INDEX_BOX_COLLIDER_3D] * sizeof(CBoxCollider3D)) +
    (header.obj_counts[SCENE_OBJ_INDEX_BALL_COLLIDER_3D] * sizeof(CBallCollider3D));

  *data = mallocate(*size, MEMORY_TAG_SCENE); // @TODO Free

  u32 offset = 0;

  mcopy_memory(*data + offset, &header, sizeof(scene_bin_header_t));
  offset += sizeof(scene_bin_header_t);

  mcopy_memory(*data + offset, scene_data->entities,
	       header.obj_counts[SCENE_OBJ_INDEX_ENTITY] * sizeof(Entity));
  offset += sizeof(header.obj_counts[SCENE_OBJ_INDEX_ENTITY] * sizeof(Entity));

  mcopy_memory(*data + offset, scene_data->codes,
	       header.obj_counts[SCENE_OBJ_INDEX_CODE] * sizeof(CCode));
  offset += sizeof(header.obj_counts[SCENE_OBJ_INDEX_CODE] * sizeof(CCode));

  return FALSE;
}

b8 scene_compiler_write_file(StringView path, char* data, u32 size) {
  return FALSE;
}
