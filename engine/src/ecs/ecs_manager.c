#include "ecs_manager.h"

#include "asset/mg3d.h"
#include "renderer/renderer_frontend.h"

b8 scene_load(Scene* scene) {
  for (u32 i = 0; i < scene->entity_count; i++) {
    if (!entity_load(scene->entities[i])) {
      MERROR_CORE("Failed to load entity: %s", scene->entities[i]->name);
      return FALSE;
    }
  }

  return TRUE;
};

b8 scene_unload(Scene* scene) {
  for (u32 i = 0; i < scene->entity_count; i++) {
    if (!entity_unload(scene->entities[i])) {
      MERROR_CORE("Failed to unload entity: %s", scene->entities[i]->name);
      return FALSE;
    }
  }
  return TRUE;
};

static b8 entity_load(Entity* entity) {
  for (u32 i = 0; i < entity->component_count; i++) {
    switch(entity->components[i].type) {
    case COMPONENT_TYPE_CODE:
      break;
      
    case COMPONENT_TYPE_TRANSFORM:
      break;

    case COMPONENT_TYPE_MESH:
      if (!cmesh_load((CMesh*)entity->components[i].component)) {
	MERROR_CORE("Failed to load mesh component for entity: %s", entity->name);
	return FALSE;
      }
      break;

    case COMPONENT_TYPE_MATERIAL:
      if (!cmaterial_load((CMaterial*)entity->components[i].component)) {
	MERROR_CORE("Failed to load material component for entity: %s", entity->name);
	return FALSE;
      }
      break;

    default:
      break;
    }
  }
  return TRUE;
};

static b8 entity_unload(Entity* entity) {
  MWARN_CORE("entity_unload() memory_leak!!!");
  return TRUE;
};

static b8 cmesh_load(CMesh* mesh) {
  if (!mg3d_load(mesh->model_path, &mesh->vertices, &mesh->vertex_count, &mesh->indices, &mesh->index_count)) {
    MERROR_CORE("Failed to load model: %s", mesh->model_path);
    return FALSE;
  }
  renderer_link_mesh(mesh);
  return TRUE;
}

static b8 cmesh_unload(CMesh* mesh) {
  MWARN_CORE("cmesh_unload() memory_leak!!!");
  return TRUE;
}

static b8 cmaterial_load(CMaterial* material) {
  return TRUE;
}

static b8 cmaterial_unload(CMaterial* material) {
  MWARN_CORE("cmaterial_unload() memory leak!!!");
  return TRUE;
}
