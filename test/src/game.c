#include "game.h"

#include <core/log.h>
#include <core/mmemory.h>
#include <maths/matrix.h>
#include <maths/maths_util.h>

b8 game_initalize(Game* game_instance) {
  MDEBUG("game_initialize() was called");
  MINFO(get_memory_usage_string());


  Vec4 vec_1 = vec4_create(2.0f, 3.0f, 0.0f, 1.0f);

  Mat4 transform = mat4_from_quat(quat_from_euler((Vec3) { M_TO_RAD(180.0f), 0.0f, 0.0f}));
  mat4_print(transform);


  return TRUE;
}

b8 on_update(Game* game_instance, f64 delta_time) {
  /* MTRACE("on_update() was called"); */
  return TRUE;
}

b8 on_render(Game* game_instance, f64 delta_time) {
  /* MDEBUG("on_render() was called"); */
  return TRUE;
}

void on_resize(Game* game_instance, u16 width, u16 height) { MDEBUG("on_resize() was called"); }
