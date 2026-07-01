/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ui/ui_types.h"
#define STB_IMAGE_IMPLEMENTATION

#include "vulkan_backend.h"

#include <stddef.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "renderer/renderer_types.h"
#include "vendor/stb_image.h"
#include "core/log.h"
#include "core/mmemory.h"
#include "define.h"
#include "maths/maths_util.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"
#include "vulkan_defines.h"
#include "vulkan_wsi.h"
#include "asset/mg3d.h"
#include "data_structures/darray.h"
#include "default_scene.h"
#include "ecs/scene_text_loader.h"
#include "ecs/ecs.h"
#include "ui/ui.h"

static VulkanContext vulkan_context = {};

static f32 viewport_scale = 2.0f;
static f32 x = 0;
static b8 toggle = TRUE;

// @TODO!: Make it so scenes and ui can be loaded after backend is init
// without crashing.
b8 vulkan_backend_init(RendererBackend* renderer_backend,
                       const char* application_name, i16 start_width,
                       i16 start_height, PlatformState* platform_state) {

  vulkan_context.scene_data = NULL_PTR;
  vulkan_context.allocator = NULL_PTR;
  vulkan_context.debug_messenger = NULL_PTR;
  vulkan_context.swapchain_extent.width = start_width;
  vulkan_context.swapchain_extent.height = start_height;


  vulkan_context.ui_data = get_ui_data_ptr();

  scene_load_text("../engine/src/ecs/default.txt");
  vulkan_request_scene_data();

  MTRACE("%s", get_memory_usage_string());
  MTRACE("%s", vulkan_context.scene_data->meshes[0].name);
  for (u32 i = 0;
       i < vulkan_context.scene_data->mesh_assets[0].vertex_count;
       i++) {
    vulkan_context.scene_data->mesh_assets[0].vertices[i].colour =
      (Vec3){0.3f, 0.5f, 0.8f};
  }

  vulkan_context.uniform_object_count =
    darray_get_length(vulkan_context.scene_data->drawable_handles);


  // @TODO: Free the stuff below
  vulkan_context.uniform_bufs =
    mallocate(vulkan_context.uniform_object_count *
	      sizeof(VkBuffer) * MAX_FRAMES_IN_FLIGHT,
	      MEMORY_TAG_RENDERER);

  vulkan_context.uniform_buf_mem =
    mallocate(vulkan_context.uniform_object_count *
	      sizeof(VkDeviceMemory) * MAX_FRAMES_IN_FLIGHT,
	      MEMORY_TAG_RENDERER);

  vulkan_context.uniform_buf_mem_map =
    mallocate(vulkan_context.uniform_object_count *
	      sizeof(void*) * MAX_FRAMES_IN_FLIGHT,
	      MEMORY_TAG_RENDERER);

  // UI TEST

  UiRoot* ui_root = ui_root_create("HUD", (Vec2u) {1920, 1080});
  UiRect* ui_square = ui_rect_create_floating((Vec2u){200, 200},
					      (Vec2u){10, 10,});
  ui_rect_set_color(ui_square, (UiColor){
      .primary_color = {
	.r = 1.0f,
	.g = 0.0f,
	.b = 0.0f,                                             
      },                                             
      .gradient = UI_COLOR_GRAD_SOLID,
    });

  ui_root_add_rect(ui_root, ui_square);  

  ui_gen_vertices(ui_root);



  /*

    if (!scene_load(&test_scene)) {
    MERROR_CORE("Failed to load scene: %s", test_scene.name);
    return FALSE;
    }

    MTRACE("|| Scene: %s || -------- (%u) entities --------", test_scene.name, test_scene.entity_count);
  */

  /*
    Vertex* vbuf = NULL_PTR;
    u32* ibuf = NULL_PTR;
    u32 vcount;
    u32 icount;
  */
  //mg3d_from_obj("../test/res/models/mclaren.obj", "../test/res/models/mclaren.mg3d");
  /*
    mg3d_load("../test/res/models/crate.mg3d", &vbuf, &vcount, &ibuf, &icount);

    vulkan_context.vertex_count = vcount;
    vulkan_context.vertices = vbuf;
    vbuf = NULL_PTR;

    vulkan_context.index_count = icount;
    vulkan_context.indices = ibuf;
    ibuf = NULL_PTR;
  */


  //MINFO_CORE("Loaded %u vertices, %u indices", vulkan_context.vertex_count, vulkan_context.index_count);

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
  if (!vulkan_select_physical_device(&vulkan_context)) {
    MERROR_CORE("Vulkan Init: Failed to select physical device.");
    return FALSE;
  }

  // GET SURFACE
  if (!vulkan_get_surface(&vulkan_context, platform_state)) {
    MERROR_CORE("Vulkan Init: Failed to create surface.");
    return FALSE;
  }

  // SETUP LOGICAL DEVICE ----------
  if (!vulkan_create_logical_device(&vulkan_context)) {
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

  // CREATE DEPTH RESOURCECS
  if (!vulkan_create_depth_resources()) {
    MERROR_CORE("Vulkan Init: Failed to create depth resoucres.");
    return FALSE;
  }

  // CREATE OBJECT PIPELINE ----------
  if (!vulkan_create_pbr_pipeline(&vulkan_context)) {
    MERROR_CORE("Vulkan Init: Failed to create pbr pipeline.");
    return FALSE;
  }

  // CREATE UI PIPELINE
  if (!vulkan_create_ui_pipeline(&vulkan_context)) {
    MERROR_CORE("Vulkan Init: Failed to create UI pipeline");
    return FALSE;    
  }    

  // CREATE COMMAND POOLS ----------
  if (!vulkan_create_command_pools()) {
    MERROR_CORE("Vulkan Init: Failed to create command pool.");
    return FALSE;
  }

  // CREATE DRAWABLE OBJECTS
  if (!vulkan_create_drawable_objects()) {
    MERROR_CORE("Vulkan Init: Failed to create drawable objects.");
    return FALSE;
  }

  // CREATE TEXTURE IMAGE
  if (!vulkan_create_texture_image()) {
    MERROR_CORE("Vulkan Init: Failed to create texture image.");
    return FALSE;
  }

  // CREATE TEXTURE IMAGE VIEW
  if (!vulkan_create_texture_image_view()) {
    MERROR_CORE("Vulkan Init: Failed to create texture image view.");
    return FALSE;
  }

  // CREATE TEXTURE IMAGE SAMPLER
  if (!vulkan_create_texture_image_sampler()) {
    MERROR_CORE("Vulkan Init: Failed to create texture imgae sampler.");
    return FALSE;
  }

  // CREATE DESCRIPTOR SETS ----------
  if (!vulkan_create_descriptor_sets()) {
    MERROR_CORE("Vukan Init: Failed to create descriptor sets");
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

  MTRACE_CORE("%s", string_VkFormat(vulkan_context.surface_format.format));


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
  vkDestroyPipeline(vulkan_context.logical_device, vulkan_context.pbr_pipeline,
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

  transition_image_layout(vulkan_context.swapchain_images[vulkan_context.current_image_index], VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, ZERO,
                          VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_IMAGE_ASPECT_COLOR_BIT);

  transition_image_layout(vulkan_context.depth_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                          VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                          VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                          VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                          VK_IMAGE_ASPECT_DEPTH_BIT);

  VkClearColorValue clear_color_value = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}};
  VkClearValue clear_color = {.color = clear_color_value};
  VkClearDepthStencilValue clear_depth_value = {.depth = 1.0f, .stencil = 0};
  VkClearValue clear_depth = {.depthStencil = clear_depth_value};

  VkRenderingAttachmentInfo rendering_attachment_info = {
    VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
  rendering_attachment_info.imageView =
    vulkan_context.swapchain_image_views[vulkan_context.current_image_index];
  rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  rendering_attachment_info.clearValue = clear_color;

  VkRenderingAttachmentInfo depth_attachment_info = {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    .imageView = vulkan_context.depth_image_view,
    .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .clearValue = clear_depth,
  };

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
  rendering_info.pDepthAttachment = &depth_attachment_info;

  // Start rendering
  vkCmdBeginRendering(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                      &rendering_info);

  // PBR RENDER
  vkCmdBindPipeline(vulkan_context.command_buffers[vulkan_context.current_frame_index],
                    VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_context.pbr_pipeline);

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

  u32 object_count = darray_get_length(vulkan_context.objects);

  for (u32 i = 0; i < object_count; i++) {

    /*
    // Get pointer to next drawable entity
    Entity* p_entity = &vulkan_context.scene_data->entities[vulkan_context.scene_data->drawable_handles[i]];

    // Get handle to buffers
    Handle32 handle;
    for (u32 i = 0; i < p_entity->component_count; i++) {
    if (p_entity->components[i].type == COMPONENT_TYPE_MESH) {
    handle = p_entity->components[i].handle;
    }
    }
    */

    VulkanDrawable object = vulkan_context.objects[i];

    VkDeviceSize temp_offsets[] = {0};  // @TODO: stuff
    vkCmdBindVertexBuffers(vulkan_context.command_buffers
                           [vulkan_context.current_frame_index],
                           0, 1,
                           &vulkan_context.vertex_bufs
                           [object.vbuf_handle], temp_offsets);

    vkCmdBindIndexBuffer(vulkan_context.command_buffers
                         [vulkan_context.current_frame_index],
                         vulkan_context.index_bufs[object.ibuf_handle],
                         0, VK_INDEX_TYPE_UINT32);
    
    
    vkCmdBindDescriptorSets(vulkan_context.command_buffers
                            [vulkan_context.current_frame_index],
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vulkan_context.pbr_pipeline_layout, 0, 1,
                            &vulkan_context.pbr_descriptor_sets
                            [(i * MAX_FRAMES_IN_FLIGHT) +
                             vulkan_context.current_frame_index],
                            0, NULL_PTR);
    
    vkCmdDrawIndexed(vulkan_context.command_buffers
                     [vulkan_context.current_frame_index],
                     vulkan_context.scene_data->mesh_assets
                     [object.vbuf_handle].index_count,
                     1, 0, 0, 0);
  }

  // UI RENDER

  vkCmdBindPipeline(vulkan_context.command_buffers
                    [vulkan_context.current_frame_index],
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vulkan_context.ui_pipeline);
  

  vkCmdSetViewport(vulkan_context.command_buffers
                   [vulkan_context.current_frame_index], 0, 1,
                   &viewport);

  vkCmdSetScissor(vulkan_context.command_buffers
                  [vulkan_context.current_frame_index],
                  0, 1,
                  &scissor);

  VkDeviceSize temp_offset[] = {0};
  vkCmdBindVertexBuffers(vulkan_context.command_buffers
                         [vulkan_context.current_frame_index],
                         0, 1, &vulkan_context.ui_vbuf, temp_offset);

  vkCmdBindIndexBuffer(vulkan_context.command_buffers
                       [vulkan_context.current_frame_index],
                       vulkan_context.ui_ibuf, 0, VK_INDEX_TYPE_UINT32);

  vkCmdDrawIndexed(vulkan_context.command_buffers
                   [vulkan_context.current_frame_index],
                   darray_get_length(vulkan_context.ui_data->
                                     rect_indices),
                   1, 0, 0, 0);

                         

  return TRUE;
}

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time) {
  // End rendering
  vkCmdEndRendering(vulkan_context.command_buffers[vulkan_context.current_frame_index]);

  transition_image_layout(
                          vulkan_context.swapchain_images[vulkan_context.current_image_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, ZERO,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

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
  MTRACE_CORE("vk resize: (%u, %u)", width, height);
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

static b8 vulkan_request_scene_data() {

  vulkan_context.scene_data = get_scene_data_ptr();
  if (vulkan_context.scene_data == NULL_PTR) {
    MERROR_CORE("Vulkan failed to get scene data");
  }

  return TRUE;
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

static b8 vulkan_create_drawable_objects() {
  vulkan_context.objects = darray_create(VulkanDrawable);

  for (u32 i = 0; i < darray_get_length(vulkan_context.scene_data->drawable_handles); i++) {
    Entity* p_entity = &vulkan_context.scene_data->entities[vulkan_context.scene_data->drawable_handles[i]];
    Handle32 mesh_handle = vulkan_context.scene_data->meshes[entity_get_mesh_handle(p_entity)].mesh_asset_handle;
    VulkanDrawable object = {
      .vbuf_handle = mesh_handle,
      .ibuf_handle = mesh_handle,
      .texture_handle = vulkan_context.scene_data->textures[material_get_texture_handle(entity_get_material(p_entity))].texture_asset_handle,
    };
    darray_push(vulkan_context.objects, object);
  }

  return TRUE;
}

static b8 vulkan_create_descriptor_set_layout() {
  //PBR
  VkDescriptorSetLayoutBinding mvp_layout_binding = {};
  mvp_layout_binding.binding = 0;
  mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  mvp_layout_binding.descriptorCount = 1;
  mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding texture_sampler_layout_binding = {
    .binding = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
  };

  VkDescriptorSetLayoutBinding layout_bindings[2] = {
    mvp_layout_binding,
    texture_sampler_layout_binding,
  };

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  descriptor_set_layout_create_info.bindingCount = 2;
  descriptor_set_layout_create_info.pBindings = layout_bindings;

  VkResult create_descriptor_set_layout_res =
    vkCreateDescriptorSetLayout(vulkan_context.logical_device, &descriptor_set_layout_create_info,
                                vulkan_context.allocator, &vulkan_context.pbr_descriptor_set_layout);
  if (create_descriptor_set_layout_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create descriptor set layout: %s",
                string_VkResult(create_descriptor_set_layout_res));
    return FALSE;
  }

  //UI
  VkDescriptorSetLayoutBinding ui_uniform_layout_binding = {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
  };

  VkDescriptorSetLayoutBinding ui_layout_bindings[1] = {
    ui_uniform_layout_binding,
  };

  VkDescriptorSetLayoutCreateInfo ui_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .bindingCount = 1,
    .pBindings = ui_layout_bindings,
  };

  if (vkCreateDescriptorSetLayout(vulkan_context.logical_device, &ui_layout_create_info,
                                  vulkan_context.allocator,
                                  &vulkan_context.ui_descriptor_set_layout) != VK_SUCCESS) {
    MERROR_CORE("Failed to create UI descriptor set layout: %s");
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

static b8 vulkan_create_depth_resources() {

  VkFormat depth_format = find_supported_depth_format();
  vulkan_context.depth_format = depth_format;

  if (!create_image(&vulkan_context.depth_image, &vulkan_context.depth_image_mem, vulkan_context.swapchain_extent.width,
                    vulkan_context.swapchain_extent.height, depth_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
    MERROR_CORE("Failed to create depth image");
    return FALSE;
  }

  if (!create_image_view(&vulkan_context.depth_image_view, vulkan_context.depth_image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, FALSE)) {
    MERROR_CORE("Failed to create depth image view");
    return FALSE;
  }

  return TRUE;
}

static b8 vulkan_create_texture_image() {

  u32 texture_count = darray_get_length(vulkan_context.scene_data->texture_assets);
  vulkan_context.texture_images = mallocate(texture_count * sizeof(VkImage), MEMORY_TAG_RENDERER); // @TODO: Free
  vulkan_context.texture_image_mem = mallocate(texture_count * sizeof(VkDeviceMemory), MEMORY_TAG_RENDERER); // @TODO: Free
  
  for(u32 i = 0; i < texture_count; i++) {
    ATexture* p_texture =
      &vulkan_context.scene_data->texture_assets[i];
    
    VkDeviceSize image_size = p_texture->width * p_texture->height * 4;
    
    
    if (!create_buffer(&vulkan_context.staging_texture_buf,
                       &vulkan_context.staging_texture_buf_mem,
                       image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
      MERROR_CORE("Failed to create staging texture buffer for: %s",
                  p_texture->image_path);
      return FALSE;
    }

    void* data;

    VkResult map_mem_res =
      vkMapMemory(vulkan_context.logical_device,
                  vulkan_context.staging_texture_buf_mem, 0, image_size,
                  ZERO, &data);
    
    if (map_mem_res != VK_SUCCESS) {
      MERROR_CORE("Failed to map staging texture buffer memory: %s",
                  string_VkResult(map_mem_res));
      return FALSE;
    }
    
    mcopy_memory(data, p_texture->texture_data, image_size);
    vkUnmapMemory(vulkan_context.logical_device,
                  vulkan_context.staging_texture_buf_mem);
    
    stbi_image_free(p_texture->texture_data);

    if (!create_image(&vulkan_context.texture_images[i],
                      &vulkan_context.texture_image_mem[i], p_texture->width,
                      p_texture->height,
                      vulkan_context.surface_format.format,
                      VK_IMAGE_TILING_OPTIMAL,
                      VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                      VK_IMAGE_USAGE_SAMPLED_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
      MERROR_CORE("Failed to create image for texture");
      return FALSE;
    }

    VkCommandBuffer command_buf =
      begin_single_time_commands(vulkan_context.graphics_command_pool);
    if (command_buf == VK_NULL_HANDLE) {
      MERROR_CORE("Failed to begin single time commands");
      return FALSE;
    }

    if (!transition_tex_image_layout(&command_buf,
                                     &vulkan_context.texture_images[i],
                                     VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)) {
      MERROR_CORE("Failed to transfer image layout");
      return FALSE;
    }

    if (!copy_buffer_to_image(&command_buf,
                              &vulkan_context.staging_texture_buf,
                              &vulkan_context.texture_images[i],
                              p_texture->width, p_texture->height)) {
      MERROR_CORE("Failed to copy buffer to image");
      return FALSE;
    }

    if (!transition_tex_image_layout(&command_buf,
                                     &vulkan_context.texture_images[i],
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)) {
      MERROR_CORE("Failed to transfer image layout");
      return FALSE;
    }

    if (!end_single_time_commands(&command_buf,
                                  vulkan_context.graphics_queue)) {
      MERROR_CORE("Failed to end single time commands");
      return FALSE;
    }
  }
   
  return TRUE;
}

static b8 vulkan_create_texture_image_view() {

  u32 texture_count = darray_get_length(vulkan_context.scene_data->texture_assets);

  vulkan_context.texture_image_views = mallocate(texture_count * sizeof(VkImageView), MEMORY_TAG_RENDERER); // @TODO: Free
    
  VkComponentMapping component_mapping;
  component_mapping.r = VK_COMPONENT_SWIZZLE_B;
  component_mapping.g = VK_COMPONENT_SWIZZLE_G;
  component_mapping.b = VK_COMPONENT_SWIZZLE_R;
  component_mapping.a = VK_COMPONENT_SWIZZLE_A;
  
  VkImageSubresourceRange subresource_range = {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1};

  for (u32 i = 0; i < texture_count; i++) {

    VkImageViewCreateInfo view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = vulkan_context.texture_images[i],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = vulkan_context.surface_format.format,
      .subresourceRange = subresource_range,
      .components = component_mapping,
    };
  
    VkResult create_img_view_res = vkCreateImageView(vulkan_context.logical_device, &view_create_info, vulkan_context.allocator,
                                                     &vulkan_context.texture_image_views[i]);
    if (create_img_view_res != VK_SUCCESS) {
      MERROR_CORE("Failed to create texture image view: %s", string_VkResult(create_img_view_res));
      return FALSE;
    }
  }

  return TRUE;
}

static b8 vulkan_create_texture_image_sampler() {

  VkSamplerCreateInfo sampler_create_info = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .magFilter = VK_FILTER_NEAREST,
    .minFilter = VK_FILTER_NEAREST,
    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
    .mipLodBias = 0.0f,
    .minLod = 0.0f,
    .maxLod = 0.0f,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .anisotropyEnable = VK_TRUE,
    .maxAnisotropy = vulkan_context.physical_device_properties.limits.maxSamplerAnisotropy,
    .compareEnable = VK_FALSE,
    .compareOp = VK_COMPARE_OP_ALWAYS,
    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    .unnormalizedCoordinates = VK_FALSE
  };

  VkResult create_sampler_res = vkCreateSampler(vulkan_context.logical_device, &sampler_create_info, vulkan_context.allocator,
                                                &vulkan_context.texture_image_sampler);
  if (create_sampler_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create sampler: %s", string_VkResult(create_sampler_res));
    return FALSE;
  }

  return TRUE;
}

static b8 vulkan_create_vertex_buffers() {
  /* vulkan_context.vertex_buffer = NULL_PTR; */
  /* vulkan_context.vertex_buffer_memory = NULL_PTR; */
  /* vulkan_context.staging_vertex_buffer = NULL_PTR; */
  /* vulkan_context.staging_vertex_buffer_mem = NULL_PTR; */

  // PBR

  u32 mesh_count =
    darray_get_length(vulkan_context.scene_data->mesh_assets);

  vulkan_context.vertex_bufs =
    mallocate(mesh_count * sizeof(VkBuffer),
	      MEMORY_TAG_RENDERER); // @TODO: Free

  vulkan_context.vertex_buf_mem =
    mallocate(mesh_count * sizeof(VkDeviceMemory),
	      MEMORY_TAG_RENDERER); // @TODO: Free

  for (u32 i = 0; i < mesh_count;i++) {
    
    VkDeviceSize buffer_size =
      vulkan_context.scene_data->mesh_assets[i].vertex_count *
      sizeof(Vertex);
 
    if (!create_buffer(&vulkan_context.staging_vertex_buffer,
                       &vulkan_context.staging_vertex_buffer_mem,
		       buffer_size,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
      MERROR_CORE("Failed to create staging buffer");
      return FALSE;
    }

    void* vertex_data = NULL_PTR;
    VkResult map_staging_mem_res =
      vkMapMemory(vulkan_context.logical_device,
		  vulkan_context.staging_vertex_buffer_mem, 0,
                  buffer_size, ZERO, &vertex_data);

    if (map_staging_mem_res != VK_SUCCESS) {
      MERROR_CORE("Failed to map vulkan staging buffer memory: %s",
                  string_VkResult(map_staging_mem_res));
      return FALSE;
    }

    mcopy_memory(vertex_data,
		 vulkan_context.scene_data->mesh_assets[i].vertices,
		 buffer_size);
    vkUnmapMemory(vulkan_context.logical_device,
		  vulkan_context.staging_vertex_buffer_mem);

    if (!create_buffer(&vulkan_context.vertex_bufs[i],
		       &vulkan_context.vertex_buf_mem[i],
                       buffer_size,
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
		       VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
      MERROR_CORE("Failed to create vertex buffer");
      return FALSE;
    }

    if (!copy_buffer(&vulkan_context.staging_vertex_buffer,
		     &vulkan_context.vertex_bufs[i],
                     buffer_size)) {
      MERROR_CORE("Failed to copy staging buffer to vertex buffer");
      return FALSE;
    }
  }

  // UI
  vulkan_context.ui_vdata = NULL_PTR;

  VkDeviceSize ui_buf_size =
    darray_get_length(vulkan_context.ui_data->rect_vertices) *
    sizeof(UiVertex);

  if (!create_buffer(&vulkan_context.ui_vbuf,
		     &vulkan_context.ui_vbuf_mem,
		     ui_buf_size,
		     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
    MERROR_CORE("Failed to create UI vertex buffer");
    return FALSE;
  }

  VkResult ui_buf_map_mem_res =
    vkMapMemory(vulkan_context.logical_device,
		vulkan_context.ui_vbuf_mem, 0,
		ui_buf_size, 0,
		&vulkan_context.ui_vdata);
  
  if (ui_buf_map_mem_res != VK_SUCCESS) {
    MERROR_CORE("Failed to map memory to ui vertices: %s",
		string_VkResult(ui_buf_map_mem_res));
    return FALSE;
  }

  mcopy_memory(vulkan_context.ui_vdata,
	vulkan_context.ui_data->rect_vertices,
	ui_buf_size);


  return TRUE;
}

static b8 vulkan_create_index_buffer() {

  // PBR
  u32 mesh_count =
    darray_get_length(vulkan_context.scene_data->mesh_assets);

  vulkan_context.index_bufs =
    mallocate(mesh_count * sizeof(VkBuffer),
	      MEMORY_TAG_RENDERER); // @TODO: Free

  vulkan_context.index_buf_mem =
    mallocate(mesh_count * sizeof(VkDeviceMemory),
	      MEMORY_TAG_RENDERER); // @TODO: Free

  for (u32 i = 0; i < mesh_count; i++) {

    VkDeviceSize buffer_size = sizeof(u32) *
      vulkan_context.scene_data->mesh_assets[i].index_count;

    if (!create_buffer(&vulkan_context.staging_index_buffer,
		       &vulkan_context.staging_index_buffer_mem,
                       buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
      MERROR_CORE("Failed to create staging index buffer");
      return FALSE;
    }

    void* index_data = NULL_PTR;
    VkResult map_mem_res =
      vkMapMemory(vulkan_context.logical_device,
		  vulkan_context.staging_index_buffer_mem, 0,
                  buffer_size, ZERO, &index_data);
    if (map_mem_res != VK_SUCCESS) {
      MERROR_CORE("Failed to map index staging buffer memory: %s",
                  string_VkResult(map_mem_res));
      return FALSE;
    }
    mcopy_memory(index_data,
		 vulkan_context.scene_data->mesh_assets[i].indices,
		 buffer_size);

    vkUnmapMemory(vulkan_context.logical_device,
		  vulkan_context.staging_index_buffer_mem);

    if (!create_buffer(&vulkan_context.index_bufs[i],
		       &vulkan_context.index_buf_mem[i],
		       buffer_size,
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
		       VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
      MERROR_CORE("Failed to create index buffer");
      return FALSE;
    }

    if (!copy_buffer(&vulkan_context.staging_index_buffer,
		     &vulkan_context.index_bufs[i],
                     buffer_size)) {
      MERROR_CORE("Failed to copy from staging to index buffer");
      return FALSE;
    }
  }

  //UI
  VkDeviceSize ui_buf_size = sizeof(u32) *
    darray_get_length(vulkan_context.ui_data->rect_indices);

  if (!create_buffer(&vulkan_context.ui_ibuf,
		     &vulkan_context.ui_ibuf_mem,
		     ui_buf_size,
		     VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
    MERROR_CORE("Failed to create UI index buffer");
    return FALSE;
  }

  VkResult ui_map_mem_res =
    vkMapMemory(vulkan_context.logical_device,
		vulkan_context.ui_ibuf_mem,
		0,
		ui_buf_size,
		0,
		&vulkan_context.ui_idata);

  if (ui_map_mem_res != VK_SUCCESS) {
    MERROR_CORE("Failed to map ui index buffer memory: %s",
		string_VkResult(ui_map_mem_res));
    return FALSE;
  }

  mcopy_memory(vulkan_context.ui_idata,
	       vulkan_context.ui_data->rect_indices,
	       ui_buf_size);

  return TRUE;
}

static b8 vulkan_create_uniform_buffers() {

  
  // PBR
  for (u32 i = 0; i < vulkan_context.uniform_object_count; i++) {
    for (u32 j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
      VkDeviceSize buffer_size = sizeof(MVPMat);

      if (!create_buffer(&vulkan_context.uniform_bufs
			 [(i * MAX_FRAMES_IN_FLIGHT) + j],
                         &vulkan_context.uniform_buf_mem
			 [(i * MAX_FRAMES_IN_FLIGHT) + j],
                         buffer_size,
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        MERROR_CORE("Failed to create uniform buffer");
        return FALSE;
      }
      
      VkResult map_mem_res =
        vkMapMemory(vulkan_context.logical_device,
                    vulkan_context.uniform_buf_mem
		    [(i * MAX_FRAMES_IN_FLIGHT) + j],
                    0, buffer_size, ZERO,
		    &vulkan_context.uniform_buf_mem_map
		    [(i * MAX_FRAMES_IN_FLIGHT) + j]);

      if (map_mem_res != VK_SUCCESS) {
        MERROR_CORE("Failed to map uniform buffer memory: %s",
		    string_VkResult(map_mem_res));
        return FALSE;
      }
    }
  }

  // UI
  
  return TRUE;
}

static b8 vulkan_create_descriptor_pool() {
  VkDescriptorPoolSize uniform_pool_size = {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = MAX_FRAMES_IN_FLIGHT *
    vulkan_context.uniform_object_count,
  };
  
  VkDescriptorPoolSize img_sampler_pool_size = {
    .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = MAX_FRAMES_IN_FLIGHT *
    vulkan_context.uniform_object_count,
  };

  VkDescriptorPoolSize pool_sizes[2] = {
    uniform_pool_size,
    img_sampler_pool_size,
  };

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
    .maxSets = MAX_FRAMES_IN_FLIGHT *
    vulkan_context.uniform_object_count,
    .poolSizeCount = 2,
    .pPoolSizes = pool_sizes,
  };

  VkResult create_descriptor_pool_res =
    vkCreateDescriptorPool(vulkan_context.logical_device,
			   &descriptor_pool_create_info,
                           vulkan_context.allocator,
			   &vulkan_context.descriptor_pool);

  if (create_descriptor_pool_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create descriptor pool: %s",
                string_VkResult(create_descriptor_pool_res));
    return FALSE;
  }

  return TRUE;
}

static b8 vulkan_create_descriptor_sets() {

  u32 object_count = darray_get_length(vulkan_context.objects);

  vulkan_context.pbr_descriptor_sets =
    mallocate(object_count *
	      sizeof(VkDescriptorSet) *
	      MAX_FRAMES_IN_FLIGHT,
	      MEMORY_TAG_RENDERER);// @TODO: Free mem

  for (u32 i = 0; i < object_count; i++) {

    VulkanDrawable object = vulkan_context.objects[i];

    VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
    
    for (u32 j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
      layouts[j] = vulkan_context.pbr_descriptor_set_layout;
    }

    VkDescriptorSetAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = vulkan_context.descriptor_pool,
      .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
      .pSetLayouts = layouts,
    };
    
    VkResult alloc_descriptor_set_res =
      vkAllocateDescriptorSets(vulkan_context.logical_device,
			       &alloc_info,
			       &vulkan_context.pbr_descriptor_sets
			       [i * MAX_FRAMES_IN_FLIGHT]);
    
    if (alloc_descriptor_set_res != VK_SUCCESS) {
      MERROR_CORE("Failed to allocate descriptor sets: %s",
                  string_VkResult(alloc_descriptor_set_res));
      return FALSE;
    }

    for (u32 j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
      VkDescriptorBufferInfo buffer_info = {
	.buffer = vulkan_context.uniform_bufs
	[(i * MAX_FRAMES_IN_FLIGHT) + j],
	.offset = 0,
	.range = VK_WHOLE_SIZE,
      };

      VkDescriptorImageInfo image_info = {
        .sampler = vulkan_context.texture_image_sampler,
        .imageView =
	vulkan_context.texture_image_views[object.texture_handle],
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      };

      VkWriteDescriptorSet uniform_write = {
	.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	.dstSet = vulkan_context.pbr_descriptor_sets
	[(i * MAX_FRAMES_IN_FLIGHT) + j],
	.dstBinding = 0,
	.dstArrayElement = 0,
	.descriptorCount = 1,
	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	.pBufferInfo = &buffer_info,
      };

      VkWriteDescriptorSet image_write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = vulkan_context.pbr_descriptor_sets
	[(i * MAX_FRAMES_IN_FLIGHT) + j],
        .dstBinding = 1,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &image_info,
      };

      VkWriteDescriptorSet writes[2] = {
        uniform_write,
        image_write,
      };

      vkUpdateDescriptorSets(vulkan_context.logical_device,
			     2, writes, 0, NULL_PTR);
    }
  }

  return TRUE;
}

static b8 vulkan_allocate_command_buffers() {
  VkCommandBufferAllocateInfo command_buffer_alloc_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = NULL_PTR,
    .commandPool = vulkan_context.graphics_command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
  };

  VkResult alloc_command_buffers_result =
    vkAllocateCommandBuffers(vulkan_context.logical_device,
			     &command_buffer_alloc_info,
			     vulkan_context.command_buffers);

  if (alloc_command_buffers_result != VK_SUCCESS) {
    MERROR_CORE("Failed to allocate vulkan command buffers: %s",
                string_VkResult(alloc_command_buffers_result));
    return FALSE;
  }
  return TRUE;
}

static b8 vulkan_create_sync_primatives() {

  vulkan_context.render_complete_semaphores =
    mallocate(vulkan_context.swapchain_image_count *
	      sizeof(VkSemaphore), MEMORY_TAG_RENDERER);

  VkSemaphoreCreateInfo present_complete_semaphore_create_info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = NULL_PTR,
  };

  VkSemaphoreCreateInfo render_complete_semaphore_create_info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = NULL_PTR,
  };

  VkFenceCreateInfo draw_fence_create_info = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkResult present_complete_create_semaphore_result =
      vkCreateSemaphore(vulkan_context.logical_device,
			&present_complete_semaphore_create_info,
                        vulkan_context.allocator,
			&vulkan_context.present_complete_semaphores[i]);

    if (present_complete_create_semaphore_result != VK_SUCCESS) {
      MERROR_CORE("Failed to create present complete semaphore: %s",
                  string_VkResult(present_complete_create_semaphore_result));
      return FALSE;
    }
  }

  for (u32 i = 0; i < vulkan_context.swapchain_image_count; i++) {

    VkResult render_complete_create_semaphore_result =
      vkCreateSemaphore(vulkan_context.logical_device,
			&render_complete_semaphore_create_info,
                        vulkan_context.allocator,
			&vulkan_context.render_complete_semaphores[i]);

    if (render_complete_create_semaphore_result != VK_SUCCESS) {
      MERROR_CORE("Failed to create render complete semaphore: %s",
                  string_VkResult(render_complete_create_semaphore_result));
      return FALSE;
    }
  }

  for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

    VkResult draw_create_fence_result =
      vkCreateFence(vulkan_context.logical_device,
		    &draw_fence_create_info,
                    vulkan_context.allocator,
		    &vulkan_context.draw_fences[i]);

    if (draw_create_fence_result != VK_SUCCESS) {
      MERROR_CORE("Failed to create draw vulkan fence: %s",
                  string_VkResult(draw_create_fence_result));
      return FALSE;
    }
  }
  return TRUE;
}

static b8 vulkan_create_swapchain(VkSwapchainKHR old_swapchain) {

  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkResult get_physical_device_surface_capabilities_result =
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_context.physical_device,
                                              vulkan_context.surface,
					      &surface_capabilities);

  if (get_physical_device_surface_capabilities_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get phys dev surface capabilities: %s",
                string_VkResult(get_physical_device_surface_capabilities_result));
    return FALSE;
  }

  VkImageUsageFlags image_usage_flags =
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  VkSwapchainCreateInfoKHR swapchain_create_info = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = NULL_PTR,
    .surface = vulkan_context.surface,
    .minImageCount = surface_capabilities.minImageCount,  // @TODO:
    // Add this as a setting in user code. (Maybe)
    .imageFormat = vulkan_context.surface_format.format,
    .imageColorSpace = vulkan_context.surface_format.colorSpace,
    .imageExtent = vulkan_context.swapchain_extent,
    .imageArrayLayers = 1,  // @MAGIC_NUMBER
    .imageUsage = image_usage_flags,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = ZERO,
    .pQueueFamilyIndices = NULL_PTR,
    .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
    .clipped = VK_TRUE,
    .oldSwapchain = old_swapchain,
  };

  VkResult create_swapchain_result =
    vkCreateSwapchainKHR(vulkan_context.logical_device,
			 &swapchain_create_info,
                         vulkan_context.allocator,
			 &vulkan_context.swapchain);

  if (create_swapchain_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create Vulkan swapchain: %s",
		string_VkResult(create_swapchain_result));
    return FALSE;
  } 

  return TRUE;
}

