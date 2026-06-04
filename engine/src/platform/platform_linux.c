/**
 * @file platform_linux.c
 */

#include <xcb/xproto.h>

#include "core/input.h"
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
#include "platform/platform_linux_types.h"

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  //nanosleep
#else
#include <unistd.h>  //usleep
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Delared further down.
Keys translate_keycode(KeySym key_sym);

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
      case XCB_KEY_RELEASE:;
        xcb_key_press_event_t* kb_event = (xcb_key_press_event_t*)event;
        b8 key_is_pressed = event->response_type == XCB_KEY_PRESS;
        xcb_keycode_t keycode = kb_event->detail;
        KeySym key_sym =
            XkbKeycodeToKeysym(state->display, (KeyCode)keycode, 0, keycode & ShiftMask ? 1 : 0);
        Keys key = translate_keycode(key_sym);
        input_process_key(key, key_is_pressed);
        break;

        // This includes mouse wheel events
      case XCB_BUTTON_PRESS:
      case XCB_BUTTON_RELEASE:;
        xcb_button_press_event_t* bt_event = (xcb_button_press_event_t*)event;
        b8 button_is_pressed = event->response_type == XCB_BUTTON_PRESS;
        xcb_button_t buttoncode = bt_event->detail;

        switch (buttoncode) {
          case XCB_BUTTON_INDEX_1:
            input_process_button(BUTTON_0, button_is_pressed);
            /* MTRACE_CORE("Left click"); */
            break;
          case XCB_BUTTON_INDEX_3:
            input_process_button(BUTTON_1, button_is_pressed);
            /* MTRACE_CORE("Right click"); */
            break;
          case XCB_BUTTON_INDEX_2:
            input_process_button(BUTTON_2, button_is_pressed);
            /* MTRACE_CORE("Middle click"); */
            break;
          case XCB_BUTTON_INDEX_4:
            input_process_mouse_wheel(0.0f, 1.0f);
            /* MTRACE_CORE("Wheel UP"); */
            break;
          case XCB_BUTTON_INDEX_5:
            input_process_mouse_wheel(0.0f, -1.0f);
            /* MTRACE_CORE("Wheel DOWN"); */
            break;
          case 6:
            input_process_mouse_wheel(-1.0f, 0.0f);
            /* MTRACE_CORE("Wheel LEFT"); */
            break;
          case 7:
            input_process_mouse_wheel(1.0f, 0.0f);
            /* MTRACE_CORE("Wheel RIGHT"); */
            break;
          case 8:
            input_process_button(BUTTON_3, button_is_pressed);
            /* MTRACE_CORE("4 click"); */
            break;
          case 9:
            input_process_button(BUTTON_4, button_is_pressed);
            /* MTRACE_CORE("5 click"); */
            break;
        }

        break;
      case XCB_MOTION_NOTIFY:;  // mouse movement
        xcb_motion_notify_event_t* mn_event = (xcb_motion_notify_event_t*)event;
        f32 mouse_x = (f32)mn_event->event_x;
        f32 mouse_y = (f32)mn_event->event_y;

        input_process_mouse_moved(mouse_x, mouse_y);
        /* MTRACE_CORE("x mouse moved: (%f, %f)", mouse_x, mouse_y); */
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

