/**
 * @file platform_win32.h
 * @brief Contains platform.h implementations for the win32 platform.
 */

#include "platform.h"

#if MPLATFORM_WINDOWS

#include <windows.h>
#include <windowsx.h>

typedef struct InternalState {
  HINSTANCE h_instance;
  HWND hwnd;
} InternalState;

b8 platform_startup(PlatformState* platform_state, const char* application_name, i32 x_pos,
                    i32 y_pos, i32 width, i32 height) {
  platform_state->internal_state = malloc(sizeof(InternalState));
  InternalState *state = (InternalState *)platform_state->internal_state;
}

#endif