static b8 vulkan_recreate_swapchain(u16 width, u16 height) {

  VkResult queue_wait_idle_result =
    vkQueueWaitIdle(vulkan_context.graphics_queue);

  if (queue_wait_idle_result != VK_SUCCESS) {
    MWARN_CORE("Failed to wait for queue idle: %s",
               string_VkResult(queue_wait_idle_result));
  }

  if (!vulkan_set_surface_extent(&vulkan_context, width, height)) {
    MERROR_CORE("Failed to set surface extent");
    return FALSE;
  }

  VkSwapchainKHR old_swapchain = vulkan_context.swapchain;

  if (!vulkan_create_swapchain(old_swapchain)) {
    MERROR_CORE("Failed to create swapchain");
    return FALSE;
  }

  for (u32 i = 0; i < vulkan_context.swapchain_image_count; i++) {
    vkDestroyImageView(vulkan_context.logical_device,
		       vulkan_context.swapchain_image_views[i],
                       vulkan_context.allocator);
  }

  vkDestroySwapchainKHR(vulkan_context.logical_device,
			old_swapchain, vulkan_context.allocator);

  if (!vulkan_get_swapchain_images()) {
    MERROR_CORE("Failed to get swapchain images");
    return FALSE;
  }

  if (!vulkan_create_image_views()) {
    MERROR_CORE("Failed to create image views");
    return FALSE;
  }

  if (!vulkan_create_depth_resources()) {
    MERROR_CORE("Failed to  create depth resources");
    return FALSE;
  }

  return TRUE;
}

