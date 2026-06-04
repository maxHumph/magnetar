/**
 * @file vulkan_defines.h
 */

#pragma once

#include <vulkan/vulkan.h>

typedef struct VulkanContext {
  VkInstance instance;
  VkDevice device;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkAllocationCallbacks* allocator;
  VkDebugUtilsMessengerEXT* debug_messenger;

} VulkanContext;
