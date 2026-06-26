/**
   @file vulkan_pipeline.h
 */

#pragma once

#include "define.h"
#include "vulkan_defines.h"
#include "renderer/renderer_types.h"

/**
 * @brief Creates the vulkan pipeline for objects within the 3D world.
 *
 * @return TRUE if the pipeline was created successfully, otherwise FALSE.
 */
b8 vulkan_create_pbr_pipeline(VulkanContext* context);  // @TODO: Split this into more atomic functions.

b8 vulkan_create_ui_pipeline(VulkanContext* context);

VkVertexInputBindingDescription get_vertex_binding_description(VertexType type);

VkVertexInputAttributeDescription* get_vertex_attribute_descriptions();

b8 vulkan_create_shader_module(VulkanContext* context, VkShaderModule* module, const char* path);
