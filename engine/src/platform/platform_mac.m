/**
 * @file platform_mac.m
 * @brief MacOS API specific stuff written in disgusting Objective C.
 *
 * Warning: I don't understand objective-c so this code is probably terrible.
 */

#include "platform.h"
#include "core/log.h"

#if defined(MPLATFORM_APPLE)

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <mach/mach_time.h>

#import <Cocoa/Cocoa.h>

/**
 * @brief Event type specifier for sruct MacEvent.
 */
typedef enum MacEventType {

  MAC_EVENT_TYPE_NONE = 0,

  MAC_EVENT_TYPE_QUIT,

  MAC_EVENT_TYPE_WINDOW_RESIZED,
  MAC_EVENT_TYPE_WINDOW_MINIMIZED,
  MAC_EVENT_TYPE_WINDOW_RESTORED,

  MAC_EVENT_TYPE_FOCUS_GAINED,
  MAC_EVENT_TYPE_FOCUS_LOST,

  MAC_EVENT_TYPE_KEY_DOWN,
  MAC_EVENT_TYPE_KEY_UP,

  MAC_EVENT_TYPE_MOUSE_MOVED,

  MAC_EVENT_TYPE_MOUSE_BUTTON_DOWN,
  MAC_EVENT_TYPE_MOUSE_BUTTON_UP,

  MAC_EVENT_TYPE_SCROLL_WHEEL,
  
} MacEventType;

typedef struct MacEventWindowResized {
  i32 width;
  i32 height;
} MacEventWindowResized;

typedef struct MacEventKeyDown {
  i32 keycode;
} MacEventKeyDown;

typedef struct MacEventKeyUp {
  i32 keycode;
} MacEventKeyUp;

typedef struct MacEventMouseMoved {
  f32 x_pos;
  f32 y_pos;
} MacEventMouseMoved;

typedef struct MacEventMouseButtonDown {
  i32 button;
  f32 x_pos;
  f32 y_pos;
} MacEventMouseButtonDown;

typedef struct MacEventMouseButtonUp{
  i32 button;
  f32 x_pos;
  f32 y_pos;
} MacEventMouseButtonUp;

/**
 * @brief Generic type for handling mac events since mac uses NSEvents for input and NSNotifications for window events.
 */
typedef struct MacEvent {

  MacEventType event_type;

  union {
    MacEventWindowResized window_resized;
    MacEventKeyDown key_down;
    MacEventKeyUp key_up;
    MacEventMouseMoved mouse_moved;
    MacEventMouseButtonDown mouse_button_down;
    MacEventMouseButtonUp mouse_button_up;
  };

} MacEvent;

/**
 * @brief Used by the engine to access os specific API code.
 */
typedef struct InternalState {
  NSWindow* ns_window;
  // CAMetalLayer* layer;
} InternalState;


@interface WindowDelegate : NSObject<NSWindowDelegate>
@end

b8 platform_startup(PlatformState* platform_state, const char* application_name, i32 x_pos,
                    i32 y_pos, i32 width, i32 height) {
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:
	   NSApplicationActivationPolicyRegular];



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

  [NSApp finishLaunching];

  [NSApp activateIgnoringOtherApps:YES];

  return TRUE;
}

void platform_shutdown(PlatformState* platform_state) {
  InternalState *state = (InternalState *)platform_state->internal_state;
  [state->ns_window close];
  state->ns_window = nil;

  free(state);
}

static MacEvent translate_ns_event(NSEvent* ns_event); //implemented further down.
static void process_event(MacEvent* e); // implemented fiurther down.

b8 platform_pump_messages(PlatformState* platform_state) {
  NSEvent* ns_event;

  while ((ns_event =
	  [NSApp nextEventMatchingMask:NSEventMaskAny
			     untilDate:nil
				inMode:NSDefaultRunLoopMode
			       dequeue:YES]))
    {
      MacEvent e = translate_ns_event(ns_event);

      if (e.event_type != MAC_EVENT_TYPE_NONE)
      {
	process_event(&e);
      }

      [NSApp sendEvent:ns_event];
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

void* platform_mem_set(void* block, i32 val, u64 size) {
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


@implementation WindowDelegate

- (BOOL)WindowShouldClose:(id)sender{
  MacEvent e;
  e.event_type = MAC_EVENT_TYPE_QUIT;

  // process_event(&e);

  return YES;
}

- (void)windowDidResize:(NSNotification *)notification
{
    NSWindow* window =
        (NSWindow*)[notification object];

    NSRect frame = [window contentRectForFrameRect:
        [window frame]];

    MacEvent e;

    e.event_type = MAC_EVENT_TYPE_WINDOW_RESIZED;

    e.window_resized.width =
        (int)frame.size.width;

    e.window_resized.height =
        (int)frame.size.height;

    // process_event(&e);
}

@end

/**
 * @brief Translates input events into a generic mac os event type so all mac events can be handled together.
 * @param ns_event A point to the NSEvent.
 * @return A MacEvent containing the relavent data.
 */
static MacEvent translate_ns_event(NSEvent* ns_event) {
  MacEvent e;
  e.event_type = MAC_EVENT_TYPE_NONE;

  switch ([ns_event type]){
  case NSEventTypeKeyDown:
    break;

  case NSEventTypeKeyUp:
    break;

  case NSEventTypeMouseMoved: {
    NSPoint p =
      [ns_event locationInWindow];

    e.event_type = MAC_EVENT_TYPE_MOUSE_MOVED;

    e.mouse_moved.x_pos = p.x;
    e.mouse_moved.y_pos = p.y;
  } break;

  case NSEventTypeLeftMouseDown: {
    NSPoint p =
      [ns_event locationInWindow];
    
    e.event_type = MAC_EVENT_TYPE_MOUSE_BUTTON_DOWN;
    e.mouse_button_down.button = 0;
    
    e.mouse_button_down.x_pos = p.x;
    e.mouse_button_down.y_pos = p.y;
    
  } break;

  case NSEventTypeLeftMouseUp:
    break;

  case NSEventTypeRightMouseDown:
    break;

  case NSEventTypeRightMouseUp:
    break;

  case NSEventTypeScrollWheel:
    break;

  default:
    break;
  }
  return e;
}

/**
 * @brief Handles how differnet MacEvents are used.
 * @param e A pointer to the MacEvent.
 */
static void process_event(MacEvent* e) {
  switch (e->event_type) {
  case MAC_EVENT_TYPE_MOUSE_MOVED:
    // MTRACE("Mouse moved: (%f, %f)", e->mouse_moved.x_pos, e->mouse_moved.y_pos);
    break;

  default:
    break;
  }

}

#endif
