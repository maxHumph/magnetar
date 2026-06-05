#include "vulkan_backend.h"

#include <vulkan/vk_enum_string_helper.h>

#include "core/log.h"
#include "core/mmemory.h"
#include "define.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_defines.h"
#include "vulkan_wsi.h"

static VulkanContext vulkan_context;

// @TODO: Split this up into functions.
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
  // Set queue create info
  f32 temp_priority = 1.0f;
  VkDeviceQueueCreateInfo device_queue_create_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  device_queue_create_info.queueFamilyIndex = 0;  // @MAGIC_NUMBER
  device_queue_create_info.queueCount = 1;        // @MAGIC_NUMBER
  device_queue_create_info.pQueuePriorities = &temp_priority;

  // Set logical device create info
  const char* const device_extension_names[] = MVK_DEVICE_EXTENSION_NAMES;
  VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
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

  // Create surface
  if (!vulkan_create_platform_surface(&vulkan_context, platform_state)) {
    MERROR_CORE("Failed to create vulkan platform surface");
    return FALSE;
  }

  // Get surface extent
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkResult get_physical_device_surface_capabilities_result =
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_context.physical_device,
                                                vulkan_context.surface, &surface_capabilities);

  VkExtent2D swapchain_image_extent;

  if (get_physical_device_surface_capabilities_result != VK_SUCCESS) {
    MERROR_CORE("Failed to get physical device surface capabilities: %s",
                string_VkResult(get_physical_device_surface_capabilities_result));
    return FALSE;
  }

  // Sets the image extent to an acceptable value
  if (surface_capabilities.currentExtent.width != UINT32_MAX) {  // Surface provides specific size
    swapchain_image_extent = surface_capabilities.currentExtent;
  } else {  // Application can provide size
    // Set width within acceptable Image Extent range
    if ((u32)start_width > surface_capabilities.maxImageExtent.width) {
      swapchain_image_extent.width = surface_capabilities.maxImageExtent.width;
    } else if ((u32)start_width < surface_capabilities.minImageExtent.width) {
      swapchain_image_extent.width = surface_capabilities.minImageExtent.width;
    } else {
      swapchain_image_extent.width = (u32)start_width;
    }

    // Set width within acceptable Image Extent range
    if ((u32)start_height > surface_capabilities.maxImageExtent.height) {
      swapchain_image_extent.height = surface_capabilities.maxImageExtent.height;
    } else if ((u32)start_height < surface_capabilities.minImageExtent.height) {
      swapchain_image_extent.height = surface_capabilities.minImageExtent.height;
    } else {
      swapchain_image_extent.height = (u32)start_height;
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

  // Create swapchain
  VkImageUsageFlags image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  swapchain_create_info.pNext = NULL_PTR;
  swapchain_create_info.surface = vulkan_context.surface;
  swapchain_create_info.minImageCount = 4;  // @TODO: Add this as a setting in user code. (Maybe)
  swapchain_create_info.imageFormat = surface_formats[selected_format].format;
  swapchain_create_info.imageColorSpace = surface_formats[selected_format].colorSpace;
  swapchain_create_info.imageExtent = swapchain_image_extent;
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

  mfree(physical_devices, physical_device_count * sizeof(VkPhysicalDevice), MEMORY_TAG_RENDERER);

  return TRUE;
}

void vulkan_backend_shutdown(RendererBackend* renderer_backend) {
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

b8 vulkan_backend_start_frame(RendererBackend* renderer_backend, f64 delta_time) { return TRUE; }

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time) { return TRUE; }

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
