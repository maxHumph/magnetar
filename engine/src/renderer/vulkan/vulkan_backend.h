/**
 * @file vulkan_backend.h
 */

#pragma once

#include "define.h"
#include "renderer/renderer_backend.h"

b8 vulkan_backend_init(RendererBackend* renderer_backend, const char* application_name,
                       PlatformState* platform_state);

void vulkan_backend_shutdown(RendererBackend* renderer_backend);

b8 vulkan_backend_start_frame(RendererBackend* renderer_backend, f64 delta_time);

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time);

void vulkan_backend_resized(RendererBackend* renderer_backend, u16 width, u16 height);
