#include "vulkan_backend.h"

#include <stddef.h>
#include <vulkan/vk_enum_string_helper.h>

#include "core/log.h"
#include "core/mmemory.h"
#include "define.h"
#include "maths/maths_util.h"
#include "renderer/vulkan/vulkan_helper.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_defines.h"
#include "vulkan_wsi.h"

static VulkanContext vulkan_context = {};

static f32 x = 0;
static b8 toggle = TRUE;

b8 vulkan_backend_init(RendererBackend* renderer_backend, const char* application_name,
                       i16 start_width, i16 start_height, PlatformState* platform_state) {
  vulkan_context.allocator = NULL_PTR;
  vulkan_context.debug_messenger = NULL_PTR;
  vulkan_context.swapchain_extent.width = start_width;
  vulkan_context.swapchain_extent.height = start_height;

  // Test vertices
  // vulkan_context.vertex_count = 4;
  //  vulkan_context.vertices =
  //  mallocate(vulkan_context.vertex_count * sizeof(Vertex), MEMORY_TAG_RENDERER);
  //
  //  vulkan_context.vertices[0].position = (Vec3){-0.5f, -0.5f, 0.0f};
  //  vulkan_context.vertices[1].position = (Vec3){0.5f, -0.5f, 0.0f};
  //  vulkan_context.vertices[2].position = (Vec3){0.5f, 0.5f, 0.0f};
  //  vulkan_context.vertices[3].position = (Vec3){-0.5f, 0.5f, 0.0f};
  //
  //  vulkan_context.vertices[0].colour = (Vec3){1.0f, 0.0f, 0.0f};
  //  vulkan_context.vertices[1].colour = (Vec3){0.0f, 1.0f, 0.0f};
  //  vulkan_context.vertices[2].colour = (Vec3){0.0f, 0.0f, 1.0f};
  //  vulkan_context.vertices[3].colour = (Vec3){0.0f, 0.0f, 0.0f};
  //
  //  vulkan_context.index_count = 6;
  //  u32 indices[] = {0, 1, 2, 0, 2, 3};
  //

  vulkan_context.vertex_count = 8;
  vulkan_context.vertices =
      mallocate(vulkan_context.vertex_count * sizeof(Vertex), MEMORY_TAG_RENDERER);

  vulkan_context.vertices[0].position = (Vec3){-0.5f, -0.5f, -0.5f};
  vulkan_context.vertices[1].position = (Vec3){0.5f, -0.5f, -0.5f};
  vulkan_context.vertices[2].position = (Vec3){0.5f, 0.5f, -0.5f};
  vulkan_context.vertices[3].position = (Vec3){-0.5f, 0.5f, -0.5f};

  vulkan_context.vertices[4].position = (Vec3){-0.5f, -0.5f, 0.5f};
  vulkan_context.vertices[5].position = (Vec3){0.5f, -0.5f, 0.5f};
  vulkan_context.vertices[6].position = (Vec3){0.5f, 0.5f, 0.5f};
  vulkan_context.vertices[7].position = (Vec3){-0.5f, 0.5f, 0.5f};

  vulkan_context.vertices[0].colour = (Vec3){1, 0, 0};
  vulkan_context.vertices[1].colour = (Vec3){0, 1, 0};
  vulkan_context.vertices[2].colour = (Vec3){0, 0, 1};
  vulkan_context.vertices[3].colour = (Vec3){1, 1, 0};

  vulkan_context.vertices[4].colour = (Vec3){1, 0, 1};
  vulkan_context.vertices[5].colour = (Vec3){0, 1, 1};
  vulkan_context.vertices[6].colour = (Vec3){0.5f, 0.5f, 0.5f};
  vulkan_context.vertices[7].colour = (Vec3){1, 1, 1};
  vulkan_context.index_count = 36;

  u32 indices[] = {// back face (z = -0.5)
                   0, 1, 2, 2, 3, 0,

                   // front face (z = +0.5)
                   4, 6, 5, 6, 4, 7,

                   // left face
                   4, 0, 3, 3, 7, 4,

                   // right face
                   1, 5, 6, 6, 2, 1,

                   // bottom face
                   4, 5, 1, 1, 0, 4,

                   // top face
                   3, 2, 6, 6, 7, 3};

  vulkan_context.indices = mallocate(vulkan_context.index_count * sizeof(u32), MEMORY_TAG_RENDERER);

  mcopy_memory(vulkan_context.indices, indices, sizeof(indices));

  // CREATE INSTANCE ----------
  if (!vulkan_create_instance(application_name)) {
    MERROR_CORE("Vulkan Init: Failed to create instance.");
    return FALSE;
  }

  // CREATE DEBGUG MESSENGER ----------
  if (!vulkan_create_debug_messenger()) {
    MERROR_CORE("Vulkan Init: Failed to create debug messenger.");
    return FALSE;
  }

  // SELECT PHYSICAL DEVICE ----------
  if (!vulkan_select_physical_device()) {
    MERROR_CORE("Vulkan Init: Failed to select physical device.");
    return FALSE;
  }

  // GET SURFACE
  if (!vulkan_get_surface(platform_state)) {
    MERROR_CORE("Vulkan Init: Failed to create surface.");
    return FALSE;
  }

  // SETUP LOGICAL DEVICE ----------
  if (!vulkan_create_logical_device()) {
    MERROR_CORE("Vulkan Init: Failed to create logical device.");
    return FALSE;
  }

  // CREATE SWAPCHAIN ----------
  if (!vulkan_create_swapchain(VK_NULL_HANDLE)) {
    MERROR_CORE("Vulkan Init: Failed to create swapchain.");
    return FALSE;
  }

  // GET SWAPCHAIN IMAGES ----------
  if (!vulkan_get_swapchain_images()) {
    MERROR_CORE("Vulkan Init: Failed to get swapchain images.");
    return FALSE;
  }

  // CREATE IMAGE VIEWS ----------
  if (!vulkan_create_image_views()) {
    MERROR_CORE("Vulkan Init: Failed to create image views.");
    return FALSE;
  }

  // CREATE DESCRIPTOR SET LAYOUT ----------
  if (!vulkan_create_descriptor_set_layout()) {
    MERROR_CORE("Vulkan Init: Failed to create descriptor set layout.");
    return FALSE;
  }

  // CREATE DESCRIPTOR POOL ----------
  if (!vulkan_create_descriptor_pool()) {
    MERROR_CORE("Vulkan Init: Failed to create descriptor pool");
    return FALSE;
  }

  // CREATE UNIFORM BUFFERS ----------
  if (!vulkan_create_uniform_buffers()) {
    MERROR_CORE("Vulkan Init: Failed to create uniform buffers.");
    return FALSE;
  }

  // CREATE DESCRIPTOR SETS ----------
  if (!vulkan_create_descriptor_sets()) {
    MERROR_CORE("Vukan Init: Failed to create descriptor sets");
    return FALSE;
  }

  // CREATE GRAPHICS PIPELINE ----------
  if (!vulkan_create_graphics_pipeline()) {
    MERROR_CORE("Vulkan Init: Failed to create graphics pipeline.");
    return FALSE;
  }

  // CREATE COMMAND POOLS ----------
  if (!vulkan_create_command_pools()) {
    MERROR_CORE("Vulkan Init: Failed to create command pool.");
    return FALSE;
  }

  // CREATE VERTEX BUFFERS ----------
  if (!vulkan_create_vertex_buffers()) {
    MERROR_CORE("Vulkan Init: Failed to create vertex buffers.");
    return FALSE;
  }

  // CREATE INDEX BUFFER ----------
  if (!vulkan_create_index_buffer()) {
    MERROR_CORE("Vulkan Init: Failed to create index buffer.");
    return FALSE;
  }

  // ALLOC COMMAND BUFFERS ----------
  if (!vulkan_allocate_command_buffers()) {
    MERROR_CORE("Vulkan Init: Failed to allocate command buffers.");
    return FALSE;
  }

  // CREATE SYNC PRIMS ----------
  if (!vulkan_create_sync_primatives()) {
    MERROR_CORE("Vulkan Init: Failed to create syncronisation objects.");
    return FALSE;
  }

  // Get device queue for rendering
  vkGetDeviceQueue(vulkan_context.logical_device, vulkan_context.graphics_queue_family_index, 0,
                   &vulkan_context.graphics_queue);

  vkGetDeviceQueue(vulkan_context.logical_device, vulkan_context.transfer_queue_family_index, 0,
                   &vulkan_context.transfer_queue);

  vulkan_context.current_image_index = 0;
  vulkan_context.current_frame_index = 0;
  return TRUE;
}

