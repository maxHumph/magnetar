/**
 * @file game.h
 */

#pragma once

#include <define.h>
#include <game_interface.h>

typedef struct GameState {
  f32 delta_time;
} GameState;

b8 game_initalize(Game* game_instance);

b8 on_update(Game* game_instance, f32 delta_time);

b8 on_render(Game* game_instance, f32 delta_time);

void on_resize(Game* game_instance, u32 width, u32 height);
