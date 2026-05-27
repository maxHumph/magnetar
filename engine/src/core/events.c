#include "events.h"

#include "data_structures/darray.h"
#include "define.h"
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

  u64 registered_count = darray_get_length(s_state.registered[code].events);  

}  

b8 event_unregister(u16 code, void* listener, fp_on_event on_event);

b8 fire_event(u16 code, void* sender, EventData data);