void vulkan_backend_shutdown(RendererBackend* renderer_backend) {
  VkResult queue_wait_idle_result = vkQueueWaitIdle(vulkan_context.graphics_queue);
  if (queue_wait_idle_result != VK_SUCCESS) {
    MERROR_CORE("Failed to wait for vulkan queue to idle before shutting down: %s",
                string_VkResult(queue_wait_idle_result));
  }

  mfree(vulkan_context.render_complete_semaphores,
        vulkan_context.swapchain_image_count * sizeof(VkSemaphore), MEMORY_TAG_RENDERER);

  vkDestroyCommandPool(vulkan_context.logical_device, vulkan_context.graphics_command_pool,
                       vulkan_context.allocator);
  vkDestroyPipeline(vulkan_context.logical_device, vulkan_context.graphics_pipeline,
                    vulkan_context.allocator);
  vkDestroySwapchainKHR(vulkan_context.logical_device, vulkan_context.swapchain,
                        vulkan_context.allocator);

  mfree(vulkan_context.swapchain_image_views,
        vulkan_context.swapchain_image_count * sizeof(VkImageView), MEMORY_TAG_RENDERER);
  mfree(vulkan_context.swapchain_images, vulkan_context.swapchain_image_count * sizeof(VkImage),
        MEMORY_TAG_RENDERER);

  mfree(vulkan_context.debug_messenger, sizeof(VkDebugUtilsMessengerEXT), MEMORY_TAG_RENDERER);

  PFN_vkDestroyDebugUtilsMessengerEXT fp_vkDestroyDebugUtilsMessengerEXT =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan_context.instance,
                                                                 "vkDestroyDebugUtilsMessengerEXT");
  fp_vkDestroyDebugUtilsMessengerEXT(vulkan_context.instance, *vulkan_context.debug_messenger,
                                     vulkan_context.allocator);

  vkDeviceWaitIdle(vulkan_context.logical_device);
  vkDestroyDevice(vulkan_context.logical_device, vulkan_context.allocator);
  vkDestroyInstance(vulkan_context.instance, vulkan_context.allocator);
}

