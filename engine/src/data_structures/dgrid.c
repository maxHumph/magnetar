#include "dgrid.h"

#include "core/mmemory.h"
#include "core/log.h"

void* _dgrid_create(u64 width, u64 height, u64 stride) {
  u64 head_size =  DGRID_FIELD_LENGTH * sizeof(u64);
  u64 array_size = width * height * stride;
  u64* array = mallocate(head_size + array_size, MEMORY_TAG_DARRAY);
  array[DGRID_WIDTH] = width;
  array[DGRID_HEIGHT] = height;
  array[DGRID_STRIDE] = stride;
  return (void*)(array + DGRID_FIELD_LENGTH);
}

void _dgrid_destroy(void* dgrid) {
  u64* head = (u64*)dgrid - DGRID_FIELD_LENGTH;
  u64 head_size = DGRID_FIELD_LENGTH * sizeof(u64);
  u64 total_size = head_size + (head[DGRID_WIDTH] *
                                head[DGRID_HEIGHT] *
                                head[DGRID_STRIDE]);
  mfree(head, total_size, MEMORY_TAG_DARRAY);
}

u64 _dgrid_get_field(void* dgrid, u64 field) {
  u64* head = (u64*)dgrid - DGRID_FIELD_LENGTH;
  return head[field];
}

void _dgrid_set_field(void* dgrid, u64 field, u64 val) {
  u64* head = (u64*)dgrid - DGRID_FIELD_LENGTH;
  head[field] = val;
}

void _dgrid_set(void* dgrid, Vec2u64 pos, void* val_ptr) {
  u64 stride = _dgrid_get_field(dgrid, DGRID_STRIDE);
  u64 width = _dgrid_get_field(dgrid, DGRID_WIDTH);
  u64 height = _dgrid_get_field(dgrid, DGRID_HEIGHT);

  if (pos.x > (width - 1)) {
    MERROR_CORE("Index out of bounds in _dgrid_set, Width: %u, pos.x: %u.", width, pos.x);
  }

  if (pos.y > (height - 1)) {
    MERROR_CORE("Index out of bounds in _dgrid_set, Height: %u, pos.y: %u.", height, pos.y);
  }

  u64 address = (u64)dgrid;
  address += ((pos.y * width) + (pos.x)) * stride;
  mcopy_memory((void*)address, val_ptr, stride); 
}

void* _dgrid_get(void* dgrid, Vec2u64 pos) {
  u64 stride = _dgrid_get_field(dgrid, DGRID_STRIDE);
  u64 width = _dgrid_get_field(dgrid, DGRID_WIDTH);
  u64 height = _dgrid_get_field(dgrid, DGRID_HEIGHT);
  
  if (pos.x > (width - 1)) {
    MERROR_CORE("Index out of bounds in _dgrid_get, Width: %u, pos.x: %u.", width, pos.x);
  }

  if (pos.y > (height - 1)) {
    MERROR_CORE("Index out of bounds in _dgrid_get, Height: %u, pos.y: %u.", height, pos.y);
  }

  u64 address = (u64)dgrid;
  address += ((pos.y * width) + pos.x) * stride;
  return (void*)address;
}
