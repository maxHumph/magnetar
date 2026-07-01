/**
 * Copyright 2026 Your Name
 * SPDX-License-Identifier: Apache-2.0
 * 
 * @file asserts.h
 * @brief Contains macros used for assertions.
 */

#pragma once

#include "define.h"

#define MASSERTIONS_ENABLED

#ifdef MASSERTIONS_ENABLED

#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

/**
 * @brief Used by ASSERT macros to give info about a failed assertion. (Implemented in log.c)
 * @param expression The expression validated by the assertion.
 * @param message Optional extra information.
 * @param file The file the assertion is used in.
 * @param line The line number of the assertion.
 */
MGAPI void report_assertion_failure(const char* expression, const char* message, const char* file,
                                    i32 line);

#define MASSERT(expr)                                          \
  {                                                            \
    if (expr) {                                                \
    } else {                                                   \
      report_assertion_failure(#expr, "", __FILE__, __LINE__); \
      debugBreak();                                            \
    }                                                          \
  }

#define MASSERT_MSG(expr, message)                                  \
  {                                                                 \
    if (expr) {                                                     \
    } else {                                                        \
      report_assertion_failure(#expr, message, __FILE__, __LINE__); \
      debugBreak();                                                 \
    }                                                               \
  }

#ifdef _DEBUG
#define MASSERT_DEBUG(expr)                                    \
  {                                                            \
    if (expr) {                                                \
    } else {                                                   \
      report_assertion_failure(#expr, "", __FILE__, __LINE__); \
    }                                                          \
  }
#else
#define MASSERT_DEBUG(expr)
#endif

#ifdef _DEBUG
#define MASSERT_MSG_DEBUG(expr, message)                            \
  {                                                                 \
    if (expr) {                                                     \
    } else {                                                        \
      report_assertion_failure(#expr, message, __FILE__, __LINE__); \
    }                                                               \
  }
#else
#define MASSERT_MSG_DEBUG(expr, message)
#endif

#else
#define MASSERT(expr)
#define MASSERT_MSG(expr, message)
#define MASSERT_DEBUG(expr)
#define MASSERT_MSG_DEBUG(expr, message)
#endif
