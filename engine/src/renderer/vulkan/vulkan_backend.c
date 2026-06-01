#include "vulkan_backend.h"

#include <vulkan/vk_enum_string_helper.h>


#include "core/log.h"
#include "define.h"
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
  }

  return TRUE;
}

void vulkan_backend_shutdown(RendererBackend* renderer_backend) {
  vkDestroyInstance(vulkan_context.instance, vulkan_context.allocator);
}

b8 vulkan_backend_start_frame(RendererBackend* renderer_backend, f64 delta_time) { return TRUE; }

b8 vulkan_backend_end_frame(RendererBackend* renderer_backend, f64 delta_time) { return TRUE; }

void vulkan_backend_resized(RendererBackend* renderer_backend, u16 width, u16 height) {}
