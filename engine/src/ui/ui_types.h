/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file ui_types.h
 */

#pragma once

#include "define.h"
#include "maths/vector.h"
#include "renderer/renderer_types.h"

typedef Mask32 UiAttributeMask;
typedef Bit32 UiAttributeBit;

static const UiAttributeMask UI_ATTRIBUTR_NONE = 0x00000000U;
static const UiAttributeBit UI_ATTRIBUTE_COLOR_BIT = 0x00000001U;
static const UiAttributeBit UI_ATTRIBUTE_IMAGE_BIT = 0x00000002U;

typedef enum UiSizeType {
  UI_SIZE_ABS,
  UI_SIZE_REL,

  MAX_UI_SIZE
} UiSizeType;

typedef enum UiAlignType {
  UI_ALIGN_NONE,

  UI_ALIGN_LEFT,
  UI_ALIGN_RIGHT,
  UI_ALIGN_TOP,
  UI_ALIGN_BOTTOM,
  UI_ALIGN_CENTER,
  UI_ALIGN_TOP_LEFT,
  UI_ALIGN_TOP_CENTER,
  UI_ALIGN_TOP_RIGHT,
  UI_ALIGN_CENTER_LEFT,
  UI_ALIGN_CENTER_RIGHT,
  UI_ALIGN_BOTTOM_LEFT,
  UI_ALIGN_BOTTOM_CENTER,
  UI_ALIGN_BOTTOM_RIGHT,

  MAX_UI_ALIGN,
} UiAlignType;

typedef enum UiColorGradType {
  UI_COLOR_GRAD_SOLID,

  UI_COLOR_GRAD_L_TO_R,
  UI_COLOR_GRAD_R_TO_L,
  UI_COLOR_GRAD_T_TO_B,
  UI_COLOR_GRAD_B_TO_T,

  MAX_UI_COLOR_GRAD,
} UiColorGradType;

typedef enum UiScaleRule {
  MAX_UI_SCALE_RULE,
} UiScaleRule;

typedef struct UiRect UiRect;

typedef struct UiRoot {
  const char* name;
  Vec2u reference_res;
  UiRect** children;
} UiRoot;

typedef struct UiColor {
  Vec3 primary_color;
  Vec3 secondary_color;
  UiColorGradType gradient;
} UiColor;

typedef struct UiRect {
  Vec2u extent;
  Vec2u offset;

  Vec2u total_offset;

  UiAlignType align;

  UiAttributeMask attribute_mask;

  UiColor color;

  b8 rooted;

  UiRect* parent;
  UiRect** children;

} UiRect;

typedef struct UiData {
  UiRoot* roots;
  UiRect* rects;

  UiVertex* rect_vertices;
  u32* rect_indices;
} UiData;
