/**
 * @file platform_linux.c
 */

#include <xcb/xproto.h>

#include "define.h"
#include "platform.h"

#if MPLATFORM_LINUX

#include <X11/XKBlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/time.h>
#include <xcb/xcb.h>

#include "core/log.h"

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  //nanosleep
#else
#include <unistd.h>  //usleep
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct InternalState {
  Display* display;
  xcb_connection_t* connection;
  xcb_window_t window;
  xcb_screen_t* screen;
  xcb_atom_t wm_protocols;
  xcb_atom_t wm_delete_win;
} InternalState;

b8 platform_startup(PlatformState* platform_state, const char* application_name, i32 x_pos,
                    i32 y_pos, i32 width, i32 height) {
  platform_state->internal_state = malloc(sizeof(InternalState));
  InternalState* state = (InternalState*)platform_state->internal_state;

  state->display = XOpenDisplay(NULL_PTR);

  XAutoRepeatOff(state->display);

  state->connection = XGetXCBConnection(state->display);

  if (xcb_connection_has_error(state->connection)) {
    MFATAL_CORE("Failed to connect to X server via XCB.");
    return FALSE;
  }

  const struct xcb_setup_t* setup = xcb_get_setup(state->connection);

  // Loop through screens
  i32 screen_p = 0;
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  for (i32 i = screen_p; i > 0; i--) {
    xcb_screen_next(&iter);
  }

  state->screen = iter.data;

  state->window = xcb_generate_id(state->connection);

  // Register event types
  // Important stuff
  u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

  // Keyboard and mouse stuff
  u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                     XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                     XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                     XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  u32 value_list[] = {state->screen->black_pixel, event_values};

  // create window
  xcb_void_cookie_t cookie =
      xcb_create_window(state->connection, XCB_COPY_FROM_PARENT, state->window, state->screen->root,
                        x_pos, y_pos, width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                        state->screen->root_visual, event_mask, value_list

      );

  // set window title
  xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window, XCB_ATOM_WM_NAME,
                      XCB_ATOM_STRING, 8, strlen(application_name), application_name);

  // tell the x server to notify when the wm tries to destroy the window.
  xcb_intern_atom_cookie_t wm_delete_cookie =
      xcb_intern_atom(state->connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
  xcb_intern_atom_cookie_t wm_protocols_cookie =
      xcb_intern_atom(state->connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
  xcb_intern_atom_reply_t* wm_delete_cookie_reply =
      xcb_intern_atom_reply(state->connection, wm_delete_cookie, NULL_PTR);
  xcb_intern_atom_reply_t* wm_protocols_cookie_reply =
      xcb_intern_atom_reply(state->connection, wm_protocols_cookie, NULL_PTR);

  state->wm_delete_win = wm_delete_cookie_reply->atom;
  state->wm_protocols = wm_protocols_cookie_reply->atom;

  xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window,
                      wm_protocols_cookie_reply->atom, 4, 32, 1, &wm_delete_cookie_reply->atom);

  // map window onto screen
  xcb_map_window(state->connection, state->window);

  // flush stream
  i32 stream_result = xcb_flush(state->connection);
  if (stream_result <= 0) {
    MFATAL_CORE("An error occured while flushing stream: %d", stream_result);
    return FALSE;
  }

  return TRUE;
}

void platform_shutdown(PlatformState* platform_state) {
  InternalState* state = (InternalState*)platform_state->internal_state;
  XAutoRepeatOn(state->display);

  xcb_destroy_window(state->connection, state->window);
}

b8 platform_pump_messages(PlatformState* platform_state) {
  InternalState* state = (InternalState*)platform_state->internal_state;

  xcb_generic_event_t* event;
  xcb_client_message_event_t* cm;

  b8 quit_flagged = FALSE;

  // poll for events
  while (event != NULL_PTR) {
    event = xcb_poll_for_event(state->connection);
    if (event == 0) {
      break;
    }

    switch (event->response_type & ~0x80) {
      case XCB_KEY_PRESS:
        break;
      case XCB_KEY_RELEASE:
        break;
      case XCB_BUTTON_PRESS:
        break;
      case XCB_BUTTON_RELEASE:
        break;
      case XCB_MOTION_NOTIFY:  // mouse movement
        break;
      case XCB_CONFIGURE_NOTIFY:  // resizing
        break;
      case XCB_CLIENT_MESSAGE:
        if (cm->data.data32[0] == state->wm_delete_win) {
          quit_flagged = TRUE;
        }
        break;
      default:
        break;
    }
    free(event);
  }
  return !quit_flagged;
}

void* platform_alloc(u64 size, b8 is_aligned) { return malloc(size); }

void platform_free(void* block, b8 is_aligned) { free(block); }

void* platform_mem_zero(void* block, u64 size) { return memset(block, 0, size); }

void* platform_mem_cpy(void* out, const void* src, u64 size) { return memcpy(out, src, size); }

void* platform_mem_set(void* target, i32 val, u64 size) { return memset(target, val, size); }

void platform_print(const char* message, u8 colour) {
  if (colour == 0 || colour == 1) {
    printf("\x1b[37;41m%s\x1b[0m", message);
  } else if (colour == 2 || colour == 3) {
    printf("\x1b[31m%s\x1b[0m", message);
  } else if (colour == 4 || colour == 5) {
    printf("\x1b[33m%s\x1b[0m", message);
  } else if (colour == 6 || colour == 7) {
    printf("\x1b[35m%s\x1b[0m", message);
  } else if (colour == 8 || colour == 9) {
    printf("\x1b[37m%s\x1b[0m", message);
  } else if (colour == 10 || colour == 11) {
    printf("\x1b[36m%s\x1b[0m", message);
  }
}

void platform_print_error(const char* message, u8 colour) { printf("%s", message); }

f64 platform_get_time_abs() {
  struct timespec time_curr;
  clock_gettime(CLOCK_MONOTONIC, &time_curr);
  return time_curr.tv_sec + (time_curr.tv_nsec * 0.000000001);
}

void platform_sleep(u64 ms) {
#if _POSIX_C_SOURCE >= 199309L
  struct timespec time_spec;
  time_spec.tv_sec = ms / 1000;
  time_spec.tv_nsec = (ms % 1000) * 1000 * 1000;
  nanosleep(&time_spec, 0);
#else
  if (ms >= 1000) {
    sleep(ms / 1000);
  }
  usleep((ms % 1000) * 1000);
#endif
}

#endif
