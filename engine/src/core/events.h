/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file events.h
 */

#pragma once

#include "define.h"

/* typedef enum EventType {} EventType; */

typedef struct EventData {
  union {
    i64 i64[2];
    u64 u64[2];
    f64 f64[2];

    i32 i32[4];
    u32 u32[4];
    f32 f32[4];

    i16 i16[8];
    u16 u16[8];

    i8 i8[16];
    u8 u8[16];
    char ch[16];

  } data;

} EventData;

// should return TRUE if the event has been handled.
typedef b8 (*fp_on_event)(u16 code, void* sender, void* listener, EventData data);

/**
 * @brief Initializes the event subsystem.
 * @return TRUE if successful. FALSE if unsucessful.
 */
b8 event_initialize();

/**
 * @brief Shutdown the event subsystem and deallocates any memory it was using.
 */
void event_shutdown();

/**
 * @brief
 */
MGAPI b8 event_register(u16 code, void* listener, fp_on_event on_event);

MGAPI b8 event_unregister(u16 code, void* listener, fp_on_event on_event);

MGAPI b8 fire_event(u16 code, void* sender, EventData data);

typedef enum SystemEventCodes {
  EVENT_CODE_APPLICATION_QUIT = 0x01,
  EVENT_CODE_WINDOW_RESIZED = 0x02,
  EVENT_CODE_KEY_DOWN = 0x03,
  EVENT_CODE_KEY_UP = 0x04,
  EVENT_CODE_BUTTON_DOWN = 0x05,
  EVENT_CODE_BUTTON_UP = 0x06,
  EVENT_CODE_MOUSE_MOVED = 0x07,
  EVENT_CODE_MOUSE_WHEEL = 0x08,
  MAX_EVENT_CODE = 0xFF,
} SystemEventCodes;
