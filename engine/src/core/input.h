/**
 * Copyright 2026 Max Humphreys
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file input.h
 * @brief Contains functions for polling input, sending platform api input data to the engine and
 * defines the key and button codes used by the engine.
 */

#pragma once

#include "define.h"
#include "maths/vector.h"

/**
 * @brief Codes for button input.
 */
typedef enum Buttons {
  // left mouse
  BUTTON_0,
  // right mouse
  BUTTON_1,
  // middle mouse
  BUTTON_2,
  // mouse button 4
  BUTTON_3,
  // mouse button 5
  BUTTON_4,

  BUTTON_MAX_BUTTONS,
} Buttons;

/**
 * @brief Engine key-codes.
 */
typedef enum Keys {

  SILLY_KEY = 0x00,

  KEY_SPACE = 0x01,
  KEY_BACKSPACE = 0x02,
  KEY_RETURN = 0x03,
  KEY_TAB = 0x04,
  KEY_ESCAPE = 0x05,

  KEY_CAPS_LOCK = 0x10,
  KEY_LSHIFT = 0x11,
  KEY_RSHIFT = 0x12,
  KEY_LCTRL = 0x13,
  KEY_RCTRL = 0x14,
  KEY_LMETA = 0x15,
  KEY_RMETA = 0x16,
  KEY_LSUPER = 0x17,
  KEY_RSUPER = 0x18,
  KEY_UP_ARROW = 0x19,
  KEY_DOWN_ARROW = 0x1A,
  KEY_LEFT_ARROW = 0x1B,
  KEY_RIGHT_ARROW = 0x1C,

  KEY_BSLASH = 0x20,
  KEY_FSLASH = 0x21,
  KEY_OPEN_SQUARE = 0x22,
  KEY_CLOSE_SQUARE = 0x23,
  KEY_SEMICOL = 0x24,
  KEY_APOSTROPHY = 0x25,
  KEY_HASH = 0x26,
  KEY_COMMA = 0x27,
  KEY_PERIOD = 0x28,

  // The one below escape and left of 1
  KEY_GRAVE = 0x30,

  KEY_1 = 0x31,
  KEY_2 = 0x32,
  KEY_3 = 0x33,
  KEY_4 = 0x34,
  KEY_5 = 0x35,
  KEY_6 = 0x36,
  KEY_7 = 0x37,
  KEY_8 = 0x38,
  KEY_9 = 0x39,
  KEY_0 = 0x3A,

  KEY_MINUS = 0x3B,
  KEY_EQUALS = 0x3C,

  KEY_A = 0x41,
  KEY_B = 0x42,
  KEY_C = 0x43,
  KEY_D = 0x44,
  KEY_E = 0x45,
  KEY_F = 0x46,
  KEY_G = 0x47,
  KEY_H = 0x48,
  KEY_I = 0x49,
  KEY_J = 0x4A,
  KEY_K = 0x4B,
  KEY_L = 0x4C,
  KEY_M = 0x4D,
  KEY_N = 0x4E,
  KEY_O = 0x4F,
  KEY_P = 0x50,
  KEY_Q = 0x51,
  KEY_R = 0x52,
  KEY_S = 0x53,
  KEY_T = 0x54,
  KEY_U = 0x55,
  KEY_V = 0x56,
  KEY_W = 0x57,
  KEY_X = 0x58,
  KEY_Y = 0x59,
  KEY_Z = 0x5A,

  KEY_F1 = 0x70,
  KEY_F2 = 0x71,
  KEY_F3 = 0x72,
  KEY_F4 = 0x73,
  KEY_F5 = 0x74,
  KEY_F6 = 0x75,
  KEY_F7 = 0x76,
  KEY_F8 = 0x77,
  KEY_F9 = 0x78,
  KEY_F10 = 0x79,
  KEY_F11 = 0x7A,
  KEY_F12 = 0x7B,
  KEY_F13 = 0x7C,
  KEY_F14 = 0x7D,
  KEY_F15 = 0x7E,
  KEY_F16 = 0x7F,
  KEY_F17 = 0x80,
  KEY_F18 = 0x81,
  KEY_F19 = 0x82,
  KEY_F20 = 0x83,
  KEY_F21 = 0x84,
  KEY_F22 = 0x85,
  KEY_F23 = 0x86,
  KEY_F24 = 0x86,

  KEY_MAX_KEYS

} Keys;

