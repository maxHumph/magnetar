/**
 * @file darray.h
 */

#pragma once

#include "define.h"

enum {
  DARRAY_CAPACITY,
  DARRAY_LENGTH,
  DARRAY_STRIDE,
  DARRAY_FIELD_LENGTH,
};

/**
 * @brief Creates a new dynamic array of.
 * @param length The initial capacity of the array.
 * @param stride The size in bytes of each item in the array.
 * @return A pointer to the first item in the array.
 */
MGAPI void* _darray_create(u64 length, u64 stride);

/**
 * @brief Destroys a dynamic array and deallocates the used memory.
 * @param darray A pointer to the array.
 */
MGAPI void _darray_destroy(void* darray);

MGAPI u64 _darray_get(void* darray, u64 field);
MGAPI void _darray_set(void* darray, u64 field, u64 val);

MGAPI void* _darray_resize(void* darray);

MGAPI void* _darray_push(void* darray, const void* val_ptr);
MGAPI void* _darray_pop(void* darray, void* dest);

MGAPI void* _darray_insert(void* darray, u64 index, void* val_ptr);
MGAPI void* _darray_pop_at(void* darray, u64 index, void* dest);

#define DARRAY_DEFAULT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

#define darray_create(type) _darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))
#define darray_create_reserved(type, capacity) _darray_create(capacity, sizeof(type))
#define darray_destroy(darray) _darray_destroy(darray)

#define darray_push(darray, val)          \
  {                                       \
    __auto_type temp = val;               \
    darray = _darray_push(darray, &temp); \
  }

#define darray_pop(darray, val_ptr) _darray_pop(darray, val_ptr)

#define darray_insert(darray, index, val)        \
  {                                              \
    __auto_type temp = val;                      \
    darray = _darray_insert(darray, index, val); \
  }

#define darray_pop_at(darray, index, dest) _darray_pop_at(darray, index, dest)

#define darray_clear(darray) _darray_set(darray, DARRAY_LENGTH, 0)

#define darray_get_capacity(darray) _darray_get(darray, DARRAY_CAPACITY)

#define darray_get_length(darray) _darray_get(darray, DARRAY_LENGTH)

#define darray_get_stride(darray) _darray_get(darray, DARRAY_STRIDE)

#define darray_set_length(darray, length) _darray_set(darray, DARRAY_LENGTH, length)
