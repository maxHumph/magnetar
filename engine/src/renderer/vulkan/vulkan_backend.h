/**
 * @file vulkan_backend.h
 */

#pragma once

#include "define.h"
#include "platform/platform.h"
#include "renderer/renderer_backend.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_defines.h"

// MacOS Extensions
#if defined(MPLATFORM_APPLE)
#define MVK_INSTANCE_EXTENSION_NAMES                                                 \
  {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, \
   VK_KHR_SURFACE_EXTENSION_NAME}
#define MVK_INSTANCE_EXTENSION_COUNT 3
#define MVK_INSTANCE_CREATE_FLAGS VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
// Linux Extensions
#elif MPLATFORM_LINUX
#include <X11/Xlib-xcb.h>  // @TODO: Move all xcb headers to a platform specific file
#include <vulkan/vulkan_xcb.h>
#define MVK_INSTANCE_EXTENSION_NAMES                                 \
  {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, \
   VK_KHR_XCB_SURFACE_EXTENSION_NAME}
#define MVK_INSTANCE_EXTENSION_COUNT 3
#define MVK_INSTANCE_CREATE_FLAGS ZERO
// Windows Extensions
#elif MPLATFORM_WINDOWS
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
                       i16 start_width, i16 start_height, PlatformState* platform_state);

/**
 * @brief Destroys the VkInstance and other things that need to be banished to the shadow realm.
 * (Called by the renderer frontend via function pointers in a RendererBackend object.)
 * @param renderer_backend A pointer to the RendererBackend object.
 */
void vulkan_backend_shutdown(RendererBackend* renderer_backend);

b8 vulkan_backend_start_frame(RendererBackend* renderer_backend, f64 delta_time);

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time);

b8 vulkan_backend_draw_frame(RendererBackend* renderer_backend);

void vulkan_backend_resized(RendererBackend* renderer_backend, u16 width, u16 height);

// Static functions

static b8 vulkan_create_instance(const char* application_name);

static b8 vulkan_create_debug_messenger();

static b8 vulkan_select_physical_device();

static b8 vulkan_get_surface(PlatformState* platform_state, i16 width, i16 height);

static b8 vulkan_create_logical_device();

static b8 vulkan_create_swapchain();

static b8 vulkan_get_swapchain_images();

static b8 vulkan_create_image_views();

static b8 vulkan_create_graphics_pipeline();

static b8 vulkan_create_command_pool();

static b8 vulkan_allocate_command_buffers();

static b8 vulkan_create_sync_primatives();

static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                      VkDebugUtilsMessageTypeFlagsEXT message_types,
                      const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

static b8 vulkan_recreate_swapchain();

static b8 vulkan_cleanup_swapchain();

static void transition_image_layout(u32 image_index, VkImageLayout old_layout,
                                    VkImageLayout new_layout, VkAccessFlags2 src_access_mask,
                                    VkAccessFlags2 dst_access_mask,
                                    VkPipelineStageFlags2 src_stage_mask,
                                    VkPipelineStageFlags2 dst_stage_mask);