b8 vulkan_backend_start_frame(RendererBackend* renderer_backend, f64 delta_time) {
  VkResult wait_for_fence_result = vkWaitForFences(
      vulkan_context.logical_device, 1,
      &vulkan_context.draw_fences[vulkan_context.current_frame_index], VK_TRUE, UINT64_MAX);

  if (wait_for_fence_result != VK_SUCCESS) {
    MERROR_CORE("Failed to wait for vulkan draw fence: %s", string_VkResult(wait_for_fence_result));
    return FALSE;
  }

  VkResult reset_fence_result =
      vkResetFences(vulkan_context.logical_device, 1,
                    &vulkan_context.draw_fences[vulkan_context.current_frame_index]);

  if (reset_fence_result != VK_SUCCESS) {
    MERROR_CORE("Failed to reset vulkan draw fence: %s", string_VkResult(reset_fence_result));
    return FALSE;
  }

  VkResult reset_command_buffer_result = vkResetCommandBuffer(
      vulkan_context.command_buffers[vulkan_context.current_frame_index], ZERO);
  if (reset_command_buffer_result != VK_SUCCESS) {
    MERROR_CORE("Failed to reset vulkan command buffer: %s",
                string_VkResult(reset_command_buffer_result));
    return FALSE;
  }

  VkResult acquire_next_image_result = vkAcquireNextImageKHR(
      vulkan_context.logical_device, vulkan_context.swapchain, UINT64_MAX,
      vulkan_context.present_complete_semaphores[vulkan_context.current_frame_index],
      VK_NULL_HANDLE, &vulkan_context.current_image_index);

  if (acquire_next_image_result == VK_SUBOPTIMAL_KHR) {
    MWARN_CORE("Vulkan acquire next image result: %s", string_VkResult(acquire_next_image_result));
  } else if (acquire_next_image_result != VK_SUCCESS) {
    MERROR_CORE("Failed to acquire next image on vulkan swapchain: %s",
                string_VkResult(acquire_next_image_result));
    return FALSE;
  }

  // Begin command buffer recording
  VkCommandBufferBeginInfo command_buffer_begin_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  command_buffer_begin_info.flags = ZERO;
  command_buffer_begin_info.pInheritanceInfo = NULL_PTR;

  VkResult begin_command_buffer_result =
      vkBeginCommandBuffer(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                           &command_buffer_begin_info);
  if (begin_command_buffer_result != VK_SUCCESS) {
    MERROR_CORE("Failed to begin vulkan command buffer: %s",
                string_VkResult(begin_command_buffer_result));
    return FALSE;
  }

  transition_image_layout(vulkan_context.current_image_index, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, ZERO,
                          VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);

  VkClearColorValue clear_color_value = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}};
  VkClearValue clear_color;
  clear_color.color = clear_color_value;

  VkRenderingAttachmentInfo rendering_attachment_info = {
      VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
  rendering_attachment_info.imageView =
      vulkan_context.swapchain_image_views[vulkan_context.current_image_index];
  rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  rendering_attachment_info.clearValue = clear_color;

  VkOffset2D render_area_offset;
  render_area_offset.x = 0;
  render_area_offset.y = 0;

  VkExtent2D render_extent;

  VkRect2D render_area;
  render_area.offset = render_area_offset;
  render_area.extent = vulkan_context.swapchain_extent;

  VkRenderingInfo rendering_info = {VK_STRUCTURE_TYPE_RENDERING_INFO};
  rendering_info.renderArea = render_area;
  rendering_info.layerCount = 1;
  rendering_info.colorAttachmentCount = 1;
  rendering_info.pColorAttachments = &rendering_attachment_info;

  // Start rendering
  vkCmdBeginRendering(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                      &rendering_info);
  vkCmdBindPipeline(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                    VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_context.graphics_pipeline);

  // Set viewport as scissor values
  VkViewport viewport = {.x = 0.0f,
                         .y = 0.0f,
                         .width = (f32)vulkan_context.swapchain_extent.width,
                         .height = (f32)vulkan_context.swapchain_extent.height,
                         .minDepth = 0.0f,
                         .maxDepth = 1.0f};

  vkCmdSetViewport(vulkan_context.command_buffers[vulkan_context.current_frame_index], 0, 1,
                   &viewport);

  VkRect2D scissor = {.offset = {.x = 0, .y = 0}, .extent = vulkan_context.swapchain_extent};

  vkCmdSetScissor(vulkan_context.command_buffers[vulkan_context.current_frame_index], 0, 1,
                  &scissor);

  VkDeviceSize temp_offsets[] = {0};  // @TODO: stuff
  vkCmdBindVertexBuffers(vulkan_context.command_buffers[vulkan_context.current_frame_index], 0, 1,
                         &vulkan_context.vertex_buffer, temp_offsets);
  vkCmdBindIndexBuffer(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                       vulkan_context.index_buffer, 0, VK_INDEX_TYPE_UINT32);

  vkCmdBindDescriptorSets(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                          VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_context.pipeline_layout, 0, 1,
                          &vulkan_context.descriptor_sets[vulkan_context.current_frame_index], 0,
                          NULL_PTR);

  vkCmdDrawIndexed(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                   vulkan_context.index_count, 1, 0, 0, 0);

  return TRUE;
}

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time) {
  // End rendering
  vkCmdEndRendering(vulkan_context.command_buffers[vulkan_context.current_frame_index]);

  transition_image_layout(
      vulkan_context.current_image_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, ZERO,
      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

  // End command buffer recording
  VkResult end_command_buffer_result =
      vkEndCommandBuffer(vulkan_context.command_buffers[vulkan_context.current_frame_index]);
  if (end_command_buffer_result != VK_SUCCESS) {
    MERROR_CORE("Failed to end vulkan command buffer: %s",
                string_VkResult(end_command_buffer_result));
    return FALSE;
  }

  VkPipelineStageFlags wait_dst_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  // @TODO: Update Uniform buffer
  update_uniform_buffer();

  VkSubmitInfo command_buffer_submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  command_buffer_submit_info.pNext = NULL_PTR;
  command_buffer_submit_info.waitSemaphoreCount = 1;
  command_buffer_submit_info.pWaitSemaphores =
      &vulkan_context.present_complete_semaphores[vulkan_context.current_frame_index];
  command_buffer_submit_info.pWaitDstStageMask = &wait_dst_stage_flags;
  command_buffer_submit_info.commandBufferCount = 1;
  command_buffer_submit_info.pCommandBuffers =
      &vulkan_context.command_buffers[vulkan_context.current_frame_index];
  command_buffer_submit_info.signalSemaphoreCount = 1;
  command_buffer_submit_info.pSignalSemaphores =
      &vulkan_context.render_complete_semaphores[vulkan_context.current_image_index];

  VkResult queue_submit_restult =
      vkQueueSubmit(vulkan_context.graphics_queue, 1, &command_buffer_submit_info,
                    vulkan_context.draw_fences[vulkan_context.current_frame_index]);

  if (queue_submit_restult != VK_SUCCESS) {
    MERROR_CORE("Failed to sumbit to vulkan queue: %s", string_VkResult(queue_submit_restult));
    return FALSE;
  }

  return TRUE;
}

b8 vulkan_backend_draw_frame(RendererBackend* renderer_backend) {
  VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores =
      &vulkan_context.render_complete_semaphores[vulkan_context.current_image_index];
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &vulkan_context.swapchain;
  present_info.pImageIndices = &vulkan_context.current_image_index;

  VkResult present_swapchain_result =
      vkQueuePresentKHR(vulkan_context.graphics_queue, &present_info);
  if (present_swapchain_result == VK_SUBOPTIMAL_KHR) {
    MWARN_CORE("Vulkan present swapchain result: %s", string_VkResult(present_swapchain_result));
  } else if (present_swapchain_result != VK_SUCCESS) {
    MERROR_CORE("Failed to present vulkan swapchain: %s",
                string_VkResult(present_swapchain_result));
    return FALSE;
  }

  vulkan_context.current_frame_index =
      (vulkan_context.current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;

  return TRUE;
}

b8 vulkan_backend_on_resize(RendererBackend* renderer_backend, u16 width, u16 height) {
  /* MTRACE_CORE("vk resize: (%u, %u)", width, height); */
  if (!vulkan_recreate_swapchain(width, height)) {
    MERROR_CORE("Vulkan On Resize: Failed to recreate swapchain.");
    return FALSE;
  }
  return TRUE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                      VkDebugUtilsMessageTypeFlagsEXT message_types,
                      const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
  switch (message_severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      MERROR_CORE("VULKAN DEBUG CALLBACK [%i: %s]: %s", callback_data->messageIdNumber,
                  callback_data->pMessageIdName, callback_data->pMessage);
      break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      MWARN_CORE("VULKAN DEBUG CALLBACK [%i: %s]: %s", callback_data->messageIdNumber,
                 callback_data->pMessageIdName, callback_data->pMessage);
      break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      MINFO_CORE("VULKAN DEBUG CALLBACK [%i: %s]: %s", callback_data->messageIdNumber,
                 callback_data->pMessageIdName, callback_data->pMessage);
      break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      MTRACE_CORE("VULKAN DEBUG CALLBACK [%i: %s]: %s", callback_data->messageIdNumber,
                  callback_data->pMessageIdName, callback_data->pMessage);
      break;

    default:
      MDEBUG_CORE("UNKNOWN VULKAN DEBUG CALLBACK [%i: %s]: %s", callback_data->messageIdNumber,
                  callback_data->pMessageIdName, callback_data->pMessage);
      break;
  }

  return VK_FALSE;
}

static b8 vulkan_create_instance(const char* application_name) {
  VkApplicationInfo application_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  application_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  application_info.apiVersion = VK_API_VERSION_1_4;
  application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  application_info.pApplicationName = application_name;
  application_info.pEngineName = "Magnetar Engine";

  // Set vulkan instance create info
  VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  const char* const instance_extension_names[] = MVK_INSTANCE_EXTENSION_NAMES;
  const char* const layer_names[] = MVK_LAYER_NAMES;
  create_info.pApplicationInfo = &application_info;
  create_info.enabledExtensionCount = MVK_INSTANCE_EXTENSION_COUNT;
  create_info.enabledLayerCount = MVK_LAYER_COUNT;
  create_info.ppEnabledExtensionNames = instance_extension_names;
  create_info.ppEnabledLayerNames = layer_names;
  create_info.flags = MVK_INSTANCE_CREATE_FLAGS;

  VkResult res_create_instance =
      vkCreateInstance(&create_info, vulkan_context.allocator, &vulkan_context.instance);

  // Check for instance creation errors
  if (res_create_instance == VK_SUCCESS) {
    MINFO_CORE("Created VkInstance");
    u32 api_version = application_info.apiVersion;
    MINFO_CORE("Vulkan API version: %u.%u.%u", VK_API_VERSION_MAJOR(api_version),
               VK_API_VERSION_MINOR(api_version), VK_API_VERSION_PATCH(api_version));
  } else {
    MERROR_CORE("Failed to create VkInstance: %s", string_VkResult(res_create_instance));
    return FALSE;
  }
  return TRUE;
}

static b8 vulkan_create_debug_messenger() {
  VkDebugUtilsMessageSeverityFlagsEXT debug_message_severity_flags =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

  // Set which type of events will call the debug messenger
  VkDebugUtilsMessageTypeFlagsEXT debug_message_type_flags =
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

  VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info = {
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
  debug_messenger_create_info.flags = ZERO;
  debug_messenger_create_info.messageSeverity = debug_message_severity_flags;
  debug_messenger_create_info.messageType = debug_message_type_flags;
  debug_messenger_create_info.pfnUserCallback = &vulkan_debug_callback;
  debug_messenger_create_info.pUserData = NULL_PTR;

  // Load function
  PFN_vkCreateDebugUtilsMessengerEXT fp_vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan_context.instance,
                                                                "vkCreateDebugUtilsMessengerEXT");
  // Create debug messenger
  vulkan_context.debug_messenger = mallocate(sizeof(VkDebugUtilsMessengerEXT), MEMORY_TAG_RENDERER);
  VkResult create_debug_utils_messenger_result =
      fp_vkCreateDebugUtilsMessengerEXT(vulkan_context.instance, &debug_messenger_create_info,
                                        vulkan_context.allocator, vulkan_context.debug_messenger);

  if (create_debug_utils_messenger_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan debug messenger: %s",
                string_VkResult(create_debug_utils_messenger_result));
    return FALSE;
  }
  return TRUE;
}

static b8 vulkan_select_physical_device() {
  u32 physical_device_count = 0;
  VkResult enumerate_physical_devices_result =
      vkEnumeratePhysicalDevices(vulkan_context.instance, &physical_device_count, NULL_PTR);
  if (enumerate_physical_devices_result != VK_SUCCESS) {
    MERROR_CORE("Failed to retrieve Vulkan physical device count: %s",
                string_VkResult(enumerate_physical_devices_result));
    return FALSE;
  }
  MDEBUG_CORE("Physical device count: %u", physical_device_count);

  // Get list of physical devices
  VkPhysicalDevice* physical_devices =
      mallocate(physical_device_count * sizeof(VkPhysicalDevice), MEMORY_TAG_RENDERER);

  enumerate_physical_devices_result =
      vkEnumeratePhysicalDevices(vulkan_context.instance, &physical_device_count, physical_devices);
  if (enumerate_physical_devices_result != VK_SUCCESS) {
    MERROR_CORE("Failed to retrieve Vulkan physical devices: %s",
                string_VkResult(enumerate_physical_devices_result));
    return FALSE;
  }

  // List physical devices and select suitable device to use
  // @TODO: Select most suitable device more accurately
  VkPhysicalDeviceProperties targeted_physical_device_properties;
  VkPhysicalDeviceProperties suitable_physical_device_properties;
  VkPhysicalDevice suitable_device = physical_devices[0];  // @TODO: Add this to vulkan_context
  for (u32 i = 0; i < physical_device_count; i++) {
    vkGetPhysicalDeviceProperties(physical_devices[i], &targeted_physical_device_properties);
    vkGetPhysicalDeviceProperties(suitable_device, &suitable_physical_device_properties);
    MDEBUG_CORE("%u. Physical Device: %s, %s", i + 1,
                string_VkPhysicalDeviceType(targeted_physical_device_properties.deviceType),
                targeted_physical_device_properties.deviceName);
    if (targeted_physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      suitable_device = physical_devices[i];
    } else if (targeted_physical_device_properties.deviceType ==
                   VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
               suitable_physical_device_properties.deviceType !=
                   VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      suitable_device = physical_devices[i];
    }
  }
  vulkan_context.physical_device = suitable_device;

  mfree(physical_devices, physical_device_count * sizeof(VkPhysicalDevice), MEMORY_TAG_RENDERER);
  return TRUE;
}

static b8 vulkan_get_surface(PlatformState* platform_state) {
  if (!vulkan_create_platform_surface(&vulkan_context, platform_state)) {
    MERROR_CORE("Failed to create vulkan platform surface");
    return FALSE;
  }

  if (!vulkan_set_surface_extent(vulkan_context.swapchain_extent.width,
                                 vulkan_context.swapchain_extent.height)) {
    MERROR_CORE("Vulkan Get Surface: Failed to set surface extent.");
    return FALSE;
  }

  // Get surface formats
  u32 surface_format_count = 0;
  VkResult get_physical_device_surface_formats_count_result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      vulkan_context.physical_device, vulkan_context.surface, &surface_format_count, NULL_PTR);
  if (get_physical_device_surface_formats_count_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get vulkan physical device surface format count: %s",
                string_VkResult(get_physical_device_surface_formats_count_result));
    return FALSE;
  }
  VkSurfaceFormatKHR surface_formats[surface_format_count];
  VkResult get_physical_device_surface_formats_result =
      vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_context.physical_device, vulkan_context.surface,
                                           &surface_format_count, surface_formats);
  if (get_physical_device_surface_formats_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get vulkan physical device surface formats: %s",
                string_VkResult(get_physical_device_surface_formats_result));
    return FALSE;
  }

  MTRACE_CORE("Surface formats:");
  for (u32 i = 0; i < surface_format_count; i++) {
    MTRACE_CORE("%u: Format: %s, Colour space: %s", i + 1,
                string_VkFormat(surface_formats[i].format),
                string_VkColorSpaceKHR(surface_formats[i].colorSpace));
  }
  // The index in surface_formats to be used in swapchain creation
  //@TODO: Maybe implement some sort of seletion algorithm?
  const u32 selected_format = 0;  // @MAGIC_NUMBER
  vulkan_context.surface_format = surface_formats[selected_format];
  return TRUE;
}

