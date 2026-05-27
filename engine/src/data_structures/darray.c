#include "darray.h"

#include "core/mmemory.h"

void* _darray_create(u64 length, u64 stride) {
  u64 head_size = DARRAY_FIELD_LENGTH * sizeof(u64);
  u64 array_size = length * stride;
  u64* array = mallocate(head_size + array_size, MEMORY_TAG_DARRAY);
  array[DARRAY_CAPACITY] = length;
  array[DARRAY_LENGTH] = 0;
  array[DARRAY_STRIDE] = stride;
  return (void*)(array + DARRAY_FIELD_LENGTH);
}

void _darray_destroy(void* darray) {
  u64* head = (u64*)darray - DARRAY_FIELD_LENGTH;
  u64 head_size = DARRAY_FIELD_LENGTH * sizeof(u64);
  u64 total_size = head_size + (head[DARRAY_CAPACITY] * head[DARRAY_STRIDE]);
  mfree(head, total_size, MEMORY_TAG_DARRAY);
}

u64 _darray_get(void* darray, u64 field) {
  u64* head = (u64*)darray - DARRAY_FIELD_LENGTH;
  return head[field];
}

void _darray_set(void* darray, u64 field, u64 val) {
  u64* head = (u64*)darray - DARRAY_FIELD_LENGTH;
  head[field] = val;
}

void* _darray_resize(void* darray) {
  u64 length = _darray_get(darray, DARRAY_LENGTH);
  u64 stride = _darray_get(darray, DARRAY_STRIDE);
  void* temp = _darray_create(DARRAY_RESIZE_FACTOR * _darray_get(darray, DARRAY_CAPACITY), stride);
  mcopy_memory(temp, darray, length * stride);

  _darray_set(temp, DARRAY_LENGTH, length);
  _darray_destroy(darray);
  return temp;
}

void* _darray_push(void* darray, const void* val_ptr) {
  u64 length = _darray_get(darray, DARRAY_LENGTH);
  u64 stride = _darray_get(darray, DARRAY_STRIDE);
  if (length >= _darray_get(darray, DARRAY_CAPACITY)) {
    darray = _darray_resize(darray);
  }
  u64 address = (u64)darray;
  address += (length * stride);
  mcopy_memory((void*)address, val_ptr, stride);
  _darray_set(darray, DARRAY_LENGTH, length + 1);
  return darray;
}

void* _darray_pop(void* darray, void* dest) {
  u64 length = _darray_get(darray, DARRAY_LENGTH);
  u64 stride = _darray_get(darray, DARRAY_STRIDE);
  u64 address = (u64)darray;
  mcopy_memory(dest, (void*)address, stride);
  _darray_set(darray, DARRAY_LENGTH, length - 1);
  return dest;
}

void* _darray_insert(void* darray, u64 index, void* val_ptr);

void* _darray_pop_at(void* darray, u64 index, void* dest);
