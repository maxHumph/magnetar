/**
 * @file platform_mac.m
 * @brief MacOS API specific stuff written in disgusting Objective C.
 *
 * Warning: I don't understand objective-c so this code is probably terrible.
 */

#include "define.h"

#if defined(MPLATFORM_APPLE)

#include "platform.h"
#include "core/log.h"
#include "core/input.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <mach/mach_time.h>

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

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
  u16 keycode;
} MacEventKeyDown;

typedef struct MacEventKeyUp {
  u16 keycode;
} MacEventKeyUp;

typedef struct MacEventMouseMoved {
  f32 x_pos;
  f32 y_pos;
} MacEventMouseMoved;

typedef struct MacEventMouseButtonDown {
  u16 button;
  f32 x_pos;
  f32 y_pos;
} MacEventMouseButtonDown;

typedef struct MacEventMouseButtonUp{
  i32 button;
  f32 x_pos;
  f32 y_pos;
} MacEventMouseButtonUp;

typedef struct MacEventScrollWheel {
  f32 delta_x;
  f32 delta_y;
} MacEventScrollWheel;

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
    MacEventScrollWheel scroll_wheel;
  };
  
} MacEvent;

/**
 * @brief Used by the engine to access os specific API code.
 */
typedef struct InternalState {
  NSWindow* ns_window;
  // CAMetalLayer* layer;
  id window_delegate;
} InternalState;


/**
 * @brief Containds methods for handling non-input window events like window_resize or window_close. (Implemented further down).
 */
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
    [NSString stringWithUTF8String:(application_name ? application_name : "Window")];
  
  [window setTitle:nsTitle];
  
  state->ns_window = window;
  
  state->window_delegate =
    [[WindowDelegate alloc] init];
  
  [state->ns_window
      setDelegate:state->window_delegate];
  
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

// - (BOOL)WindowShouldClose:(id)sender{
//   MacEvent e;
//   e.event_type = MAC_EVENT_TYPE_QUIT;

//   process_event(&e);

//   return YES;
// }

- (void)windowWillClose:(NSNotification *)notification
{
  MacEvent e;
  e.event_type = MAC_EVENT_TYPE_QUIT;
  
  process_event(&e);
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
  
  process_event(&e);
}


