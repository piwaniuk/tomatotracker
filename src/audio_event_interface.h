#ifndef AUDIO_EVENT_INTERFACE_H
#define AUDIO_EVENT_INTERFACE_H

#include "audio_event.h"

typedef struct {
  char (*fill)(AudioEvent* event, sample_t* buffer, size_t len);
  void (*destroy)(AudioEvent* event);
} AudioEventInterface;

struct AudioEvent {
  const AudioEventInterface* interface;
  void* state;
};

void ae_state_destroy(AudioEvent* event);

#endif

