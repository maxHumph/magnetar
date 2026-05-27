#include "application.h"

#include "core/events.h"
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
  f64 last_time;

} ApplicationState;

static b8 s_initialized = FALSE;
static ApplicationState s_application_state;

MGAPI b8 application_create(Game* game_instance) {
  if (s_initialized) {
    MERROR("application_create called multiple times!");
    return FALSE;
  }

  s_application_state.game_instance = game_instance;

  // Initialize subsystems.
  initialize_logging();

  if (!event_initialize()) {
    MERROR("Event subsystem failed to init.");
    return FALSE;
  }

  s_application_state.is_running = TRUE;
  s_application_state.is_suspended = FALSE;

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
    MFATAL("Failed to initialize Game.");
    return FALSE;
  }

  s_application_state.game_instance->on_resize(
      s_application_state.game_instance, s_application_state.width, s_application_state.height);

  s_initialized = TRUE;

  return TRUE;
}

MGAPI b8 application_run() {
  while (s_application_state.is_running) {
    if (!platform_pump_messages(&s_application_state.platform_state)) {
      s_application_state.is_running = FALSE;
    }

    if (!s_application_state.is_suspended) {
      if (!s_application_state.game_instance->on_update(s_application_state.game_instance,
                                                        (f32)0)) {
        MFATAL("on_update failed, EXITING PROCESS.");
        s_application_state.is_running = FALSE;
        break;
      }

      if (!s_application_state.game_instance->on_render(s_application_state.game_instance,
                                                        (f32)0)) {
        MFATAL("on_render failed, EXITING PROCESS.");
        s_application_state.is_running = FALSE;
        break;
      }
    }
  }
  s_application_state.is_running = FALSE;

  event_shutdown();

  platform_shutdown(&s_application_state.platform_state);

  return FALSE;
}
