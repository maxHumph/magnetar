#include "game.h"

#include <core/log.h>
#include <core/mmemory.h>
#include <core/input.h>
#include <ecs/ecs.h>
#include <asset/mg3d.h>

#include "test_scene.h"

static SceneData* scene_data;
static Entity* player;

b8 game_initalize(Game* game_instance) {

  MDEBUG("game_initialize() was called");
  MINFO(get_memory_usage_string());

  //mg3d_from_obj("../test/res/models/crate.obj", "../test/res/models/crate.mg3d");
  /* 
  if (!scene_load(&test_scene)) {
    MERROR_CORE("Failed to load scene: %s", test_scene.name);
    return FALSE;
  }
  MTRACE("|| Scene: %s || -------- (%u) entities --------", test_scene.name, test_scene.entity_count);
  */

  camera_set_active(2);
  
  scene_data = get_scene_data_ptr();
  player = &scene_data->entities[2];


  MINFO(get_memory_usage_string());


  return TRUE;
}

b8 on_update(Game* game_instance, f64 delta_time) {

  if (key_down(KEY_W)) {
    player->transform.position.z -= 0.1f;
  }
  if (key_down(KEY_S)) {
    player->transform.position.z += 0.1f;
  }
  if (key_down(KEY_A)) {
    player->transform.position.x -= 0.1f;
  }
  if (key_down(KEY_D)) {
    player->transform.position.x += 0.1f;
  }

  return TRUE;
}

b8 on_render(Game* game_instance, f64 delta_time) {
  /* MDEBUG("on_render() was called"); */
  return TRUE;
}

void on_resize(Game* game_instance, u16 width, u16 height) { MDEBUG("on_resize() was called"); }
