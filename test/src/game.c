#include "game.h"

#include <core/log.h>
#include <core/mmemory.h>

b8 game_initalize(Game* game_instance) {
  MDEBUG("game_initialize() was called");
  MINFO(get_memory_usage_string());
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
