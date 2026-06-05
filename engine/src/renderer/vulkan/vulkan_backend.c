#include "vulkan_backend.h"

#include <vulkan/vk_enum_string_helper.h>

#include "core/log.h"
#include "core/mmemory.h"
#include "define.h"
#include "renderer/vulkan/vulkan_helper.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_defines.h"
#include "vulkan_wsi.h"

static VulkanContext vulkan_context;

// @TODO: Split this up into functions comments in CAPS roughly outline different functions.
b8 vulkan_backend_init(RendererBackend* renderer_backend, const char* application_name,
                       i16 start_width, i16 start_height, PlatformState* platform_state) {
  vulkan_context.allocator = NULL_PTR;
  vulkan_context.debug_messenger = NULL_PTR;

  // SETUP INSTANCE ----------
  // Set vulkan app info
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

  // SETUP DEBUG MESSAGING
  // Set which message severities will call the debug messenger
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

  // SETUP PHYSICAL DEVICE ----------
  // Get physical device count
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

  // Create surface
  if (!vulkan_create_platform_surface(&vulkan_context, platform_state)) {
    MERROR_CORE("Failed to create vulkan platform surface");
    return FALSE;
  }

  // Check available device extensions (for debugging)
  /* u32 device_extension_property_count = 0; */
  /* vkEnumerateDeviceExtensionProperties(suitable_device, NULL_PTR,
   * &device_extension_property_count, */
  /*                                      NULL_PTR); */
  /* VkExtensionProperties device_extension_properties[device_extension_property_count]; */
  /* vkEnumerateDeviceExtensionProperties(suitable_device, NULL_PTR,
   * &device_extension_property_count, */
  /*                                      device_extension_properties); */
  /* MTRACE_CORE("Available device extensions:"); */
  /* for (u32 i = 0; i < device_extension_property_count; i++) { */
  /*   MTRACE_CORE("%u: %s", i + 1, device_extension_properties[i].extensionName); */
  /* } */

  // SETUP LOGICAL DEVICE ----------
  // Select suitable queue family index for graphics
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

  // Set queue create info

  f32 temp_priority = 1.0f;  // @MAGIC_NUMBER
  VkDeviceQueueCreateInfo device_queue_create_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  device_queue_create_info.queueFamilyIndex = vulkan_context.graphics_queue_family_index;
  device_queue_create_info.queueCount = 1;  // @MAGIC_NUMBER
  device_queue_create_info.pQueuePriorities = &temp_priority;

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
  device_create_info.queueCreateInfoCount = 1;
  device_create_info.pQueueCreateInfos = &device_queue_create_info;
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

  // Get surface extent
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
  if (surface_capabilities.currentExtent.width != UINT32_MAX) {  // Surface provides specific size
    vulkan_context.swapchain_extent = surface_capabilities.currentExtent;
  } else {  // Application can provide size
    // Set width within acceptable Image Extent range
    if ((u32)start_width > surface_capabilities.maxImageExtent.width) {
      vulkan_context.swapchain_extent.width = surface_capabilities.maxImageExtent.width;
    } else if ((u32)start_width < surface_capabilities.minImageExtent.width) {
      vulkan_context.swapchain_extent.width = surface_capabilities.minImageExtent.width;
    } else {
      vulkan_context.swapchain_extent.width = (u32)start_width;
    }

    // Set width within acceptable Image Extent range
    if ((u32)start_height > surface_capabilities.maxImageExtent.height) {
      vulkan_context.swapchain_extent.height = surface_capabilities.maxImageExtent.height;
    } else if ((u32)start_height < surface_capabilities.minImageExtent.height) {
      vulkan_context.swapchain_extent.height = surface_capabilities.minImageExtent.height;
    } else {
      vulkan_context.swapchain_extent.height = (u32)start_height;
    }
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
                string_VkResult(get_physical_device_surface_capabilities_result));
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

  // CREATE SWAPCHAIN
  VkImageUsageFlags image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  swapchain_create_info.pNext = NULL_PTR;
  swapchain_create_info.surface = vulkan_context.surface;
  swapchain_create_info.minImageCount = 4;  // @TODO: Add this as a setting in user code. (Maybe)
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
  swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

  VkResult create_swapchain_result =
      vkCreateSwapchainKHR(vulkan_context.logical_device, &swapchain_create_info,
                           vulkan_context.allocator, &vulkan_context.swapchain);

  if (create_swapchain_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create Vulkan swapchain: %s", string_VkResult(create_swapchain_result));
    return FALSE;
  } else {
    MINFO_CORE("Created Vulkan swapchain");
  }

  // Get swapchain images
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

  // Create image views
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

  // CREATE GRAPHICS PIPELINE
  // Read shader byte code
  u8* shader_bin = NULL_PTR;
  u64 shader_bin_size = 0;
  vulkan_read_shader_binary("../engine/src/renderer/vulkan/shaders/hello_triangle.spv",
                            &shader_bin_size, &shader_bin);

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

  // Set vertex input state create info @TODO: Set this up
  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

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
  pipeline_layout_create_info.setLayoutCount = 0;
  pipeline_layout_create_info.pushConstantRangeCount = 0;

  VkPipelineLayout pipeline_layout;
  VkResult create_pipeline_layout_result =
      vkCreatePipelineLayout(vulkan_context.logical_device, &pipeline_layout_create_info,
                             vulkan_context.allocator, &pipeline_layout);
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
  graphics_pipeline_create_info.layout = pipeline_layout;
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

  // CREATE COMMAND POOL
  VkCommandPoolCreateFlags command_pool_create_flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkCommandPoolCreateInfo command_pool_create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  command_pool_create_info.pNext = NULL_PTR;
  command_pool_create_info.flags = command_pool_create_flags;
  command_pool_create_info.queueFamilyIndex = vulkan_context.graphics_queue_family_index;

  VkResult create_command_pool_result =
      vkCreateCommandPool(vulkan_context.logical_device, &command_pool_create_info,
                          vulkan_context.allocator, &vulkan_context.command_pool);

  if (create_command_pool_result != VK_SUCCESS) {
    MERROR_CORE("Failed to create vulkan command pool: %s",
                string_VkResult(create_command_pool_result));
    return FALSE;
  }

  // ALLOC COMMAND BUFFER
  VkCommandBufferAllocateInfo command_buffer_alloc_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  command_buffer_alloc_info.pNext = NULL_PTR;
  command_buffer_alloc_info.commandPool = vulkan_context.command_pool;
  command_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_info.commandBufferCount = 1;

  VkResult alloc_command_buffers_result = vkAllocateCommandBuffers(
      vulkan_context.logical_device, &command_buffer_alloc_info, &vulkan_context.command_buffer);

  if (alloc_command_buffers_result != VK_SUCCESS) {
    MERROR_CORE("Failed to allocate vulkan command buffers: %s",
                string_VkResult(alloc_command_buffers_result));
    return FALSE;
  }

  vulkan_context.current_image_index = 0;
  return TRUE;
}

