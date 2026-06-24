#include "vulkan_device.h"

#include "core/mmemory.h"
#include "vulkan_wsi.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

b8 vulkan_select_physical_device(VulkanContext* context) {
  u32 physical_device_count = 0;
  VkResult enumerate_physical_devices_result =
    vkEnumeratePhysicalDevices(context->instance, &physical_device_count, NULL_PTR);
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
    vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices);
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
  context->physical_device = suitable_device;

  vkGetPhysicalDeviceProperties(context->physical_device, &context->physical_device_properties);

  mfree(physical_devices, physical_device_count * sizeof(VkPhysicalDevice), MEMORY_TAG_RENDERER);
  return TRUE;
}

b8 vulkan_get_surface(VulkanContext* context, PlatformState* platform_state) {
  if (!vulkan_create_platform_surface(context, platform_state)) {
    MERROR_CORE("Failed to create vulkan platform surface");
    return FALSE;
  }

  if (!vulkan_set_surface_extent(context, context->swapchain_extent.width,
                                 context->swapchain_extent.height)) {
    MERROR_CORE("Vulkan Get Surface: Failed to set surface extent.");
    return FALSE;
  }

  // Get surface formats
  u32 surface_format_count = 0;
  VkResult get_physical_device_surface_formats_count_result = vkGetPhysicalDeviceSurfaceFormatsKHR(
                                                                                                   context->physical_device, context->surface, &surface_format_count, NULL_PTR);
  if (get_physical_device_surface_formats_count_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get vulkan physical device surface format count: %s",
                string_VkResult(get_physical_device_surface_formats_count_result));
    return FALSE;
  }
  VkSurfaceFormatKHR surface_formats[surface_format_count];
  VkResult get_physical_device_surface_formats_result =
    vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, context->surface,
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
  context->surface_format = surface_formats[selected_format];
  return TRUE;
}

b8 vulkan_set_surface_extent(VulkanContext* context, i16 width, i16 height) {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkResult get_physical_device_surface_capabilities_result =
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physical_device,
                                              context->surface, &surface_capabilities);

  if (get_physical_device_surface_capabilities_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get physical device surface capabilities: %s",
                string_VkResult(get_physical_device_surface_capabilities_result));
    return FALSE;
  }

  // Sets the image extent to an acceptable value
  if (surface_capabilities.currentExtent.width != UINT32_MAX &&
      surface_capabilities.currentExtent.width != 0) {  // Surface provides specific size
    context->swapchain_extent = surface_capabilities.currentExtent;
  } else {  // Application can provide size
    // Set width within acceptable Image Extent range
    if ((u32)width > surface_capabilities.maxImageExtent.width) {
      context->swapchain_extent.width = surface_capabilities.maxImageExtent.width;
    } else if ((u32)width < surface_capabilities.minImageExtent.width) {
      context->swapchain_extent.width = surface_capabilities.minImageExtent.width;
    } else {
      context->swapchain_extent.width = (u32)width;
    }

    // Set width within acceptable Image Extent range
    if ((u32)height > surface_capabilities.maxImageExtent.height) {
      context->swapchain_extent.height = surface_capabilities.maxImageExtent.height;
    } else if ((u32)height < surface_capabilities.minImageExtent.height) {
      context->swapchain_extent.height = surface_capabilities.minImageExtent.height;
    } else {
      context->swapchain_extent.height = (u32)height;
    }
  }
  return TRUE;
}

