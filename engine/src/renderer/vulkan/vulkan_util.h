/**
 * @file vulkan_util.h
 */

#include <vulkan/vulkan.h>

// =============================
// VERTEX ATTRIBUTE DESCRIPTIONS
// =============================

MGINLINE VkVertexInputAttributeDescription vec3_attribute(u32 location, u32 binding, u32 offset) {
  return (VkVertexInputAttributeDescription) {
    .location = location,
    .binding = binding,
    .format = VK_FORMAT_R32G32B32_SFLOAT,
    .offset = offset,
  };
}

MGINLINE VkVertexInputAttributeDescription vec2_attribute(u32 location, u32 binding, u32 offset) {
  return (VkVertexInputAttributeDescription) {
    .location = location,
    .binding = binding,
    .format = VK_FORMAT_R32G32_SFLOAT,
    .offset = offset,
  };
}
