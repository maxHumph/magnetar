/**
 * @file vulkan_backend.h
 */

#pragma once

#include "define.h"
#include "renderer/renderer_backend.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_defines.h"

#if defined(MPLATFORM_APPLE)
#define MVK_INSTANCE_EXTENSION_NAMES                                                 \
  {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, \
   VK_KHR_SURFACE_EXTENSION_NAME}
#define MVK_INSTANCE_EXTENSION_COUNT 3
#define MVK_INSTANCE_CREATE_FLAGS VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
#else
#define MVK_INSTANCE_EXTENSION_NAMES \
  {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME}
#define MVK_INSTANCE_EXTENSION_COUNT 2
#define MVK_INSTANCE_CREATE_FLAGS ZERO
#endif

#define MVK_DEVICE_EXTENSION_NAMES {VK_KHR_SWAPCHAIN_EXTENSION_NAME}
#define MVK_DEVICE_EXTENSION_COUNT 1

#define MVK_LAYER_NAMES {"VK_LAYER_KHRONOS_validation"}
#define MVK_LAYER_COUNT 1
#if MRELEASE == 1
#define MVK_LAYER_NAMES NULL_PTR
#define MVK_LAYER_COUNT ZERO
#endif

/**
 * @brief Creates the vulkan instance and sets up needed layers and extensions. (Called by the
 * renderer frontend via function pointers in a RendererBackend object.)
 * @param renderer_backend A pointer to the RendererBackend object.
 * @param application_name The name to give the VkInstance.
 * @param platform_state A pointer to the PlatformState object.
 * @return TRUE if vulkan initialized successfully, otherwise FALSE.
 */
b8 vulkan_backend_init(RendererBackend* renderer_backend, const char* application_name,
                       PlatformState* platform_state);

/**
 * @brief Destroys the VkInstance and other things that need to be banished to the shadow realm.
 * (Called by the renderer frontend via function pointers in a RendererBackend object.)
 * @param renderer_backend A pointer to the RendererBackend object.
 */
void vulkan_backend_shutdown(RendererBackend* renderer_backend);

b8 vulkan_backend_start_frame(RendererBackend* renderer_backend, f64 delta_time);

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time);

void vulkan_backend_resized(RendererBackend* renderer_backend, u16 width, u16 height);

static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                      VkDebugUtilsMessageTypeFlagsEXT message_types,
                      const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);
