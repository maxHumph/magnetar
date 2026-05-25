#include <entry_point.h>
#include <platform/platform.h>

#include "game.h"

b8 create_game(Game* out_game) {
  out_game->application_info.start_x_pos = 100;
  out_game->application_info.start_y_pos = 100;
  out_game->application_info.start_width = 1280;
  out_game->application_info.start_height = 720;
  out_game->application_info.start_title = "Magnetar Engine Test Application";

  // Assign function pointers.
  out_game->initialize = game_initalize;
  out_game->on_update = on_update;
  out_game->on_render = on_render;
  out_game->on_resize = on_resize;

  out_game->state = platform_alloc(sizeof(GameState), FALSE);

  return TRUE;
}
