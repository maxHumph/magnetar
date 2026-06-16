/**
 * @file vulkan_defines.h
 */

#pragma once

#include <vulkan/vulkan.h>

#include "define.h"
#include "maths/matrix.h"
#include "maths/vector.h"
#include "vulkan/vulkan_core.h"

#define MAX_FRAMES_IN_FLIGHT 2

#define MVK_VERTEX_ATTRIBUTE_COUNT 3

/**
 * @brief Stores vertex data for use i vertex buffers.
 */
typedef struct Vertex {
  Vec3 position;
  Vec3 colour;
  Vec2 texture_coord;

} Vertex;

typedef struct MVPMat {
  Mat4 model;
  Mat4 view;
  Mat4 proj;
} MVPMat;

typedef struct VulkanContext {
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

  VkPipeline graphics_pipeline;
  VkPipelineLayout pipeline_layout;

  VkCommandPool graphics_command_pool;
  VkCommandPool transfer_command_pool;
  VkCommandBuffer command_buffers[MAX_FRAMES_IN_FLIGHT];

  u32 current_frame_index;

  VkSemaphore present_complete_semaphores[MAX_FRAMES_IN_FLIGHT];
  VkSemaphore* render_complete_semaphores;
  VkFence draw_fences[MAX_FRAMES_IN_FLIGHT];

  u32 vertex_count;
  Vertex* vertices;
  VkVertexInputBindingDescription vertex_binding_description;
  VkBuffer staging_vertex_buffer;
  VkDeviceMemory staging_vertex_buffer_mem;
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;

  u32 index_count;
  u32* indices;
  VkBuffer staging_index_buffer;
  VkDeviceMemory staging_index_buffer_mem;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_mem;

  VkBuffer staging_texture_buf;
  VkDeviceMemory staging_texture_buf_mem;
  VkImage texture_image;
  VkDeviceMemory texture_image_mem;
  VkImageView texture_image_view;
  VkSampler texture_image_sampler;

  VkImage depth_image;
  VkDeviceMemory depth_image_mem;
  VkImageView depth_image_view;
  VkFormat depth_format;

  VkDescriptorSetLayout descriptor_set_layout;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSet descriptor_sets[MAX_FRAMES_IN_FLIGHT];

  VkBuffer uniform_buffers[MAX_FRAMES_IN_FLIGHT];
  VkDeviceMemory uniform_buffer_mem[MAX_FRAMES_IN_FLIGHT];
  void* uniform_buffer_mem_mapped[MAX_FRAMES_IN_FLIGHT];

} VulkanContext;
