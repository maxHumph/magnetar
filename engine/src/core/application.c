#include "application.h"

#include "core/clock.h"
#include "core/events.h"
#include "core/input.h"
#include "core/log.h"
#include "game_interface.h"
#include "log.h"
#include "platform/platform.h"

typedef struct ApplicationState {
  Game* game_instance;

  b8 is_running;
  b8 is_suspended;

  PlatformState platform_state;

  i16 width;
  i16 height;

  Clock clock;
  f64 last_time;

} ApplicationState;

static b8 s_initialized = FALSE;
static ApplicationState s_application_state;

b8 application_on_event(u16 code, void* sender, void* listener, EventData event_data);
b8 application_on_key(u16 code, void* sender, void* listener, EventData event_data);

MGAPI b8 application_create(Game* game_instance) {
  if (s_initialized) {
    MERROR_CORE("application_create called multiple times!");
    return FALSE;
  }

  s_application_state.game_instance = game_instance;

  // Initialize subsystems.
  initialize_logging();

  if (!event_initialize()) {
    MERROR_CORE("Event subsystem failed to init.");
    return FALSE;
  }

  if (!input_initialize()) {
    MERROR_CORE("Input subsystem failed to init.");
    return FALSE;
  }

  s_application_state.is_running = TRUE;
  s_application_state.is_suspended = FALSE;

  event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
  event_register(EVENT_CODE_KEY_DOWN, 0, application_on_key);
  event_register(EVENT_CODE_KEY_UP, 0, application_on_key);

  // Setup window and other platform specific things.
  if (!platform_startup(
          &s_application_state.platform_state, game_instance->application_info.start_title,
          game_instance->application_info.start_x_pos, game_instance->application_info.start_y_pos,
          game_instance->application_info.start_width,
          game_instance->application_info.start_height)) {
    return FALSE;
  }

  // Initialize game.
  if (!s_application_state.game_instance->initialize(s_application_state.game_instance)) {
    MFATAL_CORE("Failed to initialize Game.");
    return FALSE;
  }

  s_application_state.game_instance->on_resize(
      s_application_state.game_instance, s_application_state.width, s_application_state.height);

  s_initialized = TRUE;

  return TRUE;
}

MGAPI b8 application_run() {
  clock_start(&s_application_state.clock);
  f64 delta_time = 0;
  s_application_state.last_time = s_application_state.clock.elapsed_time;
  while (s_application_state.is_running) {
    clock_update(&s_application_state.clock);
    delta_time = s_application_state.clock.elapsed_time - s_application_state.last_time;
    s_application_state.last_time = s_application_state.clock.elapsed_time;

    if (!platform_pump_messages(&s_application_state.platform_state)) {
      s_application_state.is_running = FALSE;
    }

    if (!s_application_state.is_suspended) {
      if (!s_application_state.game_instance->on_update(s_application_state.game_instance,
                                                        (f32)delta_time)) {
        MFATAL_CORE("on_update failed, EXITING PROCESS.");
        s_application_state.is_running = FALSE;
        break;
      }

      if (!s_application_state.game_instance->on_render(s_application_state.game_instance,
                                                        (f32)delta_time)) {
        MFATAL_CORE("on_render failed, EXITING PROCESS.");
        s_application_state.is_running = FALSE;
        break;
      }

      input_update(delta_time);
    }
  }
  s_application_state.is_running = FALSE;

  event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
  event_unregister(EVENT_CODE_KEY_DOWN, 0, application_on_key);
  event_unregister(EVENT_CODE_KEY_UP, 0, application_on_key);

  input_shutdown();
  event_shutdown();

  clock_stop(&s_application_state.clock);

  platform_shutdown(&s_application_state.platform_state);

  return FALSE;
}

b8 application_on_event(u16 code, void* sender, void* listener, EventData event_data) {
  switch (code) {
    case EVENT_CODE_APPLICATION_QUIT: {
      MINFO_CORE("Application QUIT");
      s_application_state.is_running = FALSE;
      return TRUE;
    }
  }

  return FALSE;
}

b8 application_on_key(u16 code, void* sender, void* listener, EventData event_data) {
  if (code == EVENT_CODE_KEY_DOWN) {
    Keys keycode = event_data.data.u16[0];
    if (keycode == KEY_ESCAPE && key_down(KEY_LSHIFT)) {
      EventData data = {};
      fire_event(EVENT_CODE_APPLICATION_QUIT, 0, data);
      return TRUE;
    }
  }
  return FALSE;
}
