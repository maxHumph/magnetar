#include "vulkan_backend.h"

#include <vulkan/vk_enum_string_helper.h>

#include "core/log.h"
#include "core/mmemory.h"
#include "define.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_defines.h"

static VulkanContext vulkan_context;

b8 vulkan_backend_init(RendererBackend* renderer_backend, const char* application_name,
                       PlatformState* platform_state) {
  vulkan_context.allocator = NULL_PTR;

  // Set vulkan app info
  VkApplicationInfo application_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  application_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  application_info.apiVersion = VK_API_VERSION_1_4;
  application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  application_info.pApplicationName = application_name;
  application_info.pEngineName = "Magnetar Engine";

  // Set vulkan instance create info
  VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  const char* const extension_names = MVK_EXTENSION_NAMES;
  create_info.pApplicationInfo = &application_info;
  create_info.enabledExtensionCount = MVK_EXTENSION_COUNT;
  create_info.enabledLayerCount = 0;
  create_info.ppEnabledExtensionNames = &extension_names;
  create_info.ppEnabledLayerNames = NULL_PTR;
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

  // List physical devices
  // TODO: Select most suitable device
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

  // Set logical device create info
  VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  device_create_info.queueCreateInfoCount = 0;
  device_create_info.pQueueCreateInfos = NULL_PTR;
  device_create_info.pEnabledFeatures = NULL_PTR;
  // Might need to mess with these for macos
  device_create_info.enabledExtensionCount = 0;
  device_create_info.ppEnabledExtensionNames = NULL_PTR;

  // Create logical device
  // TODO: select most suistable physical device
  VkResult create_device_result = vkCreateDevice(suitable_device, &device_create_info,
                                                 vulkan_context.allocator, &vulkan_context.device);
  if (create_device_result == VK_SUCCESS) {
    MINFO_CORE("Vulkan logical device created");
  } else {
    MERROR_CORE("Failed to create vulkan logical device: %s",
                string_VkResult(create_device_result));
    return FALSE;
  }

  mfree(physical_devices, physical_device_count * sizeof(VkPhysicalDevice), MEMORY_TAG_RENDERER);

  return TRUE;
}

void vulkan_backend_shutdown(RendererBackend* renderer_backend) {
  vkDeviceWaitIdle(vulkan_context.device);
  vkDestroyDevice(vulkan_context.device, vulkan_context.allocator);
  vkDestroyInstance(vulkan_context.instance, vulkan_context.allocator);
}

b8 vulkan_backend_start_frame(RendererBackend* renderer_backend, f64 delta_time) { return TRUE; }

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time) { return TRUE; }

void vulkan_backend_resized(RendererBackend* renderer_backend, u16 width, u16 height) {}
