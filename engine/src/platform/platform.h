/**
 * @file platform.h
 * @brief Contains platform agnostic functions for doing platform specific tasks.
 */

#pragma once

#include "define.h"

typedef struct PlatformState {
  void* internal_state;
} PlatformState;

b8 plataform_startup(PlatformState* platform_state, const char* application_name, i32 x_pos,
                     i32 y_pos, i32 width, i32 height);

void platform_shutdown(PlatformState* platform_state);

b8 platform_pump_messages(PlatformState* platform_state);

void* platform_alloc(u64 size, b8 is_aligned);
void platform_free(void* block, b8 is_aligned);
void* platform_mem_zero(void* block, u64 size);
void* platform_mem_cpy(void* out, const void* src, u64 size);
void* platoform_mem_set(void* target, i32 val, u64 size);

void platform_print(const char* message, u8 colour);
void platform_print_error(const char* message, u8 colour);

f64 platform_get_time_abs();

void platform_sleep(u64 ms);
