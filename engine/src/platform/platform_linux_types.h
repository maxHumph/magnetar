/**
 * @file platform_linux_types.h
 */

#pragma once

#if MPLATFORM_LINUX

#include <X11/Xlib.h>
#include <xcb/xcb.h>

typedef struct InternalState {
  Display* display;
  xcb_connection_t* connection;
  xcb_window_t window;
  xcb_screen_t* screen;
  xcb_atom_t wm_protocols;
  xcb_atom_t wm_delete_win;
} InternalState;

#endif
