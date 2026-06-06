/**
 * @file vulkan_defines.h
 */

#pragma once

#include <vulkan/vulkan.h>

#include "define.h"

typedef struct VulkanContext {
  VkInstance instance;
  VkPhysicalDevice physical_device;
  VkDevice logical_device;

  VkQueue queue;

  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkSurfaceFormatKHR surface_format;

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
  VkCommandBuffer command_buffer;

  VkSemaphore present_complete_semaphore;
  VkSemaphore render_complete_semaphore;
  VkFence draw_fence;

} VulkanContext;
