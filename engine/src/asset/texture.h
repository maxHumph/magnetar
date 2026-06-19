/**
 * @file texture.h
 */

#pragma once

#include "define.h"

b8 load_texture(const char* path, u8** texture_data, u32* width, u32* height, u32* channels);