void vulkan_backend_shutdown(RendererBackend* renderer_backend) {
  vkDestroyCommandPool(vulkan_context.logical_device, vulkan_context.command_pool,
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
  // Begin command buffer recording
  VkCommandBufferBeginInfo command_buffer_begin_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  command_buffer_begin_info.flags = ZERO;
  command_buffer_begin_info.pInheritanceInfo = NULL_PTR;

  VkResult begin_command_buffer_result =
      vkBeginCommandBuffer(vulkan_context.command_buffer, &command_buffer_begin_info);
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
  vkCmdBeginRendering(vulkan_context.command_buffer, &rendering_info);
  vkCmdBindPipeline(vulkan_context.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vulkan_context.graphics_pipeline);

  // Set viewport as scissor values
  VkViewport viewport = {.x = 0.0f,
                         .y = 0.0f,
                         .width = (f32)vulkan_context.swapchain_extent.width,
                         .height = (f32)vulkan_context.swapchain_extent.height,
                         .minDepth = 0.0f,
                         .maxDepth = 1.0f};

  vkCmdSetViewport(vulkan_context.command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {.offset = {.x = 0, .y = 0}, .extent = vulkan_context.swapchain_extent};

  vkCmdSetScissor(vulkan_context.command_buffer, 0, 1, &scissor);

  vkCmdDraw(vulkan_context.command_buffer, 3, 1, 0, 0);

  return TRUE;
}

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time) {
  // End rendering
  vkCmdEndRendering(vulkan_context.command_buffer);

  transition_image_layout(
      vulkan_context.current_image_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, ZERO,
      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

  // End command buffer recording
  VkResult end_command_buffer_result = vkEndCommandBuffer(vulkan_context.command_buffer);
  if (end_command_buffer_result != VK_SUCCESS) {
    MERROR_CORE("Failed to end vulkan command buffer: %s",
                string_VkResult(end_command_buffer_result));
    return FALSE;
  }
  return TRUE;
}

b8 vulkan_backend_draw_frame(RendererBackend* renderer_backend) {

  return TRUE;
}  

void vulkan_backend_resized(RendererBackend* renderer_backend, u16 width, u16 height) {}

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

  vkCmdPipelineBarrier2(vulkan_context.command_buffer, &dependency_info);
}
