#include "ui_core.h"

#include "ui_types.h"
#include "data_structures/darray.h"

static UiData data = {};

b8 ui_init() {
  data.roots = darray_create(UiRoot);
  data.rects = darray_create(UiRect);
  return TRUE;
}

MGAPI UiRoot* ui_root_create(const char* name, Vec2u ref_res) {
  UiRoot root = {
    .name = name,
    .reference_res = ref_res,
    .start_rect = NULL_PTR,
  };
  darray_push(data.roots, root);
  return &data.roots[darray_get_length(data.roots) - 1];
}

MGAPI UiRect* ui_rect_create(Vec2 extent, UiSizeType w_type, UiSizeType h_type) {
  return NULL_PTR;
}

MGAPI b8 ui_root_add_rect(UiRoot* root, UiRect* rect) {
  return TRUE;
}

MGAPI b8 ui_add_rect(UiRect* src_rect, UiRect* sub_rect) {
  return TRUE;
}
