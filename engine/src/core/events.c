#include "events.h"

#include "mmemory.h"

typedef struct RegisteredEvent {
  void* listener;
  fp_on_event callback;
} RegisteredEvent;

typedef struct EventCodeEntry {
  RegisteredEvent* event;
} EventCodeEntry;

#define MAX_MESSAGE_CODES 16384

typedef struct EventSystemState {
  EventCodeEntry registered[MAX_MESSAGE_CODES];
} EventSystemState;

static b8 s_initialized = FALSE;
static EventSystemState s_state;