static b8 vulkan_set_surface_extent(i16 width, i16 height) {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkResult get_physical_device_surface_capabilities_result =
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_context.physical_device,
                                                vulkan_context.surface, &surface_capabilities);

  if (get_physical_device_surface_capabilities_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get physical device surface capabilities: %s",
                string_VkResult(get_physical_device_surface_capabilities_result));
    return FALSE;
  }

  // Sets the image extent to an acceptable value
  if (surface_capabilities.currentExtent.width != UINT32_MAX && surface_capabilities.currentExtent.width != 0) {  // Surface provides specific size
    vulkan_context.swapchain_extent = surface_capabilities.currentExtent;
  } else {  // Application can provide size
    // Set width within acceptable Image Extent range
    if ((u32)width > surface_capabilities.maxImageExtent.width) {
      vulkan_context.swapchain_extent.width = surface_capabilities.maxImageExtent.width;
    } else if ((u32)width < surface_capabilities.minImageExtent.width) {
      vulkan_context.swapchain_extent.width = surface_capabilities.minImageExtent.width;
    } else {
      vulkan_context.swapchain_extent.width = (u32)width;
    }

    // Set width within acceptable Image Extent range
    if ((u32)height > surface_capabilities.maxImageExtent.height) {
      vulkan_context.swapchain_extent.height = surface_capabilities.maxImageExtent.height;
    } else if ((u32)height < surface_capabilities.minImageExtent.height) {
      vulkan_context.swapchain_extent.height = surface_capabilities.minImageExtent.height;
    } else {
      vulkan_context.swapchain_extent.height = (u32)height;
    }
  }
  return TRUE;
}

static b8 vulkan_create_logical_device() {
  u32 queue_family_properties_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(vulkan_context.physical_device,
                                           &queue_family_properties_count, NULL_PTR);
  VkQueueFamilyProperties queue_family_properties[queue_family_properties_count];
  vkGetPhysicalDeviceQueueFamilyProperties(vulkan_context.physical_device,
                                           &queue_family_properties_count, queue_family_properties);

  VkBool32 queue_family_supported = VK_FALSE;
  for (u32 i = 0; i < queue_family_properties_count; i++) {
    VkResult get_physical_device_surface_support_result = vkGetPhysicalDeviceSurfaceSupportKHR(
        vulkan_context.physical_device, i, vulkan_context.surface, &queue_family_supported);

    if (get_physical_device_surface_support_result != VK_SUCCESS) {
      MERROR_CORE("Failed to get vulkan physical device surface support: %s",
                  string_VkResult(get_physical_device_surface_support_result));
      return FALSE;
    }

    if ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && queue_family_supported) {
      vulkan_context.graphics_queue_family_index = i;
      break;
    }
  }
  if (queue_family_supported == VK_FALSE) {
    MERROR_CORE("Failed to find suitable queue family on vulkan physical device.");
    return FALSE;
  }

  for (u32 i = 0; i < queue_family_properties_count; i++) {
    VkQueueFlags flags = queue_family_properties[i].queueFlags;

    // Prefer dedicated transfer queue
    if ((flags & VK_QUEUE_TRANSFER_BIT) && !(flags & VK_QUEUE_GRAPHICS_BIT)) {
      vulkan_context.transfer_queue_family_index = i;
      queue_family_supported = VK_TRUE;
      break;
    }
  }

  /* queue_family_supported = VK_FALSE; */
  /* for (u32 i = 0; i < queue_family_properties_count; i++) { */
  /*   VkResult get_physical_device_surface_support_result = vkGetPhysicalDeviceSurfaceSupportKHR(
   */
  /*       vulkan_context.physical_device, i, vulkan_context.surface, &queue_family_supported); */

  /*   if (get_physical_device_surface_support_result != VK_SUCCESS) { */
  /*     MERROR_CORE("Failed to get vulkan physical device surface support: %s", */
  /*                 string_VkResult(get_physical_device_surface_support_result)); */
  /*     return FALSE; */
  /*   } */

  /*   if ((queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
   * queue_family_supported) { */
  /*     vulkan_context.transfer_queue_family_index = i; */
  /*     break; */
  /*   } */
  /* } */
  /* if (queue_family_supported == VK_FALSE) { */
  /*   MERROR_CORE("Failed to find suitable queue family on vulkan physical device."); */
  /*   return FALSE; */
  /* } */

  // Set queue create info

  f32 temp_priority = 1.0f;  // @MAGIC_NUMBER
  VkDeviceQueueCreateInfo device_graphics_queue_create_info = {
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  device_graphics_queue_create_info.queueFamilyIndex = vulkan_context.graphics_queue_family_index;
  device_graphics_queue_create_info.queueCount = 1;  // @MAGIC_NUMBER
  device_graphics_queue_create_info.pQueuePriorities = &temp_priority;

#if defined(MPLATFORM_APPLE)

  vulkan_context.transfer_queue_family_index = vulkan_context.graphics_queue_family_index;

  vulkan_context.queue_family_index_count = 1;
  VkDeviceQueueCreateInfo device_queue_create_infos[] = {device_graphics_queue_create_info};

#else

  VkDeviceQueueCreateInfo device_transfer_queue_create_info = {
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  device_transfer_queue_create_info.queueFamilyIndex = vulkan_context.transfer_queue_family_index;
  device_transfer_queue_create_info.queueCount = 1;  // @MAGIC_NUMBER
  device_transfer_queue_create_info.pQueuePriorities = &temp_priority;

  vulkan_context.queue_family_index_count = 2;
  VkDeviceQueueCreateInfo device_queue_create_infos[] = {device_graphics_queue_create_info,
                                                         device_transfer_queue_create_info};
#endif

  // Enable vulkan11 features
  VkPhysicalDeviceVulkan11Features physical_device_vulkan_11_features = {
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
  physical_device_vulkan_11_features.shaderDrawParameters = VK_TRUE;

  // Enable vulkan13 features
  VkPhysicalDeviceVulkan13Features physical_device_vulkan_13_features = {
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
  physical_device_vulkan_13_features.pNext = &physical_device_vulkan_11_features;
  physical_device_vulkan_13_features.dynamicRendering = VK_TRUE;
  physical_device_vulkan_13_features.synchronization2 = VK_TRUE;

  // Set logical device create info
  const char* const device_extension_names[] = MVK_DEVICE_EXTENSION_NAMES;
  VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  device_create_info.pNext = &physical_device_vulkan_13_features;
  device_create_info.queueCreateInfoCount = vulkan_context.queue_family_index_count;
  device_create_info.pQueueCreateInfos = device_queue_create_infos;
  device_create_info.pEnabledFeatures = NULL_PTR;
  device_create_info.enabledExtensionCount = MVK_DEVICE_EXTENSION_COUNT;
  device_create_info.ppEnabledExtensionNames = device_extension_names;

  // Create logical device
  VkResult create_device_result =
      vkCreateDevice(vulkan_context.physical_device, &device_create_info, vulkan_context.allocator,
                     &vulkan_context.logical_device);

  if (create_device_result == VK_SUCCESS) {
    MINFO_CORE("Vulkan logical device created");
  } else {
    MERROR_CORE("Failed to create vulkan logical device: %s",
                string_VkResult(create_device_result));
    return FALSE;
  }
  vkGetDeviceQueue(vulkan_context.logical_device, vulkan_context.graphics_queue_family_index, 0,
                   &vulkan_context.graphics_queue);

  vkGetDeviceQueue(vulkan_context.logical_device, vulkan_context.transfer_queue_family_index, 0,
                   &vulkan_context.transfer_queue);
  return TRUE;
}

static b8 vulkan_create_swapchain(VkSwapchainKHR old_swapchain) {

  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkResult get_physical_device_surface_capabilities_result =
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_context.physical_device,
                                                vulkan_context.surface, &surface_capabilities);

  if (get_physical_device_surface_capabilities_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get physical device surface capabilities: %s",
                string_VkResult(get_physical_device_surface_capabilities_result));
    return FALSE;
  }

  VkImageUsageFlags image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  swapchain_create_info.pNext = NULL_PTR;
  swapchain_create_info.surface = vulkan_context.surface;
  swapchain_create_info.minImageCount = surface_capabilities.minImageCount;  // @TODO: Add this as a setting in user code. (Maybe)
  swapchain_create_info.imageFormat = vulkan_context.surface_format.format;
  swapchain_create_info.imageColorSpace = vulkan_context.surface_format.colorSpace;
  swapchain_create_info.imageExtent = vulkan_context.swapchain_extent;
  swapchain_create_info.imageArrayLayers = 1;  // @MAGIC_NUMBER
  swapchain_create_info.imageUsage = image_usage_flags;
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.queueFamilyIndexCount = ZERO;
  swapchain_create_info.pQueueFamilyIndices = NULL_PTR;
  swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
  swapchain_create_info.clipped = VK_TRUE;
  swapchain_create_info.oldSwapchain = old_swapchain;

  VkResult create_swapchain_result =
      vkCreateSwapchainKHR(vulkan_context.logical_device, &swapchain_create_info,
                           vulkan_context.allocator, &vulkan_context.swapchain);

  if (create_swapchain_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create Vulkan swapchain: %s", string_VkResult(create_swapchain_result));
    return FALSE;
  } else {
    /* MINFO_CORE("Created Vulkan swapchain"); */
  }
  return TRUE;
}

static b8 vulkan_get_swapchain_images() {
  vulkan_context.swapchain_image_count = 0;
  VkResult get_swapchain_image_count_result =
      vkGetSwapchainImagesKHR(vulkan_context.logical_device, vulkan_context.swapchain,
                              &vulkan_context.swapchain_image_count, NULL_PTR);
  if (get_swapchain_image_count_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get vulkan swapchain image count: %s",
                string_VkResult(get_swapchain_image_count_result));
    return FALSE;
  }
  vulkan_context.swapchain_images =
      mallocate(vulkan_context.swapchain_image_count * sizeof(VkImage), MEMORY_TAG_RENDERER);
  VkResult get_swapchain_images_result = vkGetSwapchainImagesKHR(
      vulkan_context.logical_device, vulkan_context.swapchain,
      &vulkan_context.swapchain_image_count, vulkan_context.swapchain_images);
  if (get_swapchain_images_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get vulkan swapchain images: %s",
                string_VkResult(get_swapchain_images_result));
    return FALSE;
  }
  return TRUE;
}

