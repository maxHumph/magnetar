
/**
 * @file vulkan_wsi_mac.c
 */

#include <assert.h>

#include "define.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vulkan_defines.h"


#if defined(MPLATFORM_APPLE)

#include <vulkan/vulkan_metal.h>

#include "core/log.h"
#include "platform/platform_macos_types.m"
#include "vulkan_wsi.h"

b8 vulkan_create_platform_surface(VulkanContext* vulkan_context, PlatformState* platform_state) {
  InternalState* state = (InternalState*)platform_state->internal_state;

  VkMetalSurfaceCreateInfoEXT surface_create_info = {VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT};
  surface_create_info.pLayer = state->metal_layer;

  VkResult create_surface_res = vkCreateMetalSurfaceEXT(vulkan_context->instance, &surface_create_info, vulkan_context->allocator, &vulkan_context->surface);

  if (create_surface_res != VK_SUCCESS){
    MERROR_CORE("Failed to create MacOS Vulkan surface: %s", string_VkResult(create_surface_res));
    return FALSE;
  }

  return TRUE;
}

#endif