/* static b8 vulkan_cleanup_swapchain() { return TRUE; } */

static void transition_image_layout(VkImage image,
				    VkImageLayout old_layout,
                                    VkImageLayout new_layout,
				    VkAccessFlags2 src_access_mask,
                                    VkAccessFlags2 dst_access_mask,
                                    VkPipelineStageFlags2 src_stage_mask,
                                    VkPipelineStageFlags2 dst_stage_mask,
                                    VkImageAspectFlags aspect) {

  VkImageSubresourceRange subresource_range = {
    .aspectMask = aspect,
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1,
  };

  VkImageMemoryBarrier2 image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
    .srcStageMask = src_stage_mask,
    .dstStageMask = dst_stage_mask,
    .srcAccessMask = src_access_mask,
    .dstAccessMask = dst_access_mask,
    .oldLayout = old_layout,
    .newLayout = new_layout,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = image,
    .subresourceRange = subresource_range,
  };

  VkDependencyInfo dependency_info = {
    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
    .dependencyFlags = ZERO,
    .imageMemoryBarrierCount = 1,
    .pImageMemoryBarriers = &image_memory_barrier,
  };

  vkCmdPipelineBarrier2(vulkan_context.command_buffers
			[vulkan_context.current_frame_index],
                        &dependency_info);
}

