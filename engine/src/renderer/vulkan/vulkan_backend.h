/**
 * @file vulkan_backend.h
 */

#pragma once

#include <stddef.h>

#include "define.h"
#include "maths/vector.h"
#include "platform/platform.h"
#include "renderer/renderer_backend.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_defines.h"


/**
 * @brief Creates and allocates all required vulkan onject to begin rendering.
 * @note This is called via function pointers in the renderer frontend.
 *
 * @param renderer_backend, A pointer to the RendererBackend object.
 * @param application_name, The name to give the VkInstance.
 * @param start_width, The starting width of the window.
 * @param start_height, The starting height of the window.
 * @param platform_state, A pointer to the PlatformState object.
 * @return TRUE if vulkan initialized successfully, otherwise FALSE.
 */
b8 vulkan_backend_init(RendererBackend* renderer_backend, const char* application_name,
                       i16 start_width, i16 start_height, PlatformState* platform_state);

/**
 * @brief Destroys the VkInstance and other things that need to be banished to the shadow realm.
 * @note This is called via function pointers in the renderer frontend.
 *
 * @param renderer_backend, A pointer to the RendererBackend object.
 */
void vulkan_backend_shutdown(RendererBackend* renderer_backend);

/**
 * @brief Starts the vulkan command buffer recording and rendering.
 * @note This is called via function pointers in the renderer frontend.
 *
 * @param renderer_backend, A pointer to the RendererBackend object.
 * @param delta_time,
 * @return TRUE if the process was started successfully, otherwise FALSE.
 */
b8 vulkan_backend_start_frame(RendererBackend* renderer_backend, f64 delta_time);

/**
 * @brief Ends the vulkan command buffer recording and rendering, ready to be presented.
 * @note This is called via function pointers in the renderer frontend.
 *
 * @param renderer_backend, A pointer to the RendererBackend object.
 * @param delta_time,
 * @return TRUE if the process was ended successfully, otherwise FALSE.
 */
b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time);

/**
 * @brief Presents the rendered images to the surface.
 * @note This is called via function pointers in the renderer frontend.
 *
 * @param renderer_backend, A pointer to the RendererBackend object.
 * @return TRUE if the image was presented successfully, otherwise FALSE
 */
b8 vulkan_backend_draw_frame(RendererBackend* renderer_backend);

/**
 * @brief Called by the renderer frontend whenever the window is resized.
 *
 * @param renderer_backend, A pointer to the RendererBackend object.
 * @param width, The width that the window was resized to.
 * @param height, The height that the window was resized to.
 * @return TRUE if the vulkan backend handled the resize successfully, otherwise FALSE.
 */
b8 vulkan_backend_on_resize(RendererBackend* renderer_backend, u16 width, u16 height);

// STATIC FUNCTIONS

// Init functions

static b8 vulkan_request_scene_data();

/**
 * @brief Creates the vulkan instance.
 *
 * @param application_name, The name of the application.
 * @return TRUE if the vulkan instance was created successfully, otherwise FALSE.
 */
static b8 vulkan_create_instance(const char* application_name);

/**
 * @brief Creates the vulkan debug message and assignes the callback used by the validation layer.
 *
 * @return TRUE if the setup was successful, otherwise FALSE.
 */
static b8 vulkan_create_debug_messenger();


/**
 * @brief Creates the vulkan swapchain.
 *
 * @param old_swapchain, A handle to the previous swapchain if the swapchain is being recreated,
 * otherwise VK_NULL_HANDLE.
 * @return TRUE if the swapchain was created successfully, otherwise FALSE.
 */
static b8 vulkan_create_swapchain(VkSwapchainKHR old_swapchain);

/**
 * @brief Retrives the images from the swapchain and stores them in vulkan_context.
 *
 * @return TRUE if the images were retrieved successfully, otherwise FALSE.
 */
static b8 vulkan_get_swapchain_images();

/**
 * @brief Creates the vulkan image views.
 *
 * @return TRUE if the image views were created successfully, otherwise FALSE.
 */
static b8 vulkan_create_image_views();

static b8 vulkan_create_descriptor_set_layout();


/**
 * @brief Creates the vulkan command pool
 *
 * @return TRUE if the command pool was created successfully, otherwise FALSE.
 */
static b8 vulkan_create_command_pools();

static b8 vulkan_create_drawable_objects();

static b8 vulkan_create_depth_resources();

static b8 vulkan_create_texture_image();

static b8 vulkan_create_texture_image_view();

static b8 vulkan_create_texture_image_sampler();

static b8 vulkan_create_vertex_buffers();

static b8 vulkan_create_index_buffer();

static b8 vulkan_create_uniform_buffers();

static b8 vulkan_create_descriptor_pool();

static b8 vulkan_create_descriptor_sets();

/**
 * @brief Allocated required vulkan command buffers.
 *
 * @return TRUE if command buffers could be allocated, otherwise FALSE.
 */
static b8 vulkan_allocate_command_buffers();

/**
 * @brief Creates the vulkan semaphores and fences needed for rendering.
 *
 * @return TRUE if the objects were created successfully, otherwise FALSE.
 */
static b8 vulkan_create_sync_primatives();

/**
 * @brief Creates a new swapchain based on the new infomation provided by the vulkan surface and
 * destroys the old swapchain.
 * @note width and height may not be used depending on the platform surface.
 *
 * @param width, The new width of the surface.
 * @param height, The new height of the surface
 * @return TRUE if swapchain was recreated successfully, otherwise FALSE.
 */
static b8 vulkan_recreate_swapchain(u16 width, u16 height);

/**
 * Unimplemented
 */
static b8 vulkan_cleanup_swapchain();

/**
 * @brief Translates vulkan debug callbacks to magnetar log messages and outputs them.
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                      VkDebugUtilsMessageTypeFlagsEXT message_types,
                      const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

static void transition_image_layout(VkImage image, VkImageLayout old_layout,
                                    VkImageLayout new_layout, VkAccessFlags2 src_access_mask,
                                    VkAccessFlags2 dst_access_mask,
                                    VkPipelineStageFlags2 src_stage_mask,
                                    VkPipelineStageFlags2 dst_stage_mask,
				    VkImageAspectFlags aspect);

static b8 create_buffer(VkBuffer* buffer,
			VkDeviceMemory* device_memory,
			VkDeviceSize size,
                        VkBufferUsageFlags usage_flags,
			VkMemoryPropertyFlags prop_flags);

static b8 copy_buffer(VkBuffer* src_buffer, VkBuffer* dst_buffer, VkDeviceSize size);



static u32 get_memory_type(u32 type_filter, VkMemoryPropertyFlags props);

static b8 update_uniform_buffer();

static b8 create_image(VkImage* image, VkDeviceMemory* mem, u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags props);

static b8 create_image_view(VkImageView* view, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, b8 rgb_flipped);

static VkCommandBuffer begin_single_time_commands(VkCommandPool command_pool);

static b8 end_single_time_commands(VkCommandBuffer* cmd_buf, VkQueue queue);

static b8 transition_tex_image_layout(VkCommandBuffer* cmd_buf, const VkImage* image, VkImageLayout old_layout, VkImageLayout new_layout);

static b8 copy_buffer_to_image(VkCommandBuffer* cmd_buf, VkBuffer* buf, VkImage* image, u32 width, u32 height);

static VkFormat find_supported_format(const VkFormat* formats, u32 format_count, VkImageTiling tiling, VkFormatFeatureFlags flags);

static VkFormat find_supported_depth_format();
