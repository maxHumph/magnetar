#include "vulkan_pipeline.h"

#include <vulkan/vk_enum_string_helper.h>

#include "core/mmemory.h"

#include "vulkan_helper.h"

b8 vulkan_create_object_pipeline(VulkanContext* context) {

  VkShaderModule shader_module;
  if(!vulkan_create_shader_module(context, &shader_module,
                                  "../engine/src/renderer/vulkan/shaders/basic.spv")) {
    MERROR_CORE("Failed to create shader module for object pipeline");
    return FALSE;
  }

  // Create vertex shader stage create info
  VkPipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info = {
   .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
   .pNext = NULL_PTR,
   .flags = ZERO,
   .stage = VK_SHADER_STAGE_VERTEX_BIT,
   .module = shader_module,
   .pName = "vertMain",
   .pSpecializationInfo = NULL_PTR,
  };

  // Create fragment shader stage create info
  VkPipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = ZERO,
    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
    .module = shader_module,
    .pName = "fragMain",
    .pSpecializationInfo = NULL_PTR,
  };

  VkPipelineShaderStageCreateInfo shader_stage_create_infos[] = {
    vert_pipeline_shader_stage_create_info,
    frag_pipeline_shader_stage_create_info,
  };

  // Set dynamic states
  VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
   .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
   .pNext = NULL_PTR,
   .dynamicStateCount = sizeof(dynamic_states) / sizeof(VkDynamicState),
   .pDynamicStates = dynamic_states,
  };

  // Set vertex input state create info
  VkVertexInputBindingDescription vertex_binding_description = get_vertex_binding_description();
  VkVertexInputAttributeDescription* vertex_attribute_descriptions =
    get_vertex_attribute_descriptions();

  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
   .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
   .vertexBindingDescriptionCount = 1,
   .pVertexBindingDescriptions = &vertex_binding_description,
   .vertexAttributeDescriptionCount = MVK_VERTEX_ATTRIBUTE_COUNT,
   .pVertexAttributeDescriptions = vertex_attribute_descriptions,
  };

  // Set input assembly state create info
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .primitiveRestartEnable = VK_FALSE,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  };

  // Create viewport
  VkViewport viewport = {
    .x = 0.0f,
    .y = 0.0f,
    .width = context->swapchain_extent.width,
    .height = context->swapchain_extent.height,
    .minDepth = 0.0f,
    .maxDepth = 1.0f,
  };

  // Specify scissor rect
  VkOffset2D scissor_rect_offset = {
    .x = 0,
    .y = 0,
  };

  VkRect2D scissor_rect = {
    .offset = scissor_rect_offset,
    .extent = context->swapchain_extent,
  };

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .viewportCount = 1,  // @MAGIC_NUMBER
    /* .pViewports = &viewport, */
    .scissorCount = 1,  //@MAGIC_NUMBER
    /* .pScissors = &scissor_rect, */
  };

  // Set rasteriser create info
  VkCullModeFlags cull_mode_flags = VK_CULL_MODE_BACK_BIT;

  VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = cull_mode_flags,
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .lineWidth = 1.0f,  // @MAGIC_NUMBER
  };

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = VK_FALSE,
    .stencilTestEnable = VK_FALSE,
  };

  // Set multisampling opts
  VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
  };

  // Specify colour blending options
  VkColorComponentFlags color_component_flags =
    VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT
    ;

  VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
    .blendEnable = VK_TRUE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    .colorWriteMask = color_component_flags,
  };

  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,  // @MAGIC_NUMBER
    .pAttachments = &color_blend_attachment_state,
  };

  // Create pipeline layout
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = NULL_PTR,
    .setLayoutCount = 1,
    .pSetLayouts = &context->descriptor_set_layout,
    .pushConstantRangeCount = 0,
  };

  VkResult create_pipeline_layout_result =
    vkCreatePipelineLayout(context->logical_device, &pipeline_layout_create_info,
                           context->allocator, &context->pbr_pipeline_layout);
  if (create_pipeline_layout_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan pipeline layout: %s",
                string_VkResult(create_pipeline_layout_result));
    return FALSE;
  }

  // Set graphics pipeline create info
  VkPipelineRenderingCreateInfo pipeline_rendering_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
    .pNext = NULL_PTR,
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &context->surface_format.format,
    .depthAttachmentFormat = context->depth_format,
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = &pipeline_rendering_create_info,
    .stageCount = 2,  // @MAGIC_NUMBER
    .pStages = shader_stage_create_infos,
    .pVertexInputState = &vertex_input_state_create_info,
    .pInputAssemblyState = &input_assembly_state_create_info,
    .pTessellationState = NULL_PTR,
    .pViewportState = &viewport_state_create_info,
    .pRasterizationState = &rasterization_state_create_info,
    .pMultisampleState = &multisample_state_create_info,
    .pDepthStencilState = &depth_stencil_state_create_info,
    .pColorBlendState = &color_blend_state_create_info,
    .pDynamicState = &dynamic_state_create_info,
    .layout = context->pbr_pipeline_layout,
    .renderPass = NULL_PTR,
  };

  // Create graphics pipeline
  VkResult create_graphics_pipelines_result = vkCreateGraphicsPipelines(
                                                                        context->logical_device, NULL_PTR, 1, &graphics_pipeline_create_info,
                                                                        context->allocator, &context->pbr_pipeline);

  if (create_graphics_pipelines_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan graphics pipeline: %s",
                string_VkResult(create_graphics_pipelines_result));
    return FALSE;
  }
  return TRUE;
}

