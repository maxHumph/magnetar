/**
 * @file ui_types.h
 */

#pragma once

#include "define.h"
#include "maths/vector.h"

typedef enum UiSizeType {
  UI_SIZE_ABS,
  UI_SIZE_REL,
} UiSizeType;
 
typedef enum UiAlignType {
  UI_ALIGN_TOP_LEFT,
  UI_ALIGN_TOP_CENTER,
  UI_ALIGN_TOP_RIGHT,
  UI_ALIGN_CENTER_LEFT,
  UI_ALIGN_CENTER,
  UI_ALIGN_CENTER_RIGHT,
  UI_ALIGN_BOTTOM_LEFT,
  UI_ALIGN_BOTTOM_CENTER,
  UI_ALIGN_BOTTOM_RIGHT,
} UiAlignType;

typedef struct UiRect UiRect;

typedef struct UiRoot {
  const char* name;
  Vec2u reference_res;
  UiRect* start_rect;
} UiRoot;

typedef struct UiRect {
  Vec2 extent;
  UiSizeType width_type;
  UiSizeType height_type;

  UiRect* children;

} UiRect;

typedef struct UiData {
  UiRoot* roots;
  UiRect* rects;
} UiData;