@end
static Keys translate_mac_keycode(NSEvent* ns_event) {
  
  switch ([ns_event keyCode]) {
    // A-Z
  case kVK_ANSI_A:
    return KEY_A;
  case kVK_ANSI_B:
    return KEY_B;
  case kVK_ANSI_C:
    return KEY_C;
  case kVK_ANSI_D:
    return KEY_D;
  case kVK_ANSI_E:
    return KEY_E;
  case kVK_ANSI_F:
    return KEY_F;
  case kVK_ANSI_G:
    return KEY_G;
  case kVK_ANSI_H:
    return KEY_H;
  case kVK_ANSI_I:
    return KEY_I;
  case kVK_ANSI_J:
    return KEY_J;
  case kVK_ANSI_K:
    return KEY_K;
  case kVK_ANSI_L:
    return KEY_L;
  case kVK_ANSI_M:
    return KEY_M;
  case kVK_ANSI_N:
    return KEY_N;
  case kVK_ANSI_O:
    return KEY_O;
  case kVK_ANSI_P:
    return KEY_P;
  case kVK_ANSI_Q:
    return KEY_Q;
  case kVK_ANSI_R:
    return KEY_R;
  case kVK_ANSI_S:
    return KEY_S;
  case kVK_ANSI_T:
    return KEY_T;
  case kVK_ANSI_U:
    return KEY_U;
  case kVK_ANSI_V:
    return KEY_V;
  case kVK_ANSI_W:
    return KEY_W;
  case kVK_ANSI_X:
    return KEY_X;
  case kVK_ANSI_Y:
    return KEY_Y;
  case kVK_ANSI_Z:
    return KEY_Z;
    
  case kVK_ANSI_1:
    return  KEY_1;
  case kVK_ANSI_2:
    return  KEY_2;
  case kVK_ANSI_3:
    return  KEY_3;
  case kVK_ANSI_4:
    return  KEY_4;
  case kVK_ANSI_5:
    return  KEY_5;
  case kVK_ANSI_6:
    return  KEY_6;
  case kVK_ANSI_7:
    return  KEY_7;
  case kVK_ANSI_8:
    return  KEY_8;
  case kVK_ANSI_9:
    return  KEY_9;
  case kVK_ANSI_0:
    return  KEY_0;
    
    // Moving around thingys
    
  case kVK_Space:
    return KEY_SPACE; 
  case kVK_Delete:
    return KEY_BACKSPACE; 
  case kVK_Return:
    return KEY_RETURN;
  case kVK_Tab:
    return KEY_TAB;
  case kVK_Escape:
    return KEY_ESCAPE;
    
    // Modifiers and arrows
    
  case kVK_CapsLock:
    return KEY_CAPS_LOCK;
  case kVK_Shift:
    return KEY_LSHIFT;
  case kVK_RightShift:
    return KEY_RSHIFT;
  case kVK_Control:
    return KEY_LCTRL;
  case kVK_RightControl:
    return KEY_RCTRL;
  case kVK_Option:
    return KEY_LMETA;
  case kVK_RightOption:
    return KEY_RMETA;
  case kVK_Command:
    return KEY_LSUPER;
  case kVK_RightCommand:
    return KEY_RSUPER;
  case kVK_UpArrow:
    return KEY_UP_ARROW;
  case kVK_DownArrow:
    return KEY_DOWN_ARROW;
  case kVK_LeftArrow:
    return KEY_LEFT_ARROW;
  case kVK_RightArrow:
    return KEY_RIGHT_ARROW;
    
    // Symbols
  case kVK_ANSI_Minus:
    return KEY_MINUS;
  case kVK_ANSI_Equal:
    return KEY_EQUALS;
    
  case kVK_ANSI_Backslash:
    return KEY_BSLASH;
  case kVK_ANSI_Slash:
    return KEY_FSLASH;
  case kVK_ANSI_LeftBracket:
    return KEY_OPEN_SQUARE;
  case kVK_ANSI_RightBracket:
    return KEY_CLOSE_SQUARE;
  case kVK_ANSI_Semicolon:
    return KEY_SEMICOL;
  case kVK_ANSI_Quote:
    return KEY_APOSTROPHY;
    //   case kVK_?
    // KEY_HASH = 0x26,
  case kVK_ANSI_Comma:
    return KEY_COMMA;
  case kVK_ANSI_Period:
    return KEY_PERIOD;
  case kVK_ANSI_Grave:
    return KEY_GRAVE;
    
    // Func keys 
    
  case kVK_F1:
    return KEY_F1;
  case kVK_F2:
    return KEY_F2;
  case kVK_F3:
    return KEY_F3;
  case kVK_F4:
    return KEY_F4;
  case kVK_F5:
    return KEY_F5;
  case kVK_F6:
    return KEY_F6;
  case kVK_F7:
    return KEY_F7;
  case kVK_F8:
    return KEY_F8;
  case kVK_F9:
    return KEY_F9;
  case kVK_F10:
    return KEY_F10;
  case kVK_F11:
    return KEY_F11;
  case kVK_F12:
    return KEY_F12;
  case kVK_F13:
    return KEY_F13;
  case kVK_F14:
    return KEY_F14;
  case kVK_F15:
    return KEY_F15;
  case kVK_F16:
    return KEY_F16;
  case kVK_F17:
    return KEY_F17;
  case kVK_F18:
    return KEY_F18;
  case kVK_F19:
    return KEY_F19;
  case kVK_F20:
    return KEY_F20;
    // Not supported by MacOS
    /*
      KEY_F21
      KEY_F22
      KEY_F23
      KEY_F24
    */
    
  default:
    return SILLY_KEY;
  }
}

/**
 * @brief Translates input events into a generic mac os event type so all mac events can be handled together.
 * @param ns_event A point to the NSEvent.
 * @return A MacEvent containing the relavent data.
 */
