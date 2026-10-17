/**
 * @file string.h
 *
 * @brief Strings that arent C strings
 *
 * String is just a char* NOT A STRUCT, this way they can be treated
 * like char* but with a length field allocated before the character
 * array instead of silly null terminators.
 */

#pragma once

#include "define.h"

typedef char* String;

typedef const char* StringView;

MGAPI String create_string(const char* str_in);

MGAPI void destroy_string(String str);

MGAPI u64 string_length(StringView str);

MGAPI i32 string_compare(StringView str_1, StringView str_2);

// To implement

MGAPI u8 string_parse_u8(StringView string);

MGAPI u16 string_parse_u16(StringView string);

MGAPI u32 string_parse_u32(StringView string);

MGAPI u64 string_parse_u64(StringView string);

MGAPI i8 string_parse_i8(StringView string);

MGAPI i16 string_parse_i16(StringView  string);

MGAPI i32 string_parse_i32(StringView string);

MGAPI i64 string_parse_i64(StringView string);

MGAPI f32 string_parse_f32(StringView string);

MGAPI f64 string_parse_f64(StringView string);

MGAPI b8 string_parse_b8(StringView string);
