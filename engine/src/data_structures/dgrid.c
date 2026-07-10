#include "dgrid.h"

#include "core/mmemory.h"

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
}

void _dgrid_set_field(void* dgrid, u64 field, u64 val) {
}

void _dgrid_set(void* dgrid, Vec2u64 pos, void* val_ptr) {
}

void* _dgrid_get(void* dgrid, Vec2u64 pos) {
}
