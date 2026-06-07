/**
 * @file vulkan_defines.h
 */

#pragma once

#include <vulkan/vulkan.h>

#include "define.h"
#include "maths/vector.h"

#define MAX_FRAMES_IN_FLIGHT 2

/**
 * @brief Stores vertex data for use i vertex buffers.
 */
typedef struct Vertex {
  Vec2 position;
  Vec2 colour;
} Vertex;

typedef struct VulkanContext {
  VkInstance instance;
  VkPhysicalDevice physical_device;
  VkDevice logical_device;

  VkQueue queue;

  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkSurfaceFormatKHR surface_format;

  i16 height;
  i16 width;

  VkAllocationCallbacks* allocator;
  VkDebugUtilsMessengerEXT* debug_messenger;

  u32 graphics_queue_family_index;

  u32 swapchain_image_count;
  VkImage* swapchain_images;
  VkImageView* swapchain_image_views;
  u32 current_image_index;
  VkExtent2D swapchain_extent;

  VkPipeline graphics_pipeline;

  VkCommandPool command_pool;
  VkCommandBuffer command_buffers[MAX_FRAMES_IN_FLIGHT];

  u32 current_frame_index;

  VkSemaphore present_complete_semaphores[MAX_FRAMES_IN_FLIGHT];
  VkSemaphore* render_complete_semaphores;
  VkFence draw_fences[MAX_FRAMES_IN_FLIGHT];

  u64 vertex_buffer_length;
  Vertex* vertex_buffer;
  VkVertexInputBindingDescription vertex_binding_description;

} VulkanContext;
