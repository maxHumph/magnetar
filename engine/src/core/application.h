/**
 * @file application.h
 * @brief Contains things for creating and starting the application.
 */

#pragma once

#include "define.h"
#include "renderer/renderer_backend.h"

struct Game;

/**
 * @brief Contains data used to create the window.
 */
typedef struct ApplicationInfo {
  i16 start_x_pos;
  i16 start_y_pos;
  i16 start_width;
  i16 start_height;

  RendererAPI renderer_api;

  char* start_title;

} ApplicationInfo;

/**
 * @brief Create the application instance and initializes the subsystems.
 * @param A pointer to the game instance object.
 * @return TRUE if the application was created successfully, otherwise FALSE.
 */
MGAPI b8 application_create(struct Game* game_instance);

/**
 * @brief Starts the game loop.
 * @return ?
 */
MGAPI b8 application_run();
