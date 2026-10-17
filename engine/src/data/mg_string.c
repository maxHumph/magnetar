#include "mg_string.h"

#include "core/mmemory.h"

#include <string.h>

String create_string(const char* str_in) {
  u64 length = sizeof(char) * strlen(str_in);
  u64 bytes = sizeof(u64) + length;
  char* head = mallocate(bytes, MEMORY_TAG_STRING);
  ((u64*)head)[0] = length;

  String str = head + sizeof(u64);
  mcopy_memory(str, str_in, length * sizeof(char));

  return str;
}

void destroy_string(String str) {
  u64 bytes = sizeof(u64) + (string_length(str) * sizeof(char));
  char* head = str - sizeof(u64);
  mfree(head, bytes, MEMORY_TAG_STRING);
}

u64 string_length(String str) {
  return *((u64*)str - 1);
}
