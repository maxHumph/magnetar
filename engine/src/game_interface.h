/**
 * @file game_interface.h
 */

#pragma once

#include "core/application.h"
#include "renderer/renderer_backend.h"

typedef struct Game {
  ApplicationInfo application_info;

  b8 (*initialize)(struct Game* game_instance);

  b8 (*on_update)(struct Game* game_instance, f64 delta_time);

  b8 (*on_render)(struct Game* game_instance, f64 delta_time);

  void (*on_resize)(struct Game* game_instance, u16 width, u16 height);

  void* state;

} Game;
