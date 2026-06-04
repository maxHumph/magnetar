/**
 * @file renderer_backend.h
 */

#pragma once

#include "define.h"
#include "platform/platform.h"

typedef enum RendererAPI {
  RENDERER_API_VULKAN,
} RendererAPI;

typedef struct RendererBackend {
  u64 frame_number;

  b8 (*init)(struct RendererBackend* renderer_backend, const char* application_name,
             i16 start_width, i16 start_height, PlatformState* platform_state);

  void (*shutdown)(struct RendererBackend* renderer_backend);

  b8 (*start_frame)(struct RendererBackend* renderer_backend, f64 delta_time);

  b8 (*end_frame)(struct RendererBackend* renderer_backend, f64 delta_time);

  void (*resized)(struct RendererBackend* renderer_backend, u16 width, u16 height);

} RendererBackend;

b8 renderer_backend_create(RendererAPI api, const char* application_name, RendererBackend* backend);

void renderer_backend_destroy(RendererBackend* backend);
