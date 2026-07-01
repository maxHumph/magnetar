/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file vulkan_helper.h
 * @brief Contains various helper functions for the vulkan backend.
 */

#pragma once

#include "define.h"

/**
 * @brief Reads the contents of a file as binary.
 * @param path, The relative or absolute file path.
 * @param size, A pointer to a variable where the file length will be stored.
 * @param out_bin, A pointer to where the read binary will be stored, (should not be allocated yet).
 * @return TRUE is the read was successful, otherwise FALSE.
 */
b8 vulkan_read_shader_binary(const char* path, u64* size, u8** out_bin);