static b8 create_buffer(VkBuffer* buffer, VkDeviceMemory* device_mem,
			VkDeviceSize size,
			VkBufferUsageFlags usage_flags,
			VkMemoryPropertyFlags prop_flags) {

  u32 queue_family_indices[] = {
    vulkan_context.transfer_queue_family_index,
    vulkan_context.graphics_queue_family_index
  };

  VkBufferCreateInfo buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = size,
    .usage = usage_flags,
    .queueFamilyIndexCount = vulkan_context.queue_family_index_count,
    .pQueueFamilyIndices = queue_family_indices,
  };

  if (vulkan_context.queue_family_index_count == 1) {
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  } else {
    buffer_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
  }


  VkResult create_buffer_result =
    vkCreateBuffer(vulkan_context.logical_device,
		   &buffer_create_info,
                   vulkan_context.allocator, buffer);

  if (create_buffer_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan buffer: %s",
		string_VkResult(create_buffer_result));
    return FALSE;
  }

  VkMemoryRequirements buffer_mem_requirements;
  vkGetBufferMemoryRequirements(vulkan_context.logical_device,
				*buffer, &buffer_mem_requirements);

  VkMemoryAllocateInfo mem_alloc_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .allocationSize = buffer_mem_requirements.size,
    .memoryTypeIndex =
    get_memory_type(buffer_mem_requirements.memoryTypeBits,
		    prop_flags),
  };

  VkResult alloc_mem_res =
    vkAllocateMemory(vulkan_context.logical_device, &mem_alloc_info,
		     vulkan_context.allocator, device_mem);

  if (alloc_mem_res != VK_SUCCESS) {
    MERROR_CORE("Failed to allocate vulkan device memory: %s",
		string_VkResult(alloc_mem_res));
    return FALSE;
  }

  VkResult bind_buffer_mem =
    vkBindBufferMemory(vulkan_context.logical_device, *buffer,
		       *device_mem, 0);

  if (bind_buffer_mem != VK_SUCCESS) {
    MERROR_CORE("Failed to bind vulkan buffer memory: %s",
		string_VkResult(bind_buffer_mem));
  }
  return TRUE;
}

