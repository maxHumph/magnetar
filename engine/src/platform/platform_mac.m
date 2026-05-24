/**
 * @file platform_mac.m
 * @brief MacOS API specific stuff written in disgusting Objective C.
 *
 * Warning: This file is vibe coded because I don't want to learn objective c so
 * the code is probably a bit dodgy. (This is the only time I have ever or will
 * ever vibe code.)
 */

#include "platform.h"

#if defined(MPLATFORM_APPLE)

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <mach/mach_time.h>

#import <Cocoa/Cocoa.h>

typedef struct InternalState {
  NSWindow* ns_window;
  // CAMetalLayer* layer;
} InternalState;

b8 platform_startup(PlatformState* platform_state, const char* application_name, i32 x_pos,
                    i32 y_pos, i32 width, i32 height) {
  platform_state->internal_state = malloc(sizeof(InternalState));
  InternalState *state = (InternalState *)platform_state->internal_state;
  NSRect frame = NSMakeRect(x_pos, y_pos, width, height);

  NSWindow* window =
    [[NSWindow alloc] initWithContentRect:frame
				styleMask:(NSWindowStyleMaskTitled |
					   NSWindowStyleMaskClosable |
					   NSWindowStyleMaskResizable)
				  backing:NSBackingStoreBuffered
				    defer:NO];

  NSString *nsTitle =
    [NSString stringWithUTF8String:application_name ? application_name : "Window"];

  [window setTitle:nsTitle];

  state->ns_window = window;

  [state->ns_window makeKeyAndOrderFront:nil];

  [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];

  return TRUE;
}

void platform_shutdown(PlatformState* platform_state) {
  InternalState *state = (InternalState *)platform_state->internal_state;
  [state->ns_window close];
  state->ns_window = nil;

  free(state);
}

//implemented further down.
static void process_event(NSEvent* ns_event);

b8 platform_pump_messages(PlatformState* platform_state) {
  NSEvent* event;

  while ((event =
	  [NSApp nextEventMatchingMask:NSEventMaskAny
			     untilDate:nil
				inMode:NSDefaultRunLoopMode
			       dequeue:YES]))
    {
      process_event(event);

      {
	[NSApp sendEvent:event];
      }
    }
  return TRUE;
}


void* platform_alloc(u64 size, b8 is_aligned) {
  return malloc(size);
}

void platform_free(void* block, b8 is_aligned) {
  free(block);
}

void* platform_mem_zero(void* block, u64 size) {
  return memset(block, 0, size);
}

void* platform_mem_cpy(void* out, const void* src, u64 size) {
  return memcpy(out, src, size);
}

void* platoform_mem_set(void* block, i32 val, u64 size) {
  return memset(block, val, size);
}

void platform_print(const char* message, u8 colour) {
  printf("%s", message);
}

void platform_print_error(const char* message, u8 colour) {
  printf("%s", message);
}

f64 platform_get_time_abs() {
  static mach_timebase_info_data_t timebase;
  static i32 initialized = 0;

  if (!initialized) {
    mach_timebase_info(&timebase);
    initialized = 1;
  }

  u64 time = mach_absolute_time();

  f64 nanos =
    (f64)time *
    (f64)timebase.numer /
    (f64)timebase.denom;

  return nanos / 1e9;
}

void platform_sleep(u64 ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

static void process_event(NSEvent* ns_event) {
  switch ([ns_event type]){
  case NSEventTypeKeyDown:
    break;

  case NSEventTypeKeyUp:
    break;

  case NSEventTypeMouseMoved:
    break;

  default:
    break;
  }
}

#endif
