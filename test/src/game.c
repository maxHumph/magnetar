#include "game.h"

#include <core/log.h>

b8 game_initalize(Game* game_instance) {
  MDEBUG("game_initialize() was called");
  return TRUE;
}

b8 on_update(Game* game_instance, f32 delta_time) {
  /* MDEBUG("on_update() was called"); */
  return TRUE;
}

b8 on_render(Game* game_instance, f32 delta_time) {
  /* MDEBUG("on_render() was called"); */
  return TRUE;
}

void on_resize(Game* game_instance, u32 width, u32 height) { MDEBUG("on_resize() was called"); }
