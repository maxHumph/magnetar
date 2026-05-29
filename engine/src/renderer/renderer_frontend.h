/**
 * @file renderer_frontend.h
 */

#pragma once

#include "define.h"
#include "game_interface.h"

b8 renderer_init(Game* game_instance);

void renderer_shutdown();

b8 renderer_start_frame(f64 delta_time);

b8 renderer_end_frame(f64 delta_time);

b8 renderer_draw_frame();
