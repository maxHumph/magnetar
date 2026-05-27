#include "events.h"

#include "data_structures/darray.h"
#include "define.h"
#include "log.h"
#include "mmemory.h"

typedef struct RegisteredEvent {
  void* listener;
  fp_on_event callback;
} RegisteredEvent;

typedef struct EventCodeEntry {
  RegisteredEvent* events;
} EventCodeEntry;

#define MAX_MESSAGE_CODES 16384

typedef struct EventSystemState {
  EventCodeEntry registered[MAX_MESSAGE_CODES];
} EventSystemState;

static b8 s_initialized = FALSE;
static EventSystemState s_state;

b8 event_initialize() {
  if (s_initialized == TRUE) {
    return FALSE;
  }

  mzero_memory(&s_state, sizeof(s_state));
  s_initialized = TRUE;
  return TRUE;
}

void event_shutdown() {
  for (u16 i = 0; i < MAX_MESSAGE_CODES; i++) {
    if (s_state.registered[i].events != NULL_PTR) {
      darray_destroy(s_state.registered[i].events);
      s_state.registered[i].events = NULL_PTR;
    }
  }
}

b8 event_register(u16 code, void* listener, fp_on_event on_event) {
  if (s_initialized == FALSE) {
    return FALSE;
  }

  if (s_state.registered[code].events == NULL_PTR) {
    s_state.registered[code].events = darray_create(RegisteredEvent);
  }

  // check if listener is already registered.
  u64 registered_count = darray_get_length(s_state.registered[code].events);
  for (u64 i = 0; i < registered_count; i++) {
    if (s_state.registered[code].events[i].listener == listener) {
      MWARN("Listener has already been registered");
      return FALSE;
    }
  }

  RegisteredEvent event;
  event.listener = listener;
  event.callback = on_event;
  darray_push(s_state.registered[code].events, event);

  return TRUE;
}

b8 event_unregister(u16 code, void* listener, fp_on_event on_event) {
  if (s_initialized == FALSE) {
    return FALSE;
  }

  if (s_state.registered[code].events == NULL_PTR) {
    MWARN("Event: %u was not found in registered events.", code);
    return FALSE;
  }

  u64 registered_count = darray_get_length(s_state.registered[code].events);
  for (u64 i = 0; i < registered_count; i++) {
    RegisteredEvent registeredEvent = s_state.registered[code].events[i];
    if (registeredEvent.listener == listener && registeredEvent.callback == on_event) {
      RegisteredEvent bin;
      darray_pop_at(s_state.registered[code].events, i, &bin);
      return TRUE;
    }
  }

  return FALSE;
}

b8 fire_event(u16 code, void* sender, EventData data) {
  if (s_initialized == FALSE) {
    return FALSE;
  }

  if (s_state.registered[code].events == NULL_PTR) {
    MWARN("Event: %u could not be found in registered events.", code);
    return FALSE;
  }

  u64 registered_count = darray_get_length(s_state.registered[code].events);
  for (u64 i = 0; i < registered_count; i++) {
    RegisteredEvent registeredEvent = s_state.registered[code].events[i];
    if (registeredEvent.callback(code, sender, registeredEvent.listener, data)) {
      return TRUE;
    }
  }

  return FALSE;
}