b8 vulkan_create_logical_device(VulkanContext* context) {
  u32 queue_family_properties_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device,
                                           &queue_family_properties_count, NULL_PTR);
  VkQueueFamilyProperties queue_family_properties[queue_family_properties_count];
  vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device,
                                           &queue_family_properties_count, queue_family_properties);

  VkBool32 queue_family_supported = VK_FALSE;
  for (u32 i = 0; i < queue_family_properties_count; i++) {
    VkResult get_physical_device_surface_support_result = vkGetPhysicalDeviceSurfaceSupportKHR(
                                                                                               context->physical_device, i, context->surface, &queue_family_supported);

    if (get_physical_device_surface_support_result != VK_SUCCESS) {
      MERROR_CORE("Failed to get vulkan physical device surface support: %s",
                  string_VkResult(get_physical_device_surface_support_result));
      return FALSE;
    }

    if ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && queue_family_supported) {
      context->graphics_queue_family_index = i;
      break;
    }
  }
  if (queue_family_supported == VK_FALSE) {
    MERROR_CORE("Failed to find suitable queue family on vulkan physical device.");
    return FALSE;
  }

  b8 transfer_queue_found = FALSE;
  for (u32 i = 0; i < queue_family_properties_count; i++) {
    VkQueueFlags flags = queue_family_properties[i].queueFlags;

    // Prefer dedicated transfer queue
    if ((flags & VK_QUEUE_TRANSFER_BIT) && !(flags & VK_QUEUE_GRAPHICS_BIT)) {
      context->transfer_queue_family_index = i;
      transfer_queue_found = TRUE;
      break;
    }
  }
  if (transfer_queue_found == FALSE) {
    context->transfer_queue_family_index = context->graphics_queue_family_index;
  }

  /* queue_family_supported = VK_FALSE; */
  /* for (u32 i = 0; i < queue_family_properties_count; i++) { */
  /*   VkResult get_physical_device_surface_support_result = vkGetPhysicalDeviceSurfaceSupportKHR(
   */
  /*       context->physical_device, i, context->surface, &queue_family_supported); */

  /*   if (get_physical_device_surface_support_result != VK_SUCCESS) { */
  /*     MERROR_CORE("Failed to get vulkan physical device surface support: %s", */
  /*                 string_VkResult(get_physical_device_surface_support_result)); */
  /*     return FALSE; */
  /*   } */

  /*   if ((queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
   * queue_family_supported) { */
  /*     context->transfer_queue_family_index = i; */
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
  device_graphics_queue_create_info.queueFamilyIndex = context->graphics_queue_family_index;
  device_graphics_queue_create_info.queueCount = 1;  // @MAGIC_NUMBER
  device_graphics_queue_create_info.pQueuePriorities = &temp_priority;


  VkDeviceQueueCreateInfo device_queue_create_infos[2];
  VkDeviceQueueCreateInfo device_transfer_queue_create_info = {
    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  
  if (context->graphics_queue_family_index == context->transfer_queue_family_index) {
    
    context->queue_family_index_count = 1;
    device_queue_create_infos[0] = device_graphics_queue_create_info;

  } else {
    
    device_transfer_queue_create_info.queueFamilyIndex = context->transfer_queue_family_index;
    device_transfer_queue_create_info.queueCount = 1;  // @MAGIC_NUMBER
    device_transfer_queue_create_info.pQueuePriorities = &temp_priority;
    
    context->queue_family_index_count = 2;
    device_queue_create_infos[0] = device_graphics_queue_create_info;
    device_queue_create_infos[1] = device_transfer_queue_create_info;
  }

  // Enable vulkan11 features
  VkPhysicalDeviceVulkan11Features physical_device_vulkan_11_features = {
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
  physical_device_vulkan_11_features.shaderDrawParameters = VK_TRUE;

  // Enable vulkan13 features
  VkPhysicalDeviceVulkan13Features physical_device_vulkan_13_features = {
    .sType =  VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
    .pNext = &physical_device_vulkan_11_features,
    .dynamicRendering = VK_TRUE,
    .synchronization2 = VK_TRUE,
  };
  
  // Enable extra features
  VkPhysicalDeviceFeatures physical_device_features = {
    .samplerAnisotropy = VK_TRUE,
  };

  VkPhysicalDeviceFeatures2 physical_device_features_2 = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
    .pNext = &physical_device_vulkan_13_features,
    .features = physical_device_features,
  };
    

  // Set logical device create info
  const char* const device_extension_names[] = MVK_DEVICE_EXTENSION_NAMES;
  VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  device_create_info.pNext = &physical_device_features_2;
  device_create_info.queueCreateInfoCount = context->queue_family_index_count;
  device_create_info.pQueueCreateInfos = device_queue_create_infos;
  device_create_info.pEnabledFeatures = NULL_PTR;
  device_create_info.enabledExtensionCount = MVK_DEVICE_EXTENSION_COUNT;
  device_create_info.ppEnabledExtensionNames = device_extension_names;

  // Create logical device
  VkResult create_device_result =
    vkCreateDevice(context->physical_device, &device_create_info, context->allocator,
                   &context->logical_device);

  if (create_device_result == VK_SUCCESS) {
    MINFO_CORE("Vulkan logical device created");
  } else {
    MERROR_CORE("Failed to create vulkan logical device: %s",
                string_VkResult(create_device_result));
    return FALSE;
  }
  vkGetDeviceQueue(context->logical_device, context->graphics_queue_family_index, 0,
                   &context->graphics_queue);

  vkGetDeviceQueue(context->logical_device, context->transfer_queue_family_index, 0,
                   &context->transfer_queue);
  return TRUE;
}

