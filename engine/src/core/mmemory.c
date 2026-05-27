#include "mmemory.h"

#include <stdio.h>
#include <string.h>

#include "core/log.h"
#include "platform/platform.h"

struct MemoryStats {
  u64 total_allocated;
  u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {

    "unknown          ", "array            ", "darray           ", "dict             ",
    "ring_queue       ", "bst              ", "string           ", "application      ",
    "job              ", "texture          ", "material_instance", "renderer         ",
    "game             ", "transform        ", "entity           ", "entity_node      ",
    "scene            ",
};

static struct MemoryStats s_stats;

b8 initialize_memory() {
  platform_mem_zero(&s_stats, sizeof(s_stats));
  return TRUE;
}

void shutdown_memory() {}

MGAPI void* mallocate(u64 size, MemoryTag memory_tag) {
  if (memory_tag == MEMORY_TAG_UNKNOWN) {
    MWARN_CORE("mallocate call with MEMORY_TAG_UNKNOWN");
  }
  s_stats.total_allocated += size;
  s_stats.tagged_allocations[memory_tag] += size;

  void* block = platform_alloc(size, FALSE);
  platform_mem_zero(block, size);
  return block;
}

MGAPI void mfree(void* block, u64 size, MemoryTag memory_tag) {
  if (memory_tag == MEMORY_TAG_UNKNOWN) {
    MWARN_CORE("mallocate call with MEMORY_TAG_UNKNOWN");
  }

  s_stats.total_allocated -= size;
  s_stats.tagged_allocations[memory_tag] -= size;

  platform_free(block, FALSE);
}

MGAPI void* mzero_memory(void* block, u64 size) { return platform_mem_zero(block, size); }

MGAPI void* mcopy_memory(void* out, const void* src, u64 size) {
  return platform_mem_cpy(out, src, size);
}

MGAPI void* mset_memory(void* block, i32 val, u64 size) {
  return platform_mem_set(block, val, size);
}

MGAPI char* get_memory_usage_string() {
  const u64 gib = 1024 * 1024 * 1024;
  const u64 mib = 1024 * 1024;
  const u64 kib = 1024;

  u64 offset = 0;
  char buffer[8000] = "System memory usage (tagged):\n";
  offset = strlen(buffer);
  for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; i++) {
    char unit[4] = "XiB";
    f32 amount = 1.0f;
    if (s_stats.tagged_allocations[i] >= gib) {
      unit[0] = 'G';
      amount = s_stats.tagged_allocations[i] / (f32)gib;
    } else if (s_stats.tagged_allocations[i] >= mib) {
      unit[0] = 'M';
      amount = s_stats.tagged_allocations[i] / (f32)mib;
    } else if (s_stats.tagged_allocations[i] >= kib) {
      unit[0] = 'K';
      amount = s_stats.tagged_allocations[i] / (f32)kib;
    } else {
      unit[0] = 'B';
      unit[1] = '\0';
      amount = (f32)s_stats.tagged_allocations[i];
    }
    i32 length =
        snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);

    offset += length;
  }
  char* out_str = strdup(buffer);
  return out_str;
}