static b8 vulkan_create_image_views() {
  vulkan_context.swapchain_image_views =
      mallocate(vulkan_context.swapchain_image_count * sizeof(VkImageView), MEMORY_TAG_RENDERER);
  for (u32 i = 0; i < vulkan_context.swapchain_image_count; i++) {
    VkImageViewCreateFlags image_view_create_flags = ZERO;

    VkComponentMapping component_mapping;
    component_mapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    component_mapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    component_mapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    component_mapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageAspectFlags image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;

    VkImageSubresourceRange image_subresource_range;
    image_subresource_range.aspectMask = image_aspect_flags;
    // @TODO: Figure out wwhat this means
    image_subresource_range.levelCount = 1;      // @MAGIC_NUMBER
    image_subresource_range.baseMipLevel = 0;    // @MAGIC_NUMBER (Very magic number)
    image_subresource_range.layerCount = 1;      // @MAGIC_NUMBER
    image_subresource_range.baseArrayLayer = 0;  // @MAGIC_NUMBER (Very magic number)

    VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    image_view_create_info.pNext = NULL_PTR;
    image_view_create_info.flags = image_view_create_flags;
    image_view_create_info.image = vulkan_context.swapchain_images[i];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = vulkan_context.surface_format.format;
    image_view_create_info.components = component_mapping;
    image_view_create_info.subresourceRange = image_subresource_range;

    VkResult create_image_view_result =
        vkCreateImageView(vulkan_context.logical_device, &image_view_create_info,
                          vulkan_context.allocator, &vulkan_context.swapchain_image_views[i]);

    if (create_image_view_result != VK_SUCCESS) {
      MERROR_CORE("Failed to create vulkan swapchain image view: %s",
                  string_VkResult(create_image_view_result));
      return FALSE;
    }
  }
  return TRUE;
}

static b8 vulkan_create_descriptor_set_layout() {
  VkDescriptorSetLayoutBinding mvp_layout_binding = {};
  mvp_layout_binding.binding = 0;
  mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  mvp_layout_binding.descriptorCount = 1;
  mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  descriptor_set_layout_create_info.bindingCount = 1;
  descriptor_set_layout_create_info.pBindings = &mvp_layout_binding;

  VkResult create_descriptor_set_layout_res =
      vkCreateDescriptorSetLayout(vulkan_context.logical_device, &descriptor_set_layout_create_info,
                                  vulkan_context.allocator, &vulkan_context.descriptor_set_layout);
  if (create_descriptor_set_layout_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create descriptor set layout: %s",
                string_VkResult(create_descriptor_set_layout_res));
    return FALSE;
  }

  return TRUE;
}

