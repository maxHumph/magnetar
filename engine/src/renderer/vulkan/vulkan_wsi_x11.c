/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file vulkan_wsi_x11.c
 */

#include <assert.h>

#include "define.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vulkan_defines.h"

#if MPLATFORM_LINUX

#include <X11/Xlib-xcb.h>
#include <vulkan/vulkan_xcb.h>

#include "core/log.h"
#include "platform/platform_linux_types.h"
#include "vulkan_wsi.h"

b8 vulkan_create_platform_surface(VulkanContext* vulkan_context, PlatformState* platform_state) {
  InternalState* state = (InternalState*)platform_state->internal_state;

  VkXcbSurfaceCreateInfoKHR xcb_surface_create_info = {
      VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
  xcb_surface_create_info.pNext = NULL_PTR;
  xcb_surface_create_info.connection = state->connection;
  xcb_surface_create_info.window = state->window;

  VkResult create_xcb_surface_result =
      vkCreateXcbSurfaceKHR(vulkan_context->instance, &xcb_surface_create_info,
                            vulkan_context->allocator, &vulkan_context->surface);

  if (create_xcb_surface_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan XCB surface: %s",
                string_VkResult(create_xcb_surface_result));
    return FALSE;
  } else {
    MINFO_CORE("CREATED XCB SURFACE");
  }
  return TRUE;
}

#endif