static MacEvent translate_ns_event(NSEvent* ns_event) {
  MacEvent e;
  e.event_type = MAC_EVENT_TYPE_NONE;
  
  switch ([ns_event type]){
    
    // For modifier keys
  case NSEventTypeFlagsChanged:
    {
      Keys key =
        translate_mac_keycode(ns_event);
      
      NSEventModifierFlags flags =
        [ns_event modifierFlags];
      
      b8 pressed = FALSE;
      
      switch ([ns_event keyCode]) {
      case kVK_Shift:
      case kVK_RightShift:
	pressed =
	  (flags & NSEventModifierFlagShift) != 0;
	break;
	
      case kVK_Control:
      case kVK_RightControl:
	pressed =
	  (flags & NSEventModifierFlagControl) != 0;
	break;
	
      case kVK_Option:
      case kVK_RightOption:
	pressed =
	  (flags & NSEventModifierFlagOption) != 0;
	break;
	
      case kVK_Command:
      case kVK_RightCommand:
	pressed =
	  (flags & NSEventModifierFlagCommand) != 0;
	break;
	
      case kVK_CapsLock:
	pressed =
	  (flags & NSEventModifierFlagCapsLock) != 0;
	break;
      }
      
      e.event_type =
        pressed
	? MAC_EVENT_TYPE_KEY_DOWN
	: MAC_EVENT_TYPE_KEY_UP;
      
      if (pressed) {
	e.key_down.keycode = key;
      } else {
        e.key_up.keycode = key;
      }
      
    } break;
    
  case NSEventTypeKeyDown : {
    e.event_type = MAC_EVENT_TYPE_KEY_DOWN;
    e.key_down.keycode = translate_mac_keycode(ns_event);
  } break;
    
  case NSEventTypeKeyUp: {
    e.event_type = MAC_EVENT_TYPE_KEY_UP;
    e.key_up.keycode = translate_mac_keycode(ns_event);
  } break;
    
  case NSEventTypeMouseMoved: {
    NSPoint p =
      [ns_event locationInWindow];
    
    e.event_type = MAC_EVENT_TYPE_MOUSE_MOVED;
    
    e.mouse_moved.x_pos = (f32)p.x;
    e.mouse_moved.y_pos = (f32)p.y;
  } break;
    
  case NSEventTypeLeftMouseDown: {
    // NSPoint p =
    //   [ns_event locationInWindow];
    
    e.event_type = MAC_EVENT_TYPE_MOUSE_BUTTON_DOWN;
    e.mouse_button_down.button = BUTTON_0;
    
    // e.mouse_button_down.x_pos = p.x;
    // e.mouse_button_down.y_pos = p.y;
    
  } break;
    
  case NSEventTypeLeftMouseUp:
    // NSPoint p =
    //   [ns_event locationInWindow];
    
    e.event_type = MAC_EVENT_TYPE_MOUSE_BUTTON_UP;
    e.mouse_button_up.button = BUTTON_0;
    
    // e.mouse_button_up.x_pos = p.x;
    // e.mouse_button_up.y_pos = p.y;
    break;
    
  case NSEventTypeRightMouseDown:
    // NSPoint p =
    //   [ns_event locationInWindow];
    
    e.event_type = MAC_EVENT_TYPE_MOUSE_BUTTON_DOWN;
    e.mouse_button_down.button = BUTTON_1;
    
    // e.mouse_button_down.x_pos = p.x;
    // e.mouse_button_down.y_pos = p.y;
    break;
    
  case NSEventTypeRightMouseUp:
    // NSPoint p =
    //   [ns_event locationInWindow];
    
    e.event_type = MAC_EVENT_TYPE_MOUSE_BUTTON_UP;
    e.mouse_button_up.button = BUTTON_1;
    
    // e.mouse_button_up.x_pos = p.x;
    // e.mouse_button_up.y_pos = p.y;
    break;
    break;
    
  case NSEventTypeScrollWheel:
    e.event_type = MAC_EVENT_TYPE_SCROLL_WHEEL;
    e.scroll_wheel.delta_x = (f32)([ns_event deltaX]);
    e.scroll_wheel.delta_y = (f32)([ns_event deltaY]);
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
    
    // Doesnt work
  case MAC_EVENT_TYPE_QUIT:
    // MINFO_CORE("Application QUIT");
    platform_sleep(100);
    exit(0);
    break;
    
  case MAC_EVENT_TYPE_WINDOW_RESIZED:
    // MTRACE_CORE("Window Resized: (%i, %i)", e->window_resized.width, e->window_resized.height);
    break;
    
  case MAC_EVENT_TYPE_KEY_DOWN:
    input_process_key(e->key_down.keycode, TRUE);
    // MTRACE_CORE("Keydown: %d", e->key_down.keycode);
    break;
    
  case MAC_EVENT_TYPE_KEY_UP:
    input_process_key(e->key_up.keycode, FALSE);
    // MTRACE_CORE("Keyup: %d", e->key_up.keycode);
    break;
    
  case MAC_EVENT_TYPE_MOUSE_BUTTON_DOWN:
    input_process_button(e->mouse_button_down.button, TRUE);
    // MTRACE_CORE("Button down: %d", e->mouse_button_down.button);
    break;
    
  case MAC_EVENT_TYPE_MOUSE_BUTTON_UP:
    input_process_button(e->mouse_button_up.button, FALSE);
    // MTRACE_CORE("Button up: %d", e->mouse_button_up.button);
    break;
    
  case MAC_EVENT_TYPE_MOUSE_MOVED:
    input_process_mouse_moved(e->mouse_moved.x_pos, e->mouse_moved.y_pos);
    // MTRACE_CORE("Mouse moved: (%f, %f)", e->mouse_moved.x_pos, e->mouse_moved.y_pos);
    break;
    
  case MAC_EVENT_TYPE_SCROLL_WHEEL:
    input_process_mouse_wheel(e->scroll_wheel.delta_x, e->scroll_wheel.delta_y);
    // MTRACE_CORE("Mouse Wheel: (%f, %f)", e->scroll_wheel.delta_x, e->scroll_wheel.delta_y);
    break;
    
  default:
    break;
  }
  
}


#endif
