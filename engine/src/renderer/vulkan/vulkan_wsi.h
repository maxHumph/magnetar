/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file vulkan_wsi.h
 */

#pragma once

#include "platform/platform.h"
#include "vulkan_defines.h"

b8 vulkan_create_platform_surface(VulkanContext* vulkan_context, PlatformState* platform_state);
