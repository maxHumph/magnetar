/**
 * @file mg_string.h
 *
 * @brief Strings that arent C strings
 */

#pragma once

#include "define.h"

typedef char* String;

eGAPI String create_string(char* str_in);

MGAPI void destroy_string(String str);
