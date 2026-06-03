/**
 * @file define.h
 * @brief Contains definitions for basic types, platform information and
 * function exports
 */

#pragma once

// Basic types

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef char b8;
typedef int b32;

#define TRUE 1
#define FALSE 0

#define ZERO 0
#define NULL_PTR (void*)0

#define HANDLED 1

#define ENDL '\n'
#define NULL_TERM '\0'

// Define static assertions

#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// Check type sizes are consistent

STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 = 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 = 2 bytes.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 = 4 bytes.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 = 8 bytes.");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 = 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 = 2 bytes.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 = 4 bytes.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 = 8 bytes.");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 = 4 bytes.");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 = 8 bytes.");

STATIC_ASSERT(sizeof(b8) == 1, "Expected b8 = 1 byte.");
STATIC_ASSERT(sizeof(b32) == 4, "Expected b32 = 4 bytes.");

// Platform detection

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define MPLATFORM_WINDOWS 1
#ifndef _WIN64
#error "Requires a 64 bit windows version."
#endif

#elif defined(__linux__) || defined(__gnu_linux__)
#define MPLATFORM_LINUX 1
#if defined(__ANDROID__)
#define MPLATFORM_ANDROID 1
#endif

#elif defined(__unix__)
#define MPLATFORM_UNIX 1

#elif defined(_POSIX_VERSION)
#define MPLATFORM_POSIX 1

#elif __APPLE__
#define MPLATFORM_APPLE 1
#include <TargetConditionals.h>

#if TARGET_IPHONE_SIMULATOR
#define MPLATFORM_IOS 1
#define MPLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define MPLATFORM_IOS 1

#elif TARGET_OS_MAC
#define MPLATFORM_MAC 1
#else
#error "Unknown apple platform."
#endif
#else
#error "Unknown platform."
#endif

#ifdef MEXPORT

// Exports
#ifdef _MSC_VER
#define MGAPI __declspec(dllexport)
#else
#define MGAPI __attribute((visibility("default")))
#endif
#else

// Imports
#ifdef _MSC_VER
#define MGAPI __declspec(dllimport)
#else
#define MGAPI
#endif
#endif