static b8 copy_buffer(VkBuffer* src_buffer, VkBuffer* dst_buffer,
		      VkDeviceSize size) {

  VkCommandBufferAllocateInfo alloc_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = vulkan_context.transfer_command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  VkCommandBuffer copy_cmd_buf;
  VkResult create_cmd_buf_res =
    vkAllocateCommandBuffers(vulkan_context.logical_device,
			     &alloc_info, &copy_cmd_buf);

  if (create_cmd_buf_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create copy command buffer: %s",
		string_VkResult(create_cmd_buf_res));
  }

  VkCommandBufferBeginInfo cmd_buf_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  VkResult begin_cmd_buf_res =
    vkBeginCommandBuffer(copy_cmd_buf, &cmd_buf_begin_info);

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

  Entity* p_cam_ent = &vulkan_context.scene_data->entities[vulkan_context.scene_data->active_camera_entity];
  CCamera* p_cam = entity_get_camera(p_cam_ent);

  MVPMat mvp_mat;
  mvp_mat.view = mat4_transpose(mat4_inverse_transform(mat4_from_transform(p_cam_ent->transform)));
  mvp_mat.proj = mat4_perspective(M_TO_RAD(p_cam->fov),
                                  (f32)vulkan_context.swapchain_extent.width /
                                  (f32)vulkan_context.swapchain_extent.height,
                                  p_cam->near_plane, p_cam->far_plane);
  mvp_mat.proj.e22 *= -1.0f;

  for (u32 i = 0; i < vulkan_context.uniform_object_count; i++) {
    mvp_mat.model = mat4_transpose(mat4_from_transform(vulkan_context.scene_data->entities[vulkan_context.scene_data->drawable_handles[i]].transform));

    mcopy_memory(vulkan_context.uniform_buf_mem_map[(i * MAX_FRAMES_IN_FLIGHT) + vulkan_context.current_frame_index],
                 &mvp_mat, sizeof(mvp_mat));
  }

  return TRUE;
}

