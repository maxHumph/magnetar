/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file ui_core.h
 */

#pragma once

#include "define.h"
#include "ui_types.h"

b8 ui_init();

/**
 * Unimplemented.
 */
void ui_on_update();

/**
 * @brief Called whenever the window is resized.
 *
 * @param w, The new width
 * @param h, The new height
 */
void ui_on_resize(u16 w, u16 h);

MGAPI UiData* get_ui_data_ptr();

MGAPI UiRootHandle ui_root_create(const char* name, Vec2u ref_res); 


MGAPI UiRectHandle ui_rect_create(Vec2u extent, Vec2u offset,
			     UiAnchorMask anchor_mask,
			     UiAttributeMask attribute_mask);

MGAPI void ui_root_add_rect(UiRootHandle root, UiRectHandle h_rect);

MGAPI b8 ui_add_rect(UiRectHandle h_src_rect, UiRectHandle h_sub_rect);

MGAPI b8 ui_submit_tree(UiRoot* root);

void ui_calc_rects(UiRectHandle h_rect);

/**
 * @brief Generates vertex data for a UI tree.
 */
void ui_gen_vertices(UiRootHandle h_root);

/**
 * @brief Recursively generates the vertext data for a UiRect and pushes
 * it onto the UI vertex array before doing the same for any children of
 * the UiRect.
 */
void ui_gen_rect_vertices(UiRectHandle h_rect, UiRoot* root);

Handle32 ui_check_interaction_boxes();

MGAPI char* ui_rect_string(UiRectHandle h_rect);

//----------------
//    ACTIONS
//----------------

MGAPI void ui_rect_show(UiRectHandle h_rect);
MGAPI void ui_rect_hide(UiRectHandle h_rect);

MGAPI void ui_rect_set_offset(UiRectHandle h_rect, Vec2u offset);
MGAPI void ui_rect_set_extent(UiRectHandle h_rect, Vec2u extent);

MGAPI void ui_rect_set_anchor_mask(UiRectHandle h_rect,
				   UiAnchorMask mask);
MGAPI void ui_rect_set_attribute_mask(UiRectHandle h_rect,
				      UiAttributeMask mask);

MGAPI void ui_rect_set_color(UiRectHandle h_rect, UiColor color);
