#include "audio_event_interface.h"

void ae_state_destroy(AudioEvent* event) {
  free(event->state);
}

