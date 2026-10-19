/**
 * @file string.h
 *
 * @brief My string implemantation of a string type.
 *
 * String is a normal char* with metadata allocated before the
 * pointer in memory meaning they can be treated a normal cstrings.
 *
 * IMPORTANT: Any modification of a String should be done using functions
 * defined in this file to ensure metadata stays consistant.
 */

#pragma once

#include "define.h"

/**
 * @brief Generic string type.
 */
typedef char* String;

/**
 * @brief Read-only string type.
 */
typedef const char* StringView;

/**
 * @brief Allocates and creates a String from a cstring
 *
 * @param cstr, A null terminated cstring to create the String from.
 * @return The newly created String
 */
MGAPI String create_string(const char* cstr);

/**
 * @brief Frees up the memory used by a string.
 *
 * @param str, The String to destroy.
 */
MGAPI void destroy_string(String str);

/**
 * @brief Retrieves the length metadata from a String.
 *
 * @param str, The String.
 * @return The length of the String.
 */
MGAPI u64 string_get_length(StringView str);

/**
 * @brief Compares the each character of 2 Strings and returns
 * whether they are equal or which one is greater.
 *
 * Currently the same as strcmp() from the C Standared Lib.
 *
 * @param str_1, String 1.
 * @param str_2, String 2.
 * @return 0 if the strings are equal, a positive int if the first string
 * is greater, a negative int if the second string is greater.
 */
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
