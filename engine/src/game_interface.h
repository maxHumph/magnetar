/**
 * @file game_interface.h
 */

#pragma once

#include "core/application.h"

typedef struct Game {
  ApplicationInfo application_info;

  b8 (*initialize)(struct Game* game_instance);

  b8 (*on_update)(struct Game* game_instance, f32 delta_time);

  b8 (*on_render)(struct Game* game_instance, f32 delta_time);

  void (*on_resize)(struct Game* game_instance, u32 width, u32 height);

  void* state;

} Game;
