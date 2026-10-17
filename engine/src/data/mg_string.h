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

MGAPI String create_string(const char* str_in);

MGAPI void destroy_string(String str);

MGAPI u64 string_length(String str);

// To implement

MGAPI u8 string_parse_u8(String string);

MGAPI u16 string_parse_u16(String string);

MGAPI u32 string_parse_u32(String string);

MGAPI u64 string_parse_u64(String string);

MGAPI i8 string_parse_i8(String string);

MGAPI i16 string_parse_i16(String string);

MGAPI i32 string_parse_i32(String string);

MGAPI i64 string_parse_i64(String string);

MGAPI f32 string_parse_f32(String string);

MGAPI f64 string_parse_f64(String string);

MGAPI b8 string_parse_b8(String string);
