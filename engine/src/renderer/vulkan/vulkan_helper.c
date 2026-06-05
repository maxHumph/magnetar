#include "vulkan_helper.h"

#include <stdio.h>

#include "core/log.h"
#include "core/mmemory.h"

// @TODO: Check the whole read was successful
b8 vulkan_read_shader_binary(const char* path, u64* size, u8** out_bin) {
  FILE* file = fopen(path, "rb");
  i64 file_size;

  if (file == NULL_PTR) {
    MERROR_CORE("Couldn't open shader file: %s", path);
    return FALSE;
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  *out_bin = mallocate(file_size, MEMORY_TAG_RENDERER);
  if (out_bin != NULL_PTR) {
    fread(*out_bin, sizeof(u8), file_size, file);
  } else {
    MERROR_CORE("Failed to allocaate out_str while reading shader file");
    return FALSE;
  }
  fclose(file);
  *size = (u64)file_size;

  return TRUE;
}
