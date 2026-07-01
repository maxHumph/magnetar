/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file vulkan_device.h
 */

#pragma once

#include "define.h"
#include "vulkan_defines.h"

#include "platform/platform.h"

/**
 * @brief Retrieves physical device information and chooses a suitable one and assigns it to the
 * vulkan_context.
 *
 * @return TRUE if a suitable device was found, otherwise FALSE.
 */
b8 vulkan_select_physical_device(VulkanContext* context);

/**
 * @brief Connects vulkan to the platform surface setup in platform.h.
 *
 * @param platform_state, A pointer to the platform state.
 * @return TRUE if surface could be retrieved, otherwise FALSE.
 */
b8 vulkan_get_surface(VulkanContext* context, PlatformState* platform_state);

/**
 * @brief Sets the surface extent to a specified value OR the value provided by the surface handle.
 * @note width and heigth must be specified but may not be used if the vulkan surface requires
 * specific values.
 *
 * @param width, The desired width of the surface.
 * @param height, The desired height of the surface.
 * @return TRUE if the surface extent was set successfully, otherwise FALSE.
 */
b8 vulkan_set_surface_extent(VulkanContext* context, i16 width, i16 height);

/**
 * @brief Creates the vulkan logical device from the physical device in vulkan_context.
 *
 * @return TRUE if the device was created successfully, otherwise FALSE.
 */
b8 vulkan_create_logical_device(VulkanContext* context);
