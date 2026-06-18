#pragma once

#include <ecs/ecs.h>

CTransform crate_transform = {
  .position = {0.5f, 0.2f, 0.6f},
  .scale = {1.0f, 1.0f, 1.0f},
};

CMesh crate_mesh = {
  .model_path = "../test/res/models/crate.mg3d",
};

CMaterial crate_material = {
  .texture = {
    .image_path = "../test/res/textures/crate.png",
  },
};

Component crate_components[] = {
  {.type = COMPONENT_TYPE_TRANSFORM, .component = &crate_transform}, 
  {.type = COMPONENT_TYPE_MESH, .component = &crate_mesh},
  {.type = COMPONENT_TYPE_MATERIAL, .component = &crate_material},
};

Entity crate_entity = {
  .parent = NULL_PTR,
  .child_count = 0,
  .children = NULL_PTR,

  .component_count = sizeof(crate_components) / sizeof(Component),
  .components = crate_components,
}; 


Entity* entities[] = {
  &crate_entity,
};

Scene scene01 = {
  .name = "Test scene",
  .entity_count = sizeof(entities) / sizeof(const Entity*),
  .entities = entities,
};