static b8 create_image(VkImage* image, VkDeviceMemory* mem, u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags props) {

  VkImageCreateInfo image_create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
  image_create_info.imageType = VK_IMAGE_TYPE_2D;
  image_create_info.format = format;
  image_create_info.extent.width = width;
  image_create_info.extent.height = height;
  image_create_info.extent.depth = 1;
  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.tiling = tiling;
  image_create_info.usage = usage;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult create_image_res = vkCreateImage(vulkan_context.logical_device, &image_create_info, vulkan_context.allocator, image);
  
  if (create_image_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create teture image: %s", string_VkResult(create_image_res));
    return FALSE;
  }

  VkMemoryRequirements mem_req;
  vkGetImageMemoryRequirements(vulkan_context.logical_device, *image, &mem_req);

  VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  alloc_info.allocationSize = mem_req.size;
  alloc_info.memoryTypeIndex = get_memory_type(mem_req.memoryTypeBits, props);
  
  VkResult alloc_mem_res = vkAllocateMemory(vulkan_context.logical_device, &alloc_info, vulkan_context.allocator, mem);
  
  if (alloc_mem_res != VK_SUCCESS) {
    MERROR_CORE("Failed to allocate vulkan image device memory: %s", string_VkResult(alloc_mem_res));
    return FALSE;
  }

  VkResult bind_image_mem_res = vkBindImageMemory(vulkan_context.logical_device, *image, *mem, 0);

  if (bind_image_mem_res != VK_SUCCESS) {
    MERROR_CORE("Failed to bind vulkan image memory: %s", string_VkResult(bind_image_mem_res));
    return FALSE;
  }

  return TRUE;
}

