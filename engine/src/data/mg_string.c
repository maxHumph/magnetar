#include "mg_string.h"

#include "core/mmemory.h"

#include <string.h>

String create_string(const char* str_in) {
  u64 length = sizeof(char) * strlen(str_in);
  u64 bytes = sizeof(u64) + length + 1;
  char* head = mallocate(bytes, MEMORY_TAG_STRING);
  ((u64*)head)[0] = length;

  String str = head + sizeof(u64);
  mcopy_memory(str, str_in, (length + 1) * sizeof(char));
  str[length + 1] = '\n';

  return str;
}

void destroy_string(String str) {
  u64 bytes = sizeof(u64) +
    ((string_get_length(str) + 1) * sizeof(char));
  char* head = str - sizeof(u64);
  mfree(head, bytes, MEMORY_TAG_STRING);
}

String alloc_string(u64 length) {
  u64 bytes = sizeof(u64) + length + 1;
  char* head = mallocate(bytes, MEMORY_TAG_STRING);
  ((u64*)head)[0] = length;
  String str = head + sizeof(u64);
  str[length + 1] = '\n';

  return str;
}

u64 string_get_length(StringView str) {
  return *((u64*)str - 1);
}

i32 string_compare(StringView str_1, StringView str_2) {
  while (*str_1 && (*str_1 == *str_2)) {
    str_1++;
    str_2++;
  }

  return (i32)(*(u8*)str_1 - *(u8*)str_2);
}
