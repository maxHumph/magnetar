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

MGAPI UiData* get_ui_data_ptr();

MGAPI UiRoot* ui_root_create(const char* name, Vec2u ref_res); 


MGAPI UiRect* ui_rect_create(Vec2u extent, Vec2u offset,
			     UiAnchorMask anchor_mask,
			     UiAttributeMask attribute_mask);

MGAPI void ui_root_add_rect(UiRoot* root, UiRect* rect);

MGAPI b8 ui_add_rect(UiRect* src_rect, UiRect* sub_rect);

MGAPI void ui_rect_set_color(UiRect* rect, UiColor color);

MGAPI void /*or b8*/ ui_submit_tree(UiRoot* root);

/**
 * @brief Generates vertex data for a UI tree.
 */
void ui_gen_vertices(UiRoot* root);

/**
 * @brief Recursively generates the vertext data for a UiRect and pushes
 * it onto the UI vertex array before doing the same for any children of
 * the UiRect.
 */
void ui_gen_rect_vertices(UiRect* rect, UiRoot* root);

Handle32 ui_check_interaction_boxes();