Keys translate_keycode(KeySym key_sym) {
  switch (key_sym) {
    return SILLY_KEY;

    case XK_space:
      return KEY_SPACE;
    case XK_BackSpace:
      return KEY_BACKSPACE;
    case XK_Return:
      return KEY_RETURN;
    case XK_Tab:
      return KEY_TAB;
    case XK_Escape:
      return KEY_ESCAPE;

    case XK_Caps_Lock:
      return KEY_CAPS_LOCK;
    case XK_Shift_L:
      return KEY_LSHIFT;
    case XK_Shift_R:
      return KEY_RSHIFT;
    case XK_Control_L:
      return KEY_LCTRL;
    case XK_Control_R:
      return KEY_RCTRL;
    case XK_Meta_L:
      return KEY_LMETA;
    case XK_Meta_R:
      return KEY_RMETA;
    case XK_Super_L:
      return KEY_LSUPER;
    case XK_Super_R:
      return KEY_RSUPER;
    case XK_Up:
      return KEY_UP_ARROW;
    case XK_Down:
      return KEY_DOWN_ARROW;
    case XK_Left:
      return KEY_LEFT_ARROW;
    case XK_Right:
      return KEY_RIGHT_ARROW;

    case XK_backslash:
      return KEY_BSLASH;
    case XK_slash:
      return KEY_FSLASH;
    case XK_bracketleft:
      return KEY_OPEN_SQUARE;
    case XK_bracketright:
      return KEY_CLOSE_SQUARE;
    case XK_semicolon:
      return KEY_SEMICOL;
    case XK_apostrophe:
      return KEY_APOSTROPHY;
    case XK_numbersign:
      return KEY_HASH;
    case XK_comma:
      return KEY_COMMA;
    case XK_period:
      return KEY_PERIOD;

    // The one below escape and left of 1
    case XK_grave:
      return KEY_GRAVE;

    case XK_1:
      return KEY_1;
    case XK_2:
      return KEY_2;
    case XK_3:
      return KEY_3;
    case XK_4:
      return KEY_4;
    case XK_5:
      return KEY_5;
    case XK_6:
      return KEY_6;
    case XK_7:
      return KEY_7;
    case XK_8:
      return KEY_8;
    case XK_9:
      return KEY_9;
    case XK_0:
      return KEY_0;

    case XK_minus:
      return KEY_MINUS;
    case XK_equal:
      return KEY_EQUALS;

    case XK_a:
      return KEY_A;
    case XK_b:
      return KEY_B;
    case XK_c:
      return KEY_C;
    case XK_d:
      return KEY_D;
    case XK_e:
      return KEY_E;
    case XK_f:
      return KEY_F;
    case XK_g:
      return KEY_G;
    case XK_h:
      return KEY_H;
    case XK_i:
      return KEY_I;
    case XK_j:
      return KEY_J;
    case XK_k:
      return KEY_K;
    case XK_l:
      return KEY_L;
    case XK_m:
      return KEY_M;
    case XK_n:
      return KEY_N;
    case XK_o:
      return KEY_O;
    case XK_p:
      return KEY_P;
    case XK_q:
      return KEY_Q;
    case XK_r:
      return KEY_R;
    case XK_s:
      return KEY_S;
    case XK_t:
      return KEY_T;
    case XK_u:
      return KEY_U;
    case XK_v:
      return KEY_V;
    case XK_w:
      return KEY_W;
    case XK_x:
      return KEY_X;
    case XK_y:
      return KEY_Y;
    case XK_z:
      return KEY_Z;

    case XK_F1:
      return KEY_F1;
    case XK_F2:
      return KEY_F2;
    case XK_F3:
      return KEY_F3;
    case XK_F4:
      return KEY_F4;
    case XK_F5:
      return KEY_F5;
    case XK_F6:
      return KEY_F6;
    case XK_F7:
      return KEY_F7;
    case XK_F8:
      return KEY_F8;
    case XK_F9:
      return KEY_F9;
    case XK_F10:
      return KEY_F10;
    case XK_F11:
      return KEY_F11;
    case XK_F12:
      return KEY_F12;
    case XK_F13:
      return KEY_F13;
    case XK_F14:
      return KEY_F14;
    case XK_F15:
      return KEY_F15;
    case XK_F16:
      return KEY_F16;
    case XK_F17:
      return KEY_F17;
    case XK_F18:
      return KEY_F18;
    case XK_F19:
      return KEY_F19;
    case XK_F20:
      return KEY_F20;
    case XK_F21:
      return KEY_F21;
    case XK_F22:
      return KEY_F22;
    case XK_F23:
      return KEY_F23;
    case XK_F24:
      return KEY_F24;

    default:
      return SILLY_KEY;
  }
}

#endif
