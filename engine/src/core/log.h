/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file log.h
 * @brief Contains stuff used form console and file logging.
 */

#pragma once

#include "define.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if MRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

/**
 * @brief Used to set the importance of log messages.
 */
typedef enum LogLevel {
  LOG_LEVEL_FATAL_CORE = 0,
  LOG_LEVEL_FATAL = 1,
  LOG_LEVEL_ERROR_CORE = 2,
  LOG_LEVEL_ERROR = 3,
  LOG_LEVEL_WARN_CORE = 4,
  LOG_LEVEL_WARN = 5,
  LOG_LEVEL_INFO_CORE = 6,
  LOG_LEVEL_INFO = 7,
  LOG_LEVEL_DEBUG_CORE = 8,
  LOG_LEVEL_DEBUG = 9,
  LOG_LEVEL_TRACE_CORE = 10,
  LOG_LEVEL_TRACE = 11,
} LogLevel;

b8 initialize_logging();
void shutdown_logging();

/**
 * @brief Generic function for logging. (Use MERROR(message, ...), MWARN(message, ...) etc
 * instead).
 * @param level The importance level of the message.
 * @param message A message to log using format specifiers to pass in values.
 */
MGAPI void log_output(LogLevel level, const char* message, ...);

#ifndef MFATAL_CORE
#define MFATAL_CORE(message, ...) log_output(LOG_LEVEL_FATAL_CORE, message, ##__VA_ARGS__);
#endif

#ifndef MFATAL
#define MFATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);
#endif

#ifndef MERROR_CORE
#define MERROR_CORE(message, ...) log_output(LOG_LEVEL_ERROR_CORE, message, ##__VA_ARGS__);
#endif

#ifndef MERROR
#define MERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#ifndef MWARN_CORE
#if LOG_WARN_ENABLED == 1
#define MWARN_CORE(message, ...) log_output(LOG_LEVEL_WARN_CORE, message, ##__VA_ARGS__);
#else
#define MWARN_CORE(message, ...)
#endif
#endif

#ifndef MWARN
#if LOG_WARN_ENABLED == 1
#define MWARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define MWARN(message, ...)
#endif
#endif

#ifndef MINFO_CORE
#if LOG_INFO_ENABLED == 1
#define MINFO_CORE(message, ...) log_output(LOG_LEVEL_INFO_CORE, message, ##__VA_ARGS__);
#else
#define MINFO_CORE(message, ...)
#endif
#endif

#ifndef MINFO
#if LOG_INFO_ENABLED == 1
#define MINFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define MINFO(message, ...)
#endif
#endif

#ifndef MDEBUG_CORE
#if LOG_DEBUG_ENABLED == 1
#define MDEBUG_CORE(message, ...) log_output(LOG_LEVEL_DEBUG_CORE, message, ##__VA_ARGS__);
#else
#define MDEBUG_CORE(message, ...)
#endif
#endif

#ifndef MDEBUG
#if LOG_DEBUG_ENABLED == 1
#define MDEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define MDEBUG(message, ...)
#endif
#endif

#ifndef MTRACE_CORE
#if LOG_TRACE_ENABLED == 1
#define MTRACE_CORE(message, ...) log_output(LOG_LEVEL_TRACE_CORE, message, ##__VA_ARGS__);
#else
#define MTRACE_CORE(message, ...)
#endif
#endif

#ifndef MTRACE
#if LOG_TRACE_ENABLED == 1
#define MTRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define MTRACE(message, ...)
#endif
#endif
