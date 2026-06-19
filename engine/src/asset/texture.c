#include "texture.h"

#include "vendor/stb_image.h"
#include "core/log.h"

b8 load_texture(const char* path, u8** texture_data, u32* width, u32* height, u32* channels) {
  
  *texture_data = stbi_load(path, (i32*)width, (i32*)height, (i32*)channels, STBI_rgb_alpha);
  
  if (texture_data == NULL_PTR) {
    MERROR_CORE("Failed to load texture image: %s", path);
    return FALSE;
  }
  return TRUE;
}