static b8 create_image_view(VkImageView* view, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, b8 rgb_flipped) {

  VkComponentMapping components = {
    .r = VK_COMPONENT_SWIZZLE_R,
    .g = VK_COMPONENT_SWIZZLE_G,
    .b = VK_COMPONENT_SWIZZLE_B,
    .a = VK_COMPONENT_SWIZZLE_A,
  };

  if (rgb_flipped) {
    components.r = VK_COMPONENT_SWIZZLE_B;
    components.b = VK_COMPONENT_SWIZZLE_R;
  }

  VkImageSubresourceRange subresource_range = {
    .aspectMask = aspect_flags,
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1,
  };

  VkImageViewCreateInfo view_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .flags = ZERO,
    .image = image,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = format,
    .components = components,
    .subresourceRange = subresource_range,
  };

  VkResult create_image_view_res = vkCreateImageView(vulkan_context.logical_device, &view_create_info, vulkan_context.allocator, view);
  if (create_image_view_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create image view: %s", string_VkResult(create_image_view_res));
    return FALSE;
  }

  return TRUE;
}

static VkCommandBuffer begin_single_time_commands(VkCommandPool command_pool) {
  VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  alloc_info.commandPool = command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer command_buf;

  VkResult alloc_cmd_buf_res = vkAllocateCommandBuffers(vulkan_context.logical_device, &alloc_info, &command_buf);
  if (alloc_cmd_buf_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan command buffer: %s", string_VkResult(alloc_cmd_buf_res));
    return VK_NULL_HANDLE;
  }

  VkCommandBufferBeginInfo begin_cmd_buf_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  begin_cmd_buf_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
  VkResult begin_cmd_buf_res = vkBeginCommandBuffer(command_buf, &begin_cmd_buf_info);
  if (begin_cmd_buf_res != VK_SUCCESS) {
    MERROR_CORE("Failed to begin command buffer: %s", string_VkResult(begin_cmd_buf_res));
    return VK_NULL_HANDLE;
  }

  return command_buf;
}