static b8 vulkan_create_graphics_pipeline() {
  // Read shader byte code
  u8* shader_bin = NULL_PTR;
  u64 shader_bin_size = 0;
  vulkan_read_shader_binary("../engine/src/renderer/vulkan/shaders/basic.spv", &shader_bin_size,
                            &shader_bin);

  // Create shader module
  VkShaderModuleCreateInfo shader_module_create_info = {
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  shader_module_create_info.pNext = NULL_PTR;
  shader_module_create_info.codeSize = shader_bin_size;
  shader_module_create_info.pCode = (u32*)shader_bin;

  VkShaderModule shader_module;

  VkResult create_shader_module_result =
      vkCreateShaderModule(vulkan_context.logical_device, &shader_module_create_info,
                           vulkan_context.allocator, &shader_module);
  if (create_shader_module_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan shader module: %s",
                string_VkResult(create_shader_module_result));
    return FALSE;
  }

  mfree(shader_bin, shader_bin_size * sizeof(u8), MEMORY_TAG_RENDERER);

  // Create vertex shader stage create info
  VkPipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  vert_pipeline_shader_stage_create_info.pNext = NULL_PTR;
  vert_pipeline_shader_stage_create_info.flags = ZERO;
  vert_pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_pipeline_shader_stage_create_info.module = shader_module;
  vert_pipeline_shader_stage_create_info.pName = "vertMain";
  vert_pipeline_shader_stage_create_info.pSpecializationInfo = NULL_PTR;

  // Create fragment shader stage create info
  VkPipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  frag_pipeline_shader_stage_create_info.pNext = NULL_PTR;
  frag_pipeline_shader_stage_create_info.flags = ZERO;
  frag_pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_pipeline_shader_stage_create_info.module = shader_module;
  frag_pipeline_shader_stage_create_info.pName = "fragMain";
  frag_pipeline_shader_stage_create_info.pSpecializationInfo = NULL_PTR;

  VkPipelineShaderStageCreateInfo shader_stage_create_infos[] = {
      vert_pipeline_shader_stage_create_info, frag_pipeline_shader_stage_create_info};

  // Set dynamic states
  VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
  dynamic_state_create_info.pNext = NULL_PTR;
  dynamic_state_create_info.dynamicStateCount = sizeof(dynamic_states) / sizeof(VkDynamicState);
  dynamic_state_create_info.pDynamicStates = dynamic_states;

  // Set vertex input state create info
  VkVertexInputBindingDescription vertex_binding_description = get_vertex_binding_description();
  VkVertexInputAttributeDescription* vertex_attribute_descriptions =
      get_vertex_attribute_descriptions();

  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
  vertex_input_state_create_info.pVertexBindingDescriptions = &vertex_binding_description;
  vertex_input_state_create_info.vertexAttributeDescriptionCount = MVK_VERTEX_ATTRIBUTE_COUNT;
  vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_attribute_descriptions;

  // Set input assembly state create info
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
  input_assembly_state_create_info.pNext = NULL_PTR;
  input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;
  input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  // Create viewport
  VkViewport viewport;
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = vulkan_context.swapchain_extent.width;
  viewport.height = vulkan_context.swapchain_extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Specify scissor rect
  VkOffset2D scissor_rect_offset;
  scissor_rect_offset.x = 0;
  scissor_rect_offset.y = 0;

  VkRect2D scissor_rect;
  scissor_rect.offset = scissor_rect_offset;
  scissor_rect.extent = vulkan_context.swapchain_extent;

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
  viewport_state_create_info.pNext = NULL_PTR;
  viewport_state_create_info.viewportCount = 1;  // @MAGIC_NUMBER
  /* viewport_state_create_info.pViewports = &viewport; */
  viewport_state_create_info.scissorCount = 1;  //@MAGIC_NUMBER
  /* viewport_state_create_info.pScissors = &scissor_rect; */

  // Set rasteriser create info
  VkCullModeFlags cull_mode_flags = VK_CULL_MODE_BACK_BIT;

  VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
  rasterization_state_create_info.pNext = NULL_PTR;
  rasterization_state_create_info.depthClampEnable = VK_FALSE;
  rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
  rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterization_state_create_info.cullMode = cull_mode_flags;
  rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterization_state_create_info.depthBiasEnable = VK_FALSE;
  rasterization_state_create_info.lineWidth = 1.0f;  // @MAGIC_NUMBER

  // Set multisampling opts
  VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
  multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisample_state_create_info.sampleShadingEnable = VK_FALSE;

  // Specify colour blending options
  VkColorComponentFlags color_component_flags = VK_COLOR_COMPONENT_R_BIT |
                                                VK_COLOR_COMPONENT_G_BIT |
                                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendAttachmentState color_blend_attachment_state;
  color_blend_attachment_state.blendEnable = VK_TRUE;
  color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment_state.colorWriteMask = color_component_flags;

  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  color_blend_state_create_info.logicOpEnable = VK_FALSE;
  color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_state_create_info.attachmentCount = 1;  // @MAGIC_NUMBER
  color_blend_state_create_info.pAttachments = &color_blend_attachment_state;

  // Create pipeline layout
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipeline_layout_create_info.pNext = NULL_PTR;
  pipeline_layout_create_info.setLayoutCount = 1;
  pipeline_layout_create_info.pSetLayouts = &vulkan_context.descriptor_set_layout;
  pipeline_layout_create_info.pushConstantRangeCount = 0;

  VkResult create_pipeline_layout_result =
      vkCreatePipelineLayout(vulkan_context.logical_device, &pipeline_layout_create_info,
                             vulkan_context.allocator, &vulkan_context.pipeline_layout);
  if (create_pipeline_layout_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan pipeline layout: %s",
                string_VkResult(create_pipeline_layout_result));
    return FALSE;
  }

  // Set graphics pipeline create info
  VkPipelineRenderingCreateInfo pipeline_rendering_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
  pipeline_rendering_create_info.pNext = NULL_PTR;
  pipeline_rendering_create_info.colorAttachmentCount = 1;
  pipeline_rendering_create_info.pColorAttachmentFormats = &vulkan_context.surface_format.format;

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
  graphics_pipeline_create_info.pNext = &pipeline_rendering_create_info;
  graphics_pipeline_create_info.stageCount = 2;  // @MAGIC_NUMBER
  graphics_pipeline_create_info.pStages = shader_stage_create_infos;
  graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
  graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
  graphics_pipeline_create_info.pTessellationState = NULL_PTR;
  graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
  graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
  graphics_pipeline_create_info.pMultisampleState = &multisample_state_create_info;
  graphics_pipeline_create_info.pDepthStencilState = NULL_PTR;
  graphics_pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
  graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
  graphics_pipeline_create_info.layout = vulkan_context.pipeline_layout;
  graphics_pipeline_create_info.renderPass = NULL_PTR;

  // Create graphics pipeline
  VkResult create_graphics_pipelines_result = vkCreateGraphicsPipelines(
      vulkan_context.logical_device, NULL_PTR, 1, &graphics_pipeline_create_info,
      vulkan_context.allocator, &vulkan_context.graphics_pipeline);

  if (create_graphics_pipelines_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan graphics pipeline: %s",
                string_VkResult(create_graphics_pipelines_result));
    return FALSE;
  }
  return TRUE;
}

static b8 vulkan_create_command_pools() {
  VkCommandPoolCreateFlags graphics_command_pool_create_flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkCommandPoolCreateInfo graphics_command_pool_create_info = {
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  graphics_command_pool_create_info.pNext = NULL_PTR;
  graphics_command_pool_create_info.flags = graphics_command_pool_create_flags;
  graphics_command_pool_create_info.queueFamilyIndex = vulkan_context.graphics_queue_family_index;

  VkResult create_graphics_command_pool_result =
      vkCreateCommandPool(vulkan_context.logical_device, &graphics_command_pool_create_info,
                          vulkan_context.allocator, &vulkan_context.graphics_command_pool);

  if (create_graphics_command_pool_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan command pool: %s",
                string_VkResult(create_graphics_command_pool_result));
    return FALSE;
  }
  VkCommandPoolCreateFlags transfer_command_pool_create_flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkCommandPoolCreateInfo transfer_command_pool_create_info = {
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  transfer_command_pool_create_info.pNext = NULL_PTR;
  transfer_command_pool_create_info.flags = transfer_command_pool_create_flags;
  transfer_command_pool_create_info.queueFamilyIndex = vulkan_context.transfer_queue_family_index;

  VkResult create_transfer_command_pool_result =
      vkCreateCommandPool(vulkan_context.logical_device, &transfer_command_pool_create_info,
                          vulkan_context.allocator, &vulkan_context.transfer_command_pool);

  if (create_transfer_command_pool_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan command pool: %s",
                string_VkResult(create_transfer_command_pool_result));
    return FALSE;
  }
  return TRUE;
}

