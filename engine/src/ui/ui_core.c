/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ui_core.h"

#include "data_structures/darray.h"
#include "define.h"
#include "renderer/renderer_types.h"
#include "ui_types.h"
#include "core/events.h"
#include "core/input.h"

static UiData data = {};

// Event handler defs

b8 ui_on_button(u16 code, void* sender,
		void* listener, EventData event_data);

// API FUNCTIONS

b8 ui_init() {
  data.roots = darray_create(UiRoot);
  data.rects = darray_create(UiRect);
  data.clickables = darray_create(Handle32);

  event_register(EVENT_CODE_BUTTON_DOWN, 0, ui_on_button);

  return TRUE;
}

UiData* get_ui_data_ptr() { return &data; }

UiRoot* ui_root_create(const char* name, Vec2u ref_res) {
  UiRoot root = {
      .name = name,
      .reference_res = ref_res,
      .children = NULL_PTR,
  };
  darray_push(data.roots, root);
  return &data.roots[darray_get_length(data.roots) - 1];
}


// @TODO instead of returning a pointer return a handle since the pointer will become invalid.
UiRect* ui_rect_create(Vec2u extent, Vec2u offset,
		       UiAnchorMask anchor_mask,
		       UiAttributeMask attribute_mask) {
  UiRect rect = {
      .extent = extent,
      .offset = offset,
      .anchor_mask = anchor_mask,
      .attribute_mask = attribute_mask,
      .rooted = FALSE,
      .parent = NULL_PTR,
      .children = NULL_PTR,
  };
  darray_push(data.rects, rect);
  if (rect.attribute_mask & UI_ATTRIBUTE_CLICKABLE_BIT) {
    darray_push(data.clickables,
		(Handle32)(darray_get_length(data.rects) - 1));
  }
  return &data.rects[darray_get_length(data.rects) - 1];
}

void ui_root_add_rect(UiRoot* root, UiRect* rect) {
  if (root->children == NULL_PTR) {
    root->children = darray_create(UiRect*);
  }
  rect->rooted = TRUE;
  darray_push(root->children, rect);
}

b8 ui_add_rect(UiRect* src_rect, UiRect* sub_rect) {
  if (src_rect->parent == NULL_PTR && src_rect->rooted != TRUE) {
    MERROR_CORE("UiRect must already be part of a UI tree to have children attached");
    return FALSE;
  }
  if (src_rect->children == NULL_PTR) {
    src_rect->children = darray_create(UiRect*);
  }
  /*
  switch (sub_rect->anchor_mask) {
    case UI_ANCHOR_NONE:
      sub_rect->total_offset =
	vec2u_add(src_rect->total_offset, sub_rect->offset);
      break;

    default:
      sub_rect->total_offset =
	vec2u_add(src_rect->total_offset, sub_rect->offset);
      break;
  }
  */
  sub_rect->parent = src_rect;
  darray_push(src_rect->children, sub_rect);
  return TRUE;
}

void ui_rect_set_color(UiRect* rect, UiColor color) {
  if (!(rect->attribute_mask & UI_ATTRIBUTE_COLOR_BIT)) {
    rect->attribute_mask =
      rect->attribute_mask | UI_ATTRIBUTE_COLOR_BIT;
  }
  rect->color = color;
}

b8 ui_submit_tree(UiRoot* root) {
  return TRUE;
}

// INTERNAL FUNCTIONS

void ui_calc_rects(UiRect* rect) {

  if (rect->parent == NULL_PTR) {
    rect->total_offset = rect->offset;
    rect->total_extent = rect->extent;
  }

  if ((rect->anchor_mask & UI_ANCHOR_TOP_BIT) &&
      !(rect->anchor_mask & UI_ANCHOR_BOTTOM_BIT)) {

    rect->total_offset.y = rect->parent->total_offset.y;
    rect->total_extent.y = rect->extent.y;

  } else if (!(rect->anchor_mask & UI_ANCHOR_TOP_BIT) &&
	     (rect->anchor_mask & UI_ANCHOR_BOTTOM_BIT)) {

    rect->total_offset.y =
      (rect->parent->total_offset.y + rect->parent->total_extent.y) -
      rect->total_extent.y;
    rect->total_extent.y = rect->extent.y;

  } else if (rect->anchor_mask &
	     (UI_ANCHOR_TOP_BIT | UI_ANCHOR_BOTTOM_BIT)) {

    rect->total_offset.y = rect->parent->total_offset.y;
    rect->total_extent.y = rect->parent->total_extent.y;

  } else {

    rect->total_offset.y = rect->offset.y;
    rect->total_extent.y = rect->extent.y;
  }

  if ((rect->anchor_mask & UI_ANCHOR_LEFT_BIT) &&
      !(rect->anchor_mask & UI_ANCHOR_RIGHT_BIT)) {

    rect->total_offset.x = rect->parent->total_offset.x;
    rect->total_extent.x = rect->extent.x;

  } else if (!(rect->anchor_mask & UI_ANCHOR_LEFT_BIT) &&
	     (rect->anchor_mask & UI_ANCHOR_RIGHT_BIT)) {

    rect->total_offset.x =
      (rect->parent->total_offset.x + rect->parent->total_extent.x) -
      rect->total_extent.x;
    rect->total_extent.x = rect->extent.x;

  } else if (rect->anchor_mask &
	     (UI_ANCHOR_LEFT_BIT | UI_ANCHOR_RIGHT_BIT)) {

    rect->total_offset.x = rect->parent->total_offset.x;
    rect->total_extent.x = rect->parent->total_extent.x;

  } else {

    rect->total_offset.x = rect->offset.x;
    rect->total_extent.x = rect->extent.x;
  }

  MTRACE("RECT -- offset: (%u, %u), extent: (%u, %u)",
	 rect->offset.x,
	 rect->offset.y,
	 rect->extent.x,
	 rect->extent.y);

  MTRACE("RECT TOTAL -- offset: (%u, %u), extent: (%u, %u)",
	 rect->total_offset.x,
	 rect->total_offset.y,
	 rect->total_extent.x,
	 rect->total_extent.y);

  if (rect->children != NULL_PTR) {
    for (u32 i = 0; i < darray_get_length(rect->children); i++) {
      ui_calc_rects(rect->children[i]);
    }
  }
  
}