/**
 * @brief Initializes the input subsystem.
 * @return TRUE if initialization was successful, FALSE if it failed.
 */
b8 input_initialize();

void input_shutdown();

/**
 * @brief
 * @param delta_time
 *
 * Called once per frame.
 */
void input_update(f64 delta_time);

// Key input

/**
 * @brief Returns whether a key is currently down.
 * @param The keycode to check for. (KEY_...).
 * @return TRUE if the key is down, otherwise FALSE.
 */
MGAPI b8 key_down(Keys key);

/**
 * @brief Returns whether a key is currently up.
 * @param The keycode to check for. (KEY_...).
 * @return TRUE if the key is up, otherwise FALSE.
 */
MGAPI b8 key_up(Keys key);

/**
 * @brief Return whether a key was down in the previous frame.
 * @param The keycode to check for. (KEY_...).
 * @return TRUE if the key was down, otherwise FALSE.
 */
MGAPI b8 key_down_prev(Keys key);

/**
 * @brief Return whether a key was up in the previous frame.
 * @param The keycode to check for. (KEY_...).
 * @return TRUE if the key was up, otherwise FALSE.
 */
MGAPI b8 key_up_prev(Keys key);

/**
 * @brief Used in platform specific code sent key input data to the engine.
 * @param key The keycode to process.
 * @param is_pressed TRUE if the key is down, FALSE if it is UP.
 */
void input_process_key(Keys key, b8 is_pressed);

// Mouse input

/**
 * @brief Returns whether a mouse button is currently down.
 * @param button The button_code to check for.
 * @return TRUE if the button is down, otherwise FALSE.
 */
MGAPI b8 button_down(Buttons button);

/**
 * @brief Returns whether a mouse button is currently up.
 * @param button The button_code to check for.
 * @return TRUE if the button is up, otherwise FALSE.
 */
MGAPI b8 button_up(Buttons button);

/**
 * @brief Returns whether a mouse button was down on the previous frame.
 * @param button The button_code to check for.
 * @return TRUE if the button was down, otherwise FALSE.
 */
MGAPI b8 button_down_prev(Buttons button);

/**
 * @brief Returns whether a mouse button was up on the previous frame.
 * @param button The button_code to check for.
 * @return TRUE if the button was up, otherwise FALSE.
 */
MGAPI b8 button_up_prev(Buttons button);

/**
 * @brief Gets the current position of the mouse.
 * @return The mouse position as a Vec2f32.
 */
MGAPI Vec2f32 mouse_pos();

/**
 * @brief Gets the position of the mouse on the previous frame.
 * @return The previous mouse position as a Vec2f32.
 */
MGAPI Vec2f32 mouse_pos_prev();

/**
 * @brief Used in platform specific code to send button input data to the engine.
 * @param button The button code to process.
 * @param is_pressed TRUE if the button is down, FALSE if it is UP.
 */
void input_process_button(Buttons button, b8 is_pressed);

/**
 * @brief Used in platform specific code to send mouse position input data to the engine.
 * @param x The x position of the mouse.
 * @param y The y position of the mouse.
 */
void input_process_mouse_moved(f32 x, f32 y);

/**
 * @brief Used in platform specific code to send mouse wheel input data to the engine.
 * @param delta The direction of movement.
 *
 * Mouse wheel input can currently only be obtained through the Event subsystem hence why there are
 * no mouse wheel polling functions.
 */
void input_process_mouse_wheel(f32 delta_x, f32 delta_y);
