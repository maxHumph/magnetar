/**
 * @file dgrid.h
 */

#pragma once

#include "define.h"
#include "maths/vector.h"

enum {
  DGRID_WIDTH,
  DGRID_HEIGHT,
  DGRID_STRIDE,

  DGRID_FIELD_LENGTH,
};

MGAPI void* _dgrid_create(u64 width, u64 height, u64 stride);
MGAPI void _dgrid_destroy(void* dgrid);

MGAPI u64 _dgrid_get_field(void* dgrid, u64 field);
MGAPI void _dgrid_set_field(void* dgrid, u64 field, u64 val);

MGAPI void _dgrid_set(void* dgrid, Vec2u64 pos, void* val_ptr);
MGAPI void* _dgrid_get(void* dgrid, Vec2u64 pos);
