/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file mmemory.h
 * @brief Handles memory allocation within the game engine.
 */

#pragma once

#include "define.h"

typedef enum MemoryTag {

  MEMORY_TAG_UNKNOWN,

  MEMORY_TAG_ARRAY,
  MEMORY_TAG_DARRAY,
  MEMORY_TAG_DICT,
  MEMORY_TAG_RING_QUEUE,
  MEMORY_TAG_BST,
  MEMORY_TAG_STRING,
  MEMORY_TAG_APPLICATION,
  MEMORY_TAG_JOB,
  MEMORY_TAG_MODEL,
  MEMORY_TAG_TEXTURE,
  MEMORY_TAG_MATERIAL_INSTANCE,
  MEMORY_TAG_RENDERER,
  MEMORY_TAG_GAME,
  MEMORY_TAG_TRANSFORM,
  MEMORY_TAG_ENTITY,
  MEMORY_TAG_COMPONENT,
  MEMORY_TAG_SCENE,

  MEMORY_TAG_MAX_TAGS,

} MemoryTag;

/**
 * @brief Initializes the memory subsystem.
 * @return Whether the initialization was succesful.
 */
b8 initialize_memory();

/**
 * @brief Shuts down the memory subsystem.
 */
void shutdown_memory();

/**
 * @brief Allocates a block of memory and sets all values to 0.
 * @param size The size in bytes of the memory allocation.
 * @param memory_tag The MemoryTag for the allocation to be associated with.
 * @return A pointer to the start of the allocated memory block.
 */
MGAPI void* mallocate(u64 size, MemoryTag memory_tag);

/**
 * @brief Frees a block of memory.
 * @param block A pointer to the start of the memory block.
 * @param size The size in bytes of the memory block.
 * @param memory_tag The MemoryTag that the block was associated with.
 */
MGAPI void mfree(void* block, u64 size, MemoryTag memory_tag);

/**
 * @brief Sets all values in a block of memory to 0.
 * @param block A pointer to the start of the memory block.
 * @param size The size in bytes of the memory block.
 * @return A pointer to the start of the memory block.
 */
MGAPI void* mzero_memory(void* block, u64 size);

/**
 * @brief Copies the values in a block a memory to a new location in memory.
 * @param out A pointer to the destination of the copied memory.
 * @param src A pointer to the start of the memory block to be copied.
 * @param size The size in bytes of the memory block.
 * @return A pointer to the start of the destination.
 */
MGAPI void* mcopy_memory(void* out, const void* src, u64 size);

/**
 * @brief Sets all values in a block of memory to a specified value.
 * @param block A pointer to the start of the memory block.
 * @param val The value to set the memory to.
 * @param size The size in bytes of the memory block.
 * @return A pointer to the start of the memory block.
 */
MGAPI void* mset_memory(void* block, i32 val, u64 size);

/**
 * @brief Gives statistics about the system memory usage.
 * @return The statistics as a string.
 */
MGAPI char* get_memory_usage_string();