static b8 end_single_time_commands(VkCommandBuffer* cmd_buf, VkQueue queue) {
  VkResult end_cmd_buf_res = vkEndCommandBuffer(*cmd_buf);
  if (end_cmd_buf_res != VK_SUCCESS) {
    MERROR_CORE("Failed to end command buffer: %s", string_VkResult(end_cmd_buf_res));
    return FALSE;
  }

  VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = cmd_buf;

  VkResult queue_submit_res = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
  if (queue_submit_res != VK_SUCCESS) {
    MERROR_CORE("Failed to submit to queue: %s", string_VkResult(queue_submit_res));
    return FALSE;
  }

  VkResult wait_idle_res = vkQueueWaitIdle(queue);
  if (wait_idle_res != VK_SUCCESS) {
    MWARN_CORE("Failed to wait for queue idle: %s", string_VkResult(wait_idle_res));
  }

  return TRUE;
}

static b8 transition_tex_image_layout(VkCommandBuffer* cmd_buf, const VkImage* image, VkImageLayout old_layout, VkImageLayout new_layout) {

  VkImageSubresourceRange subresource_range = {};
  subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresource_range.levelCount = 1;
  subresource_range.layerCount = 1;

  VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = *image;
  barrier.subresourceRange = subresource_range;

  VkPipelineStageFlags src_stage;
  VkPipelineStageFlags dst_stage;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = ZERO;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

  } else {
    MERROR_CORE("Unsupported layout transition %s -> %s", string_VkImageLayout(old_layout), string_VkImageLayout(new_layout));
    return FALSE;
  }

  vkCmdPipelineBarrier(*cmd_buf, src_stage, dst_stage, ZERO, 0, NULL_PTR, 0, NULL_PTR, 1, &barrier);

  return TRUE;
}

static b8 copy_buffer_to_image(VkCommandBuffer* cmd_buf, VkBuffer* buf, VkImage* image, u32 width, u32 height) {

  VkImageSubresourceLayers layers;
  layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  layers.mipLevel = 0;
  layers.baseArrayLayer = 0;
  layers.layerCount = 1;

  VkOffset3D offset = {.x = 0, .y = 0, .z = 0};
  VkExtent3D extent = {.width = width, .height = height, .depth = 1};

  VkBufferImageCopy buf_image_copy;
  buf_image_copy.bufferOffset = 0;
  buf_image_copy.bufferRowLength = 0;
  buf_image_copy.bufferImageHeight = 0;
  buf_image_copy.imageSubresource = layers;
  buf_image_copy.imageOffset = offset;
  buf_image_copy.imageExtent = extent;
  
  vkCmdCopyBufferToImage(*cmd_buf, *buf, *image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buf_image_copy);

  return TRUE;
}

static VkFormat find_supported_format(const VkFormat* formats, u32 format_count, VkImageTiling tiling, VkFormatFeatureFlags flags) {
  for (u32 i = 0; i < format_count; i++) {

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(vulkan_context.physical_device, formats[i], &props);

    if (((tiling == VK_IMAGE_TILING_LINEAR) && ((props.linearTilingFeatures & flags) == flags)) ||
        ((tiling == VK_IMAGE_TILING_OPTIMAL) && ((props.optimalTilingFeatures & flags) == flags))) {
      return formats[i];
    }
  }
  MWARN_CORE("Couldn't find supported format with correct features");
  return ZERO;
}

static VkFormat find_supported_depth_format() {

  VkFormat formats[3] = {
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT_S8_UINT,
  };

  return find_supported_format(formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