void ui_gen_vertices(UiRoot* root) {
  data.rect_vertices = darray_create(UiVertex);
  data.rect_indices = darray_create(u32);
  for (u32 i = 0; i < darray_get_length(root->children); i++) {
    ui_gen_rect_vertices(root->children[i], root);
  }
}


void ui_gen_rect_vertices(UiRect* rect, UiRoot* root) {

  UiVertex v1 = {
      .pos =
          {
              .x = (f32)rect->total_offset.x,
              .y = (f32)rect->total_offset.y,
          },
      .color = rect->color.primary_color,
  };
  UiVertex v2 = {
      .pos =
          {
              .x = (f32)(rect->total_offset.x + rect->total_extent.x),
              .y = (f32)rect->total_offset.y,
          },
      .color = rect->color.primary_color,
  };
  UiVertex v3 = {
      .pos =
          {
              .x = (f32)(rect->total_offset.x + rect->total_extent.x),
              .y = (f32)(rect->total_offset.y + rect->total_extent.y),
          },
      .color = rect->color.primary_color,
  };
  UiVertex v4 = {
      .pos =
          {
              .x = (f32)rect->total_offset.x,
              .y = (f32)(rect->total_offset.y + rect->total_extent.y),
          },
      .color = rect->color.primary_color,
  };




  /*
  if ((rect->anchor_mask & UI_ANCHOR_TOP_BIT) &&
      !(rect->anchor_mask & UI_ANCHOR_BOTTOM_BIT)) {

    v1.pos.y = (f32)rect->parent->offset.y;
    v2.pos.y = (f32)rect->parent->offset.y;
    v3.pos.y += ((f32)rect->parent->offset.y - (f32)rect->offset.y);
    v4.pos.y += ((f32)rect->parent->offset.y - (f32)rect->offset.y);

  } else if (!(rect->anchor_mask & UI_ANCHOR_TOP_BIT) &&
	     (rect->anchor_mask & UI_ANCHOR_BOTTOM_BIT)) {
    v1.pos.y -= (((f32)rect->parent->offset.y + (f32)rect->parent->extent.y) -
		 ((f32)rect->offset.y + (f32)rect->extent.y));
    v2.pos.y -= (((f32)rect->parent->offset.y + (f32)rect->parent->extent.y) -
		 ((f32)rect->offset.y + (f32)rect->extent.y));
    v3.pos.y = (f32)rect->parent->offset.y + (f32)rect->parent->extent.y;
    v4.pos.y = (f32)rect->parent->offset.y + (f32)rect->parent->extent.y;
  } else if (rect->anchor_mask & (UI_ANCHOR_TOP_BIT | UI_ANCHOR_BOTTOM_BIT)) {
  } else {
  }

  if ((rect->anchor_mask & UI_ANCHOR_LEFT_BIT) && !(rect->anchor_mask & UI_ANCHOR_RIGHT_BIT)) {
  } else if (!(rect->anchor_mask & UI_ANCHOR_LEFT_BIT) && (rect->anchor_mask & UI_ANCHOR_RIGHT_BIT)) {
  } else if (rect->anchor_mask & (UI_ANCHOR_LEFT_BIT | UI_ANCHOR_RIGHT_BIT)) {
  } else {
  }
  */


  darray_push(data.rect_vertices, v1);
  darray_push(data.rect_vertices, v2);
  darray_push(data.rect_vertices, v3);
  darray_push(data.rect_vertices, v4);
  u32 vertex_count = darray_get_length(data.rect_vertices) - 1;

  darray_push(data.rect_indices, vertex_count - 3);
  darray_push(data.rect_indices, vertex_count - 2);
  darray_push(data.rect_indices, vertex_count - 1);
  darray_push(data.rect_indices, vertex_count - 3);
  darray_push(data.rect_indices, vertex_count - 1);
  darray_push(data.rect_indices, vertex_count);

  if (rect->children != NULL_PTR) {
    for (u32 i = 0; i < darray_get_length(rect->children); i++) {
      ui_gen_rect_vertices(rect->children[i], root);
    }
  }
}

b8 ui_on_button(u16 code, void* sender,
		void* listener, EventData event_data) {
  if (code == EVENT_CODE_BUTTON_DOWN) {
    Buttons buttoncode = event_data.data.u16[0];
    switch (buttoncode) {
    case BUTTON_0:
      break;
    default:
      break;
    }
  }
  return FALSE;
}

Handle32 ui_check_interaction_boxes() {
  for (u32 i = 0; i < darray_get_length(data.clickables); i++) {
    MTRACE("%d", i);
  }
}
