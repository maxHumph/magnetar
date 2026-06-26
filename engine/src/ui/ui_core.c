#include "ui_core.h"

#include "ui_types.h"
#include "data_structures/darray.h"

static UiData data = {};

b8 ui_init() {
  data.roots = darray_create(UiRoot);
  data.rects = darray_create(UiRect);
  return TRUE;
}

MGAPI UiData* get_ui_data_ptr() {
  return &data;
}

MGAPI UiRoot* ui_root_create(const char* name, Vec2u ref_res) {
  UiRoot root = {
    .name = name,
    .reference_res = ref_res,
    .children = NULL_PTR,
  };
  darray_push(data.roots, root);
  return &data.roots[darray_get_length(data.roots) - 1];
}

MGAPI UiRect* ui_rect_create_aligned(Vec2 extent, UiSizeType w_type, UiSizeType h_type, UiAlignType align) {
  UiRect rect = {
    .extent = extent,
    .w_type = w_type,
    .h_type = h_type,
    .offset = (Vec2){0.0f, 0.0f},
    .x_type = UI_SIZE_ABS,
    .y_type = UI_SIZE_ABS,
    .align = align,
    .children = NULL_PTR,
  };
  darray_push(data.rects, rect);
  return &data.rects[darray_get_length(data.rects) - 1];
}

MGAPI UiRect* ui_rect_create_floating(Vec2 extent, UiSizeType w_type, UiSizeType h_type, Vec2 offset, UiSizeType x_type, UiSizeType y_type) {
  UiRect rect = {
    .extent = extent,
    .w_type = w_type,
    .h_type = h_type,
    .offset = offset,
    .x_type = x_type,
    .y_type = y_type,
    .align = UI_ALIGN_NONE,
    .children = NULL_PTR,
  };
  darray_push(data.rects, rect);
  return &data.rects[darray_get_length(data.rects) - 1];
}

MGAPI void ui_root_add_rect(UiRoot* root, UiRect* rect) {
  if (root->children == NULL_PTR) {
    root->children = darray_create(UiRect*);
  }
  darray_push(root->children, rect);
}

MGAPI void ui_add_rect(UiRect* src_rect, UiRect* sub_rect) {
  if (src_rect->children == NULL_PTR) {
    src_rect->children = darray_create(UiRect*);
  }
  darray_push(src_rect->children, sub_rect);
}

MGAPI void ui_rect_set_color(UiRect* rect, UiColor color) {
  if (!(rect->attribute_mask & UI_ATTRIBUTE_COLOR_BIT)) {
    rect->attribute_mask = rect->attribute_mask | UI_ATTRIBUTE_COLOR_BIT;
  }
  rect->color = color;
}
