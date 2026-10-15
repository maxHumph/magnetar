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

MGAPI String create_string(char* str_in);

MGAPI void destroy_string(String str);

MGAPI u64 string_length(String str);
