/**
 * @file ui_core.h
 */

#pragma once

#include "define.h"
#include "ui_types.h"

b8 ui_init();

MGAPI UiData* get_ui_data_ptr();

MGAPI UiRoot* ui_root_create(const char* name, Vec2u ref_res); 

MGAPI UiRect* ui_rect_create_aligned(Vec2 extent, UiSizeType w_type, UiSizeType h_type, UiAlignType align);

MGAPI UiRect* ui_rect_create_floating(Vec2 extent, UiSizeType w_type, UiSizeType h_type, Vec2 offset, UiSizeType x_type, UiSizeType y_type);

MGAPI void ui_root_add_rect(UiRoot* root, UiRect* rect);

MGAPI void ui_add_rect(UiRect* src_rect, UiRect* sub_rect);

MGAPI void ui_rect_set_color(UiRect* rect, UiColor color);
