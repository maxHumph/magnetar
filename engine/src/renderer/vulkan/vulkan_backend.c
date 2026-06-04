#include "vulkan_backend.h"

#include <vulkan/vk_enum_string_helper.h>

#include "core/log.h"
#include "core/mmemory.h"
#include "define.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_defines.h"
#include "vulkan_wsi.h"

static VulkanContext vulkan_context;

b8 vulkan_backend_init(RendererBackend* renderer_backend, const char* application_name,
                       PlatformState* platform_state) {
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
  VkPhysicalDevice suitable_device = physical_devices[0];
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
  device_queue_create_info.queueFamilyIndex = 0;
  device_queue_create_info.queueCount = 1;
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
  VkResult create_device_result = vkCreateDevice(suitable_device, &device_create_info,
                                                 vulkan_context.allocator, &vulkan_context.device);

  if (create_device_result == VK_SUCCESS) {
    MINFO_CORE("Vulkan logical device created");
  } else {
    MERROR_CORE("Failed to create vulkan logical device: %s",
                string_VkResult(create_device_result));
    return FALSE;
  }

  // Create surface
  if (!vulkan_create_platform_surface(&vulkan_context, platform_state)) {
    MERROR("Failed to create vulkan platform surface");
    return FALSE;
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

  vkDeviceWaitIdle(vulkan_context.device);
  vkDestroyDevice(vulkan_context.device, vulkan_context.allocator);
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
