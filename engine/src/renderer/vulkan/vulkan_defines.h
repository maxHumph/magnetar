/**
 * @file vulkan_defines.h
 */

#pragma once

#include <vulkan/vulkan.h>

typedef struct VulkanContext {
  VkInstance instance;
  VkPhysicalDevice physical_device;
  VkDevice logical_device;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkAllocationCallbacks* allocator;
  VkDebugUtilsMessengerEXT* debug_messenger;

} VulkanContext;
