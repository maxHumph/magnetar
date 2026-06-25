/**
 * @file vulkan_defines.h
 */

#pragma once

#include <vulkan/vulkan.h>

#include "define.h"
#include "maths/matrix.h"
#include "maths/vector.h"
#include "vulkan/vulkan_core.h"
#include "ecs/ecs_types.h"
#include "renderer/renderer_types.h"

// MacOS Extensions
#if defined(MPLATFORM_APPLE)
#include <vulkan/vulkan_metal.h>
#define MVK_INSTANCE_EXTENSION_NAMES                                                 \
  {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, \
    VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_METAL_SURFACE_EXTENSION_NAME}
#define MVK_INSTANCE_EXTENSION_COUNT 4
#define MVK_INSTANCE_CREATE_FLAGS VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
#define MVK_DEVICE_EXTENSION_NAMES {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"}
#define MVK_DEVICE_EXTENSION_COUNT 2
// Linux Extensions
#elif MPLATFORM_LINUX
#include <X11/Xlib-xcb.h>  // @TODO: Move all xcb headers to a platform specific file
#include <vulkan/vulkan_xcb.h>
#define MVK_INSTANCE_EXTENSION_NAMES                                 \
  {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, \
   VK_KHR_XCB_SURFACE_EXTENSION_NAME}
#define MVK_INSTANCE_EXTENSION_COUNT 3
#define MVK_INSTANCE_CREATE_FLAGS ZERO
#define MVK_DEVICE_EXTENSION_NAMES {VK_KHR_SWAPCHAIN_EXTENSION_NAME}
#define MVK_DEVICE_EXTENSION_COUNT 1
// Windows Extensions
#elif MPLATFORM_WINDOWS
#define MVK_INSTANCE_EXTENSION_NAMES \
  {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME}
#define MVK_INSTANCE_EXTENSION_COUNT 2
#define MVK_INSTANCE_CREATE_FLAGS ZERO
#define MVK_DEVICE_EXTENSION_NAMES {VK_KHR_SWAPCHAIN_EXTENSION_NAME}
#define MVK_DEVICE_EXTENSION_COUNT 1
#endif


#define MVK_LAYER_NAMES {"VK_LAYER_KHRONOS_validation"}
#define MVK_LAYER_COUNT 1
#if MRELEASE == 1
#define MVK_LAYER_NAMES NULL_PTR
#define MVK_LAYER_COUNT ZERO
#endif

#define MAX_FRAMES_IN_FLIGHT 2

#define MVK_VERTEX_ATTRIBUTE_COUNT 3

/**
 * @struct VulkanDrawable
 * @brief Containes information about an object to be used during rendering.
 */
typedef struct VulkanDrawable {
  Handle32 vbuf_handle;
  Handle32 ibuf_handle;
  Handle32 texture_handle;
} VulkanDrawable;

/**
 * @struct VulkanContext
 * @brief Contains useful data for vulkan rendering.
 */
typedef struct VulkanContext {
  SceneData* scene_data;

  VulkanDrawable* objects;

  VkInstance instance;
  VkPhysicalDevice physical_device;
  VkDevice logical_device;

  VkPhysicalDeviceProperties physical_device_properties;

  VkQueue graphics_queue;
  VkQueue transfer_queue;

  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkSurfaceFormatKHR surface_format;

  i16 height;
  i16 width;

  VkAllocationCallbacks* allocator;
  VkDebugUtilsMessengerEXT* debug_messenger;

  u32 queue_family_index_count;
  u32 graphics_queue_family_index;
  u32 transfer_queue_family_index;

  u32 swapchain_image_count;
  VkImage* swapchain_images;
  VkImageView* swapchain_image_views;
  u32 current_image_index;
  VkExtent2D swapchain_extent;

  VkPipeline pbr_pipeline;
  VkPipelineLayout pbr_pipeline_layout;
  VkPipeline ui_pipeline;
  VkPipelineLayout ui_pipeline_layout;

  VkCommandPool graphics_command_pool;
  VkCommandPool transfer_command_pool;
  VkCommandBuffer command_buffers[MAX_FRAMES_IN_FLIGHT];

  u32 current_frame_index;

  VkSemaphore present_complete_semaphores[MAX_FRAMES_IN_FLIGHT];
  VkSemaphore* render_complete_semaphores;
  VkFence draw_fences[MAX_FRAMES_IN_FLIGHT];

  VkVertexInputBindingDescription vertex_binding_description;
  VkBuffer staging_vertex_buffer;
  VkDeviceMemory staging_vertex_buffer_mem;
  VkBuffer* vertex_bufs;
  VkDeviceMemory* vertex_buf_mem;

  VkBuffer staging_index_buffer;
  VkDeviceMemory staging_index_buffer_mem;
  VkBuffer* index_bufs;
  VkDeviceMemory* index_buf_mem;

  VkBuffer staging_texture_buf;
  VkDeviceMemory staging_texture_buf_mem;
  VkImage* texture_images;
  VkDeviceMemory* texture_image_mem;
  VkImageView* texture_image_views;
  VkSampler texture_image_sampler;

  VkImage depth_image;
  VkDeviceMemory depth_image_mem;
  VkImageView depth_image_view;
  VkFormat depth_format;

  VkDescriptorSetLayout descriptor_set_layout;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSet* descriptor_sets;

  u32 uniform_object_count;
  VkBuffer* uniform_bufs;
  VkDeviceMemory* uniform_buf_mem;
  void** uniform_buf_mem_map;

  /*
  VkBuffer uniform_buffers[MAX_FRAMES_IN_FLIGHT];
  VkDeviceMemory uniform_buffer_mem[MAX_FRAMES_IN_FLIGHT];
  void* uniform_buffer_mem_mapped[MAX_FRAMES_IN_FLIGHT];
  */

} VulkanContext;
