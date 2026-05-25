/**
 * @file platform.h
 * @brief Contains platform agnostic functions for doing platform specific tasks.
 */

#pragma once

#include "define.h"

/**
 * @brief Holds void pointers to objects in platform specific code to connect the engine to the
 * platform API.
 */
typedef struct PlatformState {
  void* internal_state;
} PlatformState;

/**
 * @brief Calls platform specific startup code.
 * @param platform_state Pointer to a plaform state.
 * @param application_name The window title.
 * @param x_pos The horizonal position of the window.
 * @param y_pos The vertical position of the window.
 * @param width The width in pixels of the render area of the window.
 * @param height The height in pixels of the render area of the window.
 * @return Whether the startup was succesful.
 */
b8 platform_startup(PlatformState* platform_state, const char* application_name, i32 x_pos,
                    i32 y_pos, i32 width, i32 height);

/**
 * @brief Calls platform specific shutdown code.
 * @param platform_state Pointer the the platform state object.
 */
void platform_shutdown(PlatformState* platform_state);

/**
 * @brief Calls platform specific message code for things like input events and window resizes.
 * @param platform_state Pointer to the platform state object.
 */
b8 platform_pump_messages(PlatformState* platform_state);

void* platform_alloc(u64 size, b8 is_aligned);
void platform_free(void* block, b8 is_aligned);
void* platform_mem_zero(void* block, u64 size);
void* platform_mem_cpy(void* out, const void* src, u64 size);
void* platform_mem_set(void* target, i32 val, u64 size);

void platform_print(const char* message, u8 colour);
void platform_print_error(const char* message, u8 colour);

/**
 * @brief Gets the system time via platform specific implementations.
 * @return The time in seconds.
 */
f64 platform_get_time_abs();

/**
 * @brief Calls a platorm specific sleep function.
 * @param ms The time to sleep for in ms.
 */
void platform_sleep(u64 ms);
