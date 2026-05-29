/**
 * @file game.h
 */

#pragma once

#include <define.h>
#include <game_interface.h>

typedef struct GameState {
  f64 delta_time;
} GameState;

/**
 * @brief Called when the game is first opened.
 * @param game_instance A pointer to the game object.
 * @return Whether the initaization was succesful.
 */
b8 game_initalize(Game* game_instance);

/**
 * @brief Called continuously while the game is running.
 * @param game_instance A pointer to the Game object.
 * @param delta_time The time that the update is called.
 * @brief Whether the update was succesful.
 */
b8 on_update(Game* game_instance, f64 delta_time);

b8 on_render(Game* game_instance, f64 delta_time);

void on_resize(Game* game_instance, u16 width, u16 height);
