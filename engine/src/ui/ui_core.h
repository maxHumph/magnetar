/**
 * @file ui_core.h
 */

#pragma once

#include "define.h"
#include "ui_types.h"

b8 ui_init();

MGAPI UiRoot* ui_root_create(const char* name, Vec2u ref_res); 

MGAPI UiRect* ui_rect_create(Vec2 extent, UiSizeType w_type, UiSizeType h_type);

MGAPI b8 ui_root_add_rect(UiRoot* root, UiRect* rect);

MGAPI b8 ui_add_rect(UiRect* src_rect, UiRect* sub_rect);