static b8 vulkan_create_vertex_buffers() {
  /* vulkan_context.vertex_buffer = NULL_PTR; */
  /* vulkan_context.vertex_buffer_memory = NULL_PTR; */
  /* vulkan_context.staging_vertex_buffer = NULL_PTR; */
  /* vulkan_context.staging_vertex_buffer_mem = NULL_PTR; */

  VkDeviceSize buffer_size = vulkan_context.vertex_count * sizeof(Vertex);

  if (!create_buffer(&vulkan_context.staging_vertex_buffer,
                     &vulkan_context.staging_vertex_buffer_mem, buffer_size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
    MERROR_CORE("Failed to create staging buffer");
    return FALSE;
  }

  void* vertex_data = NULL_PTR;
  VkResult map_staging_mem_res =
      vkMapMemory(vulkan_context.logical_device, vulkan_context.staging_vertex_buffer_mem, 0,
                  buffer_size, ZERO, &vertex_data);

  if (map_staging_mem_res != VK_SUCCESS) {
    MERROR_CORE("Failed to map vulkan staging buffer memory: %s",
                string_VkResult(map_staging_mem_res));
    return FALSE;
  }

  mcopy_memory(vertex_data, vulkan_context.vertices, buffer_size);
  vkUnmapMemory(vulkan_context.logical_device, vulkan_context.staging_vertex_buffer_mem);

  if (!create_buffer(&vulkan_context.vertex_buffer, &vulkan_context.vertex_buffer_memory,
                     buffer_size,
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
    MERROR_CORE("Failed to create vertex buffer");
    return FALSE;
  }

  if (!copy_buffer(&vulkan_context.staging_vertex_buffer, &vulkan_context.vertex_buffer,
                   buffer_size)) {
    MERROR_CORE("Failed to copy staging buffer to vertex buffer");
    return FALSE;
  }

  return TRUE;
}

static b8 vulkan_create_index_buffer() {
  VkDeviceSize buffer_size = sizeof(vulkan_context.indices[0]) * vulkan_context.index_count;

  if (!create_buffer(&vulkan_context.staging_index_buffer, &vulkan_context.staging_index_buffer_mem,
                     buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
    MERROR_CORE("Failed to create staging index buffer");
    return FALSE;
  }

  void* index_data = NULL_PTR;
  VkResult map_mem_res =
      vkMapMemory(vulkan_context.logical_device, vulkan_context.staging_index_buffer_mem, 0,
                  buffer_size, ZERO, &index_data);
  if (map_mem_res != VK_SUCCESS) {
    MERROR_CORE("Failed to map index staging index buffer memory: %s",
                string_VkResult(map_mem_res));
    return FALSE;
  }
  mcopy_memory(index_data, vulkan_context.indices, buffer_size);
  vkUnmapMemory(vulkan_context.logical_device, vulkan_context.staging_index_buffer_mem);

  if (!create_buffer(&vulkan_context.index_buffer, &vulkan_context.index_buffer_mem, buffer_size,
                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
    MERROR_CORE("Failed to create index buffer");
    return FALSE;
  }

  if (!copy_buffer(&vulkan_context.staging_index_buffer, &vulkan_context.index_buffer,
                   buffer_size)) {
    MERROR_CORE("Failed to copy from staging to index buffer");
    return FALSE;
  }
  return TRUE;
}

static b8 vulkan_create_uniform_buffers() {
  VkDeviceSize buffer_size = sizeof(MVPMat);

  for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (!create_buffer(
            &vulkan_context.uniform_buffers[i], &vulkan_context.uniform_buffer_mem[i], buffer_size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
      MERROR_CORE("Failed to create uniform buffer");
      return FALSE;
    }

    VkResult map_mem_res =
        vkMapMemory(vulkan_context.logical_device, vulkan_context.uniform_buffer_mem[i], 0,
                    buffer_size, ZERO, &vulkan_context.uniform_buffer_mem_mapped[i]);
    if (map_mem_res != VK_SUCCESS) {
      MERROR_CORE("Failed to map uniform buffer memory: %s", string_VkResult(map_mem_res));
      return FALSE;
    }
  }

  return TRUE;
}

static b8 vulkan_create_descriptor_pool() {
  VkDescriptorPoolSize descriptor_pool_size = {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                               .descriptorCount = MAX_FRAMES_IN_FLIGHT};

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
  descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  descriptor_pool_create_info.maxSets = MAX_FRAMES_IN_FLIGHT;
  descriptor_pool_create_info.poolSizeCount = 1;
  descriptor_pool_create_info.pPoolSizes = &descriptor_pool_size;

  VkResult create_descriptor_pool_res =
      vkCreateDescriptorPool(vulkan_context.logical_device, &descriptor_pool_create_info,
                             vulkan_context.allocator, &vulkan_context.descriptor_pool);
  if (create_descriptor_pool_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create descriptor pool: %s",
                string_VkResult(create_descriptor_pool_res));
    return FALSE;
  }

  return TRUE;
}

static b8 vulkan_create_descriptor_sets() {
  VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
  for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    layouts[i] = vulkan_context.descriptor_set_layout;
  }

  VkDescriptorSetAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
  alloc_info.descriptorPool = vulkan_context.descriptor_pool;
  alloc_info.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
  alloc_info.pSetLayouts = layouts;

  VkResult alloc_descriptor_set_res = vkAllocateDescriptorSets(
      vulkan_context.logical_device, &alloc_info, vulkan_context.descriptor_sets);
  if (alloc_descriptor_set_res != VK_SUCCESS) {
    MERROR_CORE("Failed to allocate descriptor sets: %s",
                string_VkResult(alloc_descriptor_set_res));
    return FALSE;
  }

  for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkDescriptorBufferInfo buffer_info;
    buffer_info.buffer = vulkan_context.uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet descriptor_write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    descriptor_write.dstSet = vulkan_context.descriptor_sets[i];
    descriptor_write.dstBinding = 0;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorCount = 1;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(vulkan_context.logical_device, 1, &descriptor_write, 0, NULL_PTR);
  }

  return TRUE;
}

static b8 vulkan_allocate_command_buffers() {
  VkCommandBufferAllocateInfo command_buffer_alloc_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  command_buffer_alloc_info.pNext = NULL_PTR;
  command_buffer_alloc_info.commandPool = vulkan_context.graphics_command_pool;
  command_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  VkResult alloc_command_buffers_result = vkAllocateCommandBuffers(
      vulkan_context.logical_device, &command_buffer_alloc_info, vulkan_context.command_buffers);

  if (alloc_command_buffers_result != VK_SUCCESS) {
    MERROR_CORE("Failed to allocate vulkan command buffers: %s",
                string_VkResult(alloc_command_buffers_result));
    return FALSE;
  }
  return TRUE;
}

static b8 vulkan_create_sync_primatives() {
  vulkan_context.render_complete_semaphores =
      mallocate(vulkan_context.swapchain_image_count * sizeof(VkSemaphore), MEMORY_TAG_RENDERER);

  VkSemaphoreCreateInfo present_complete_semaphore_create_info = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  present_complete_semaphore_create_info.pNext = NULL_PTR;

  VkSemaphoreCreateInfo render_complete_semaphore_create_info = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  render_complete_semaphore_create_info.pNext = NULL_PTR;

  VkFenceCreateInfo draw_fence_create_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  draw_fence_create_info.pNext = NULL_PTR;
  draw_fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkResult present_complete_create_semaphore_result =
        vkCreateSemaphore(vulkan_context.logical_device, &present_complete_semaphore_create_info,
                          vulkan_context.allocator, &vulkan_context.present_complete_semaphores[i]);

    if (present_complete_create_semaphore_result != VK_SUCCESS) {
      MERROR_CORE("Failed to create present complete vulkan semaphore: %s",
                  string_VkResult(present_complete_create_semaphore_result));
      return FALSE;
    }
  }

  for (u32 i = 0; i < vulkan_context.swapchain_image_count; i++) {
    VkResult render_complete_create_semaphore_result =
        vkCreateSemaphore(vulkan_context.logical_device, &render_complete_semaphore_create_info,
                          vulkan_context.allocator, &vulkan_context.render_complete_semaphores[i]);

    if (render_complete_create_semaphore_result != VK_SUCCESS) {
      MERROR_CORE("Failed to create render complete vulkan semaphore: %s",
                  string_VkResult(render_complete_create_semaphore_result));
      return FALSE;
    }
  }

  for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkResult draw_create_fence_result =
        vkCreateFence(vulkan_context.logical_device, &draw_fence_create_info,
                      vulkan_context.allocator, &vulkan_context.draw_fences[i]);
    if (draw_create_fence_result != VK_SUCCESS) {
      MERROR_CORE("Failed to create draw vulkan fence: %s",
                  string_VkResult(draw_create_fence_result));
      return FALSE;
    }
  }
  return TRUE;
}

static b8 vulkan_recreate_swapchain(u16 width, u16 height) {
  VkResult queue_wait_idle_result = vkQueueWaitIdle(vulkan_context.graphics_queue);
  if (queue_wait_idle_result != VK_SUCCESS) {
    MWARN_CORE("Failed to wait for queue idle while recreating vulkan swapchain: %s",
               string_VkResult(queue_wait_idle_result));
  }

  if (!vulkan_set_surface_extent(width, height)) {
    MERROR_CORE("Failed to set surface extent while recreating vulkan swapchain");
    return FALSE;
  }

  VkSwapchainKHR old_swapchain = vulkan_context.swapchain;

  if (!vulkan_create_swapchain(old_swapchain)) {
    MERROR_CORE("Failed to create swapchain");
    return FALSE;
  }

  for (u32 i = 0; i < vulkan_context.swapchain_image_count; i++) {
    vkDestroyImageView(vulkan_context.logical_device, vulkan_context.swapchain_image_views[i],
                       vulkan_context.allocator);
  }

  vkDestroySwapchainKHR(vulkan_context.logical_device, old_swapchain, vulkan_context.allocator);

  if (!vulkan_get_swapchain_images()) {
    MERROR_CORE("Failed to get swapchain images");
    return FALSE;
  }

  if (!vulkan_create_image_views()) {
    MERROR_CORE("Failed to create image views");
    return FALSE;
  }

  return TRUE;
}

