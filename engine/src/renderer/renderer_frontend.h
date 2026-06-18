/**
 * @file renderer_frontend.h
 */

#pragma once

#include "define.h"
#include "game_interface.h"
#include "platform/platform.h"
#include "ecs/ecs_types.h"

b8 renderer_init(Game* game_instance, PlatformState* platform_state);

void renderer_shutdown();

b8 renderer_start_frame(f64 delta_time);

b8 renderer_end_frame(f64 delta_time);

b8 renderer_draw_frame();

b8 renderer_on_resize(u16 width, u16 height);

b8 renderer_link_mesh(CMesh* mesh);
