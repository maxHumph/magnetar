#include "input.h"

#include "core/events.h"
#include "core/mmemory.h"
#include "maths/vector.h"

typedef struct KeyboardState {
  b8 keys[256];
} KeyboardState;

typedef struct MouseState {
  i16 x;
  i16 y;
  u8 buttons[BUTTON_MAX_BUTTONS];
} MouseState;

typedef struct InputState {
  KeyboardState keyboard_state_curr;
  KeyboardState keyboard_state_prev;

  MouseState mouse_state_curr;
  MouseState mouse_state_prev;
} InputState;

static b8 s_initialized = FALSE;
static InputState s_input_state = {};

b8 input_initialize() {
  mzero_memory(&s_input_state, sizeof(InputState));

  s_initialized = TRUE;
  return TRUE;
}

void input_shutdown() { s_initialized = FALSE; }

void input_update(f64 delta_time) {
  if (s_initialized == FALSE) {
    return;
  }

  mcopy_memory(&s_input_state.keyboard_state_prev, &s_input_state.keyboard_state_prev,
               sizeof(KeyboardState));
  mcopy_memory(&s_input_state.mouse_state_prev, &s_input_state.mouse_state_prev,
               sizeof(MouseState));
}

MGAPI b8 key_down(Keys key) { return s_input_state.keyboard_state_curr.keys[key] == TRUE; }

MGAPI b8 key_up(Keys key) { return s_input_state.keyboard_state_curr.keys[key] == FALSE; }

MGAPI b8 key_down_prev(Keys key) { return s_input_state.keyboard_state_prev.keys[key] == TRUE; }

MGAPI b8 key_up_prev(Keys key) { return s_input_state.keyboard_state_prev.keys[key] == FALSE; }

void input_process_key(Keys key, b8 is_pressed) {
  if (s_input_state.keyboard_state_curr.keys[key] != is_pressed) {
    s_input_state.keyboard_state_curr.keys[key] = is_pressed;

    EventData event_data;
    event_data.data.u16[0] = key;

    if (is_pressed) {
      fire_event(EVENT_CODE_KEY_DOWN, 0, event_data);
    } else {
      fire_event(EVENT_CODE_KEY_UP, 0, event_data);
    }
  }
}

MGAPI b8 button_down(Buttons button) {
  return s_input_state.mouse_state_curr.buttons[button] == TRUE;
}

MGAPI b8 button_up(Buttons button) {
  return s_input_state.mouse_state_curr.buttons[button] == FALSE;
}

MGAPI b8 button_down_prev(Buttons button) {
  return s_input_state.mouse_state_prev.buttons[button] == TRUE;
}

MGAPI b8 button_up_prev(Buttons button) {
  return s_input_state.mouse_state_prev.buttons[button] == FALSE;
}

MGAPI Vec2i16 mouse_pos() {
  Vec2i16 pos;
  pos.x = s_input_state.mouse_state_curr.x;
  pos.y = s_input_state.mouse_state_curr.y;
  return pos;
}

MGAPI Vec2i16 mouse_pos_prev() {
  Vec2i16 pos;
  pos.x = s_input_state.mouse_state_prev.x;
  pos.y = s_input_state.mouse_state_prev.y;
  return pos;
}

void input_process_button(Buttons button, b8 is_pressed) {
  if (s_input_state.mouse_state_curr.buttons[button] != is_pressed) {
    s_input_state.mouse_state_curr.buttons[button] = is_pressed;

    EventData event_data;
    event_data.data.u16[0] = button;

    if (is_pressed) {
      fire_event(EVENT_CODE_BUTTON_DOWN, 0, event_data);
    } else {
      fire_event(EVENT_CODE_BUTTON_UP, 0, event_data);
    }
  }
}

void input_process_mouse_moved(i16 x, i16 y) {
  if (s_input_state.mouse_state_curr.x != x || s_input_state.mouse_state_curr.y != y) {
    s_input_state.mouse_state_curr.x = x;
    s_input_state.mouse_state_curr.y = y;

    EventData event_data;
    event_data.data.i16[0] = x;
    event_data.data.i16[1] = y;

    fire_event(EVENT_CODE_MOUSE_MOVED, 0, event_data);
  }
}

void imput_process_mouse_wheel(i8 delta) {
  EventData event_data;
  event_data.data.u8[0] = delta;
  fire_event(EVENT_CODE_MOUSE_WHEEL, 0, event_data);
}