/* static b8 vulkan_cleanup_swapchain() { return TRUE; } */

static void transition_image_layout(u32 image_index, VkImageLayout old_layout,
                                    VkImageLayout new_layout, VkAccessFlags2 src_access_mask,
                                    VkAccessFlags2 dst_access_mask,
                                    VkPipelineStageFlags2 src_stage_mask,
                                    VkPipelineStageFlags2 dst_stage_mask) {
  VkImageSubresourceRange subresource_range;
  subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresource_range.baseMipLevel = 0;
  subresource_range.levelCount = 1;
  subresource_range.baseArrayLayer = 0;
  subresource_range.layerCount = 1;

  VkImageMemoryBarrier2 image_memory_barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
  image_memory_barrier.srcStageMask = src_stage_mask;
  image_memory_barrier.dstStageMask = dst_stage_mask;
  image_memory_barrier.srcAccessMask = src_access_mask;
  image_memory_barrier.dstAccessMask = dst_access_mask;
  image_memory_barrier.oldLayout = old_layout;
  image_memory_barrier.newLayout = new_layout;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = vulkan_context.swapchain_images[image_index];
  image_memory_barrier.subresourceRange = subresource_range;

  VkDependencyInfo dependency_info = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
  dependency_info.dependencyFlags = ZERO;
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers = &image_memory_barrier;

  vkCmdPipelineBarrier2(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                        &dependency_info);
}

static b8 create_buffer(VkBuffer* buffer, VkDeviceMemory* device_mem, VkDeviceSize size,
                        VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags prop_flags) {
  u32 queue_family_indices[] = {vulkan_context.transfer_queue_family_index,
                                vulkan_context.graphics_queue_family_index};
  VkBufferCreateInfo buffer_create_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  buffer_create_info.size = size;
  buffer_create_info.usage = usage_flags;
  if (vulkan_context.queue_family_index_count == 1) {
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  } else {
    buffer_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
  }
  buffer_create_info.queueFamilyIndexCount = vulkan_context.queue_family_index_count;
  buffer_create_info.pQueueFamilyIndices = queue_family_indices;

  VkResult create_buffer_result = vkCreateBuffer(vulkan_context.logical_device, &buffer_create_info,
                                                 vulkan_context.allocator, buffer);

  if (create_buffer_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan buffer: %s", string_VkResult(create_buffer_result));
    return FALSE;
  }

  VkMemoryRequirements buffer_mem_requirements;
  vkGetBufferMemoryRequirements(vulkan_context.logical_device, *buffer, &buffer_mem_requirements);

  VkMemoryAllocateInfo mem_alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  mem_alloc_info.allocationSize = buffer_mem_requirements.size;
  mem_alloc_info.memoryTypeIndex =
      get_memory_type(buffer_mem_requirements.memoryTypeBits, prop_flags);

  VkResult alloc_mem_res = vkAllocateMemory(vulkan_context.logical_device, &mem_alloc_info,
                                            vulkan_context.allocator, device_mem);
  if (alloc_mem_res != VK_SUCCESS) {
    MERROR_CORE("Failed to allocate vulkan device memory: %s", string_VkResult(alloc_mem_res));
    return FALSE;
  }

  VkResult bind_buffer_mem =
      vkBindBufferMemory(vulkan_context.logical_device, *buffer, *device_mem, 0);
  if (bind_buffer_mem != VK_SUCCESS) {
    MERROR_CORE("Failed to bind vulkan buffer memory: %s", string_VkResult(bind_buffer_mem));
  }
  return TRUE;
}

static b8 copy_buffer(VkBuffer* src_buffer, VkBuffer* dst_buffer, VkDeviceSize size) {
  VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  alloc_info.commandPool = vulkan_context.transfer_command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer copy_cmd_buf;
  VkResult create_cmd_buf_res =
      vkAllocateCommandBuffers(vulkan_context.logical_device, &alloc_info, &copy_cmd_buf);
  if (create_cmd_buf_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create copy command buffer: %s", string_VkResult(create_cmd_buf_res));
  }

  VkCommandBufferBeginInfo cmd_buf_begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  cmd_buf_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  VkResult begin_cmd_buf_res = vkBeginCommandBuffer(copy_cmd_buf, &cmd_buf_begin_info);
  if (begin_cmd_buf_res != VK_SUCCESS) {
    MERROR_CORE("Failed to begin command buffer: %s", string_VkResult(begin_cmd_buf_res));
    return FALSE;
  }

  VkBufferCopy buf_copy = {.srcOffset = 0, .dstOffset = 0, .size = size};
  vkCmdCopyBuffer(copy_cmd_buf, *src_buffer, *dst_buffer, 1, &buf_copy);

  VkResult end_cmd_buf_res = vkEndCommandBuffer(copy_cmd_buf);
  if (end_cmd_buf_res != VK_SUCCESS) {
    MERROR_CORE("Failed to end command buffer: %s", string_VkResult(end_cmd_buf_res));
    return FALSE;
  }
  VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &copy_cmd_buf;

  VkResult queue_submit_res =
      vkQueueSubmit(vulkan_context.transfer_queue, 1, &submit_info, NULL_PTR);

  if (queue_submit_res != VK_SUCCESS) {
    MERROR_CORE("Failed to submit to queue: %s", string_VkResult(queue_submit_res));
    return FALSE;
  }

  VkResult wait_idle_res = vkQueueWaitIdle(vulkan_context.transfer_queue);
  if (wait_idle_res != VK_SUCCESS) {
    MERROR_CORE("Failed to wait for queue idle: %s", string_VkResult(wait_idle_res));
    return FALSE;
  }
  return TRUE;
}

static VkVertexInputBindingDescription get_vertex_binding_description() {
  VkVertexInputBindingDescription out;
  out.binding = 0;
  out.stride = sizeof(Vertex);
  out.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return out;
}

static VkVertexInputAttributeDescription* get_vertex_attribute_descriptions() {
  static VkVertexInputAttributeDescription out[] = {
      {
          .location = 0,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32_SFLOAT,
          .offset = offsetof(Vertex, position),
      },
      {
          .location = 1,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32_SFLOAT,
          .offset = offsetof(Vertex, colour),
      },
      {
          .location = 2,
          .binding = 0,
          .format = VK_FORMAT_R32G32_SFLOAT,
          .offset = offsetof(Vertex, texture_coord),

      },
  };
  return out;
}

static u32 get_memory_type(u32 type_filter, VkMemoryPropertyFlags props) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(vulkan_context.physical_device, &memory_properties);

  for (u32 i = 0; i < memory_properties.memoryTypeCount; i++) {
    if ((type_filter & (1 << i)) &&
        (memory_properties.memoryTypes[i].propertyFlags & props) == props) {
      return i;
    }
  }
  MERROR_CORE("Failed to find memory type.");
  return UINT32_MAX;
}

static b8 update_uniform_buffer() {
  x += 0.150f;
  if (x >= 360.0f) {
    x = 0.0f;
  }

  MVPMat mvp_mat;
  Mat4 model_tran = {1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f};

  mvp_mat.model =
      mat4_mul(mat4_from_quat(quat_from_euler((Vec3){M_TO_RAD(x), M_TO_RAD(x), M_TO_RAD(x)})),
               mat4_transpose(model_tran));
  mvp_mat.view = mat4_iden();
  mvp_mat.proj = mat4_perspective(
      M_TO_RAD(90.0f),
      (f32)vulkan_context.swapchain_extent.width / (f32)vulkan_context.swapchain_extent.height,
      0.1f, 1000.0f);

  mvp_mat.proj.e22 *= -1.0f;
  mcopy_memory(vulkan_context.uniform_buffer_mem_mapped[vulkan_context.current_frame_index],
               &mvp_mat, sizeof(mvp_mat));
  return TRUE;
}
