/**
 * @file application.h
 */

#pragma once

#include "define.h"

struct Game;

typedef struct ApplicationInfo {
  i16 start_x_pos;
  i16 start_y_pos;
  i16 start_width;
  i16 start_height;

  char* start_title;

} ApplicationInfo;

MGAPI b8 application_create(struct Game* game_instance);
MGAPI b8 application_run();
