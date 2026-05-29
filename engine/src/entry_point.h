/**
 * @file entry_point.h
 * @brief Contains the main method.
 *
 * IMPORTANT: b8 create_game(Game*) must be defined in the game code.
 */

#pragma once

#include "core/application.h"
#include "core/log.h"
#include "core/mmemory.h"
#include "game_interface.h"

extern b8 create_game(Game* out_game);

int main(void) {
  if (!initialize_memory()) {
    MFATAL("Failed to initialize memory subsystem");
    return -1;
  }

  Game game_instance;
  if (!create_game(&game_instance)) {
    MFATAL("Failed to create_game.");
    return -2;
  }

  if (!game_instance.initialize || !game_instance.on_update || !game_instance.on_render ||
      !game_instance.on_resize) {
    MFATAL("Required Game function pointers are not assigned.");
    return -3;
  }

  // Init
  if (!application_create(&game_instance)) {
    MINFO("Failed to create application.");
    return 1;
  }

  // Start game loop
  if (!application_run()) {
    MINFO("Failed to run application.");
    return 2;
  }

  shutdown_memory();

  return 0;
}
