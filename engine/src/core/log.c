#include "log.h"

#include "core/asserts.h"

// temp
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

b8 initialize_logging() { return TRUE; }

void shutdown_logging() {}

void log_output(LogLevel level, const char* message, ...) {
  // 32K character limit
  const char* level_strings[12] = {
      "[FATAL (core)]: ",   "[FATAL]: ",   "[ERROR (core)]: ", "[ERROR]: ",
      "[WARNING (core)]: ", "[WARNING]: ", "[INFO (core)]: ",  "[INFO]: ",
      "[DEBUG (core)]: ",   "[DEBUG]: ",   "[TRACE (core)]: ", "[TRACE]: "};
  b8 is_error = level < 4;

  char out_buffer[32000];
  memset(out_buffer, 0, sizeof(out_buffer));

  // __builtin_va_list arg_ptr;
  va_list arg_ptr;
  va_start(arg_ptr, message);
  // generates a formated string and moves it to out_message
  vsnprintf(out_buffer, 32000, message, arg_ptr);
  va_end(arg_ptr);

  char out_message[32000];
  sprintf(out_message, "%s%s\n", level_strings[level], out_buffer);

  printf("%s", out_message);
}

void report_assertion_failure(const char* expression, const char* message, const char* file,
                              i32 line) {
  log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: %s, %s:%d\n", expression, message,
             file, line);
}