b8 vulkan_create_ui_pipeline(VulkanContext* context) {
  VkShaderModule shader_module;
  if(!vulkan_create_shader_module(context, &shader_module,
                                  "../engine/src/renderer/vulkan/shaders/basic.spv")) {
    MERROR_CORE("Failes to create shader module for UI pipeline");
    return FALSE;
  }



  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .dynamicStateCount = sizeof(dynamic_states) / sizeof(VkDynamicState),
    .pDynamicStates = dynamic_states,
  };

  // VERTEX INPUT

  VkVertexInputBindingDescription vert_binding_desc = get_vertex_binding_description();
  VkVertexInputAttributeDescription* vert_attr_descs = get_vertex_attribute_descriptions();

  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .vertexBindingDescriptionCount = 1,
    .pVertexBindingDescriptions = &vert_binding_desc,
    .vertexAttributeDescriptionCount = MVK_VERTEX_ATTRIBUTE_COUNT,
    .pVertexAttributeDescriptions = vert_attr_descs,
  };

  // INPUT ASSEMBLY

  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = ZERO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };

  // VERTEX SHADER

  VkPipelineShaderStageCreateInfo vert_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = ZERO,
    .stage = VK_SHADER_STAGE_VERTEX_BIT,
    .module = shader_module,
    .pName = "vertMain",
    .pSpecializationInfo = NULL_PTR,
  };

  // VIEWPORT AND SCISSOR

  VkViewport viewport = {
    .x = 0.0f,
    .y = 0.0f,
    .width = context->swapchain_extent.width,
    .height = context->swapchain_extent.height,
    .minDepth = 0.0f,
    .maxDepth = 1.0f,
  };

  VkOffset2D scissor_offset = {
    .x = 0,
    .y = 0,
  };

  VkRect2D scissor = {
    .offset = scissor_offset,
    .extent = context->swapchain_extent,
  };

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = ZERO,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };

  // RASTERIAZATION

  VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = 0,
    .depthClampEnable = VK_TRUE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .lineWidth = 1.0f,
  };

  // DEPTH STENCIL TEST

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = 0,
    .depthTestEnable = VK_FALSE,
    .depthWriteEnable = VK_FALSE,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = VK_FALSE,
    .stencilTestEnable = VK_FALSE,
  };

  // FRAGMENT SHADER

  VkPipelineShaderStageCreateInfo frag_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = ZERO,
    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
    .module = shader_module,
    .pName = "fragMain",
    .pSpecializationInfo = NULL_PTR,
  };

  // MULTISAMPLING

  VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
  };

  // COLOUR BLENDING

  VkColorComponentFlags color_component_flags =
    VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
    .blendEnable = VK_TRUE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    .colorWriteMask = color_component_flags,
  };

  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = NULL_PTR,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &color_blend_attachment_state,
    
  };


  // CREATE PIPELINE LAYOUT
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = NULL_PTR,
    .flags = 0,
    .setLayoutCount = 1,
    .pSetLayouts = &context->descriptor_set_layout, // @TODO: Create separate descriptor layout for ui.
    .pushConstantRangeCount = 0,
  };

  VkResult create_pipeline_layout_res =
    vkCreatePipelineLayout(context->logical_device,
                           &pipeline_layout_create_info,
                           context->allocator,
                           &context->ui_pipeline_layout);

  if (create_pipeline_layout_res != VK_SUCCESS) {
    MERROR_CORE("Failed to create ui pipeline layout",
                string_VkResult(create_pipeline_layout_res));
    return FALSE;
  }

  // SHADER STAGES LIST

  VkPipelineShaderStageCreateInfo shader_stage_create_infos[] = {
    vert_stage_create_info,
    frag_stage_create_info,
  };

  // CREATE PIPELINE

  VkPipelineRenderingCreateInfo pipeline_rendering_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
    .pNext = NULL_PTR,
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &context->surface_format.format,
    .depthAttachmentFormat = context->depth_format,
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = &pipeline_rendering_create_info,
    .stageCount = 2,
    .pStages = shader_stage_create_infos,
    .pVertexInputState = &vertex_input_state_create_info,
    .pInputAssemblyState = &input_assembly_state_create_info,
    .pTessellationState = NULL_PTR,
    .pViewportState = &viewport_state_create_info,
    .pRasterizationState = &rasterization_state_create_info,
    .pMultisampleState = &multisample_state_create_info,
    .pDepthStencilState = &depth_stencil_state_create_info,
    .pColorBlendState = &color_blend_state_create_info,
    .pDynamicState = &dynamic_state_create_info,
    .layout = context->ui_pipeline_layout,
    .renderPass = NULL_PTR,
  };

  VkResult create_graphics_pipeline_result =
    vkCreateGraphicsPipelines(context->logical_device,
                              VK_NULL_HANDLE,
                              1,
                              &graphics_pipeline_create_info,
                              context->allocator,
                              &context->ui_pipeline);

  return TRUE; 
}

VkVertexInputBindingDescription get_vertex_binding_description() {
  VkVertexInputBindingDescription out = {
    .binding = 0,
    .stride = sizeof(Vertex),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };
  return out;
}

VkVertexInputAttributeDescription* get_vertex_attribute_descriptions() {
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

b8 vulkan_create_shader_module(VulkanContext* context, VkShaderModule* module, const char* path){
  // Read shader byte code
  u8* shader_bin = NULL_PTR;
  u64 shader_bin_size = 0;
  vulkan_read_shader_binary(path, &shader_bin_size,
                            &shader_bin);

  // Create shader module
  VkShaderModuleCreateInfo shader_module_create_info = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = NULL_PTR,
    .codeSize = shader_bin_size,
    .pCode = (u32*)shader_bin,
  };


  VkResult create_shader_module_result =
    vkCreateShaderModule(context->logical_device, &shader_module_create_info,
                         context->allocator, module);
  if (create_shader_module_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan shader module: %s",
                string_VkResult(create_shader_module_result));
    return FALSE;
  }

  mfree(shader_bin, shader_bin_size * sizeof(u8), MEMORY_TAG_RENDERER);
  return TRUE;
}
