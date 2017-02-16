#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "audio_event.h"

typedef struct {
  char (*fill)(AudioEvent* event, sample_t* buffer, size_t len);
  void (*destroy)(AudioEvent* event);
} AudioEventInterface;

struct AudioEvent {
  const AudioEventInterface* interface;
  void* state;
};

char ae_fill(AudioEvent* event, sample_t* buffer, size_t len) {
  return event->interface->fill(event, buffer, len);
}

void ae_destroy(AudioEvent* event) {
  event->interface->destroy(event);
  free(event);
}

void ae_state_destroy(AudioEvent* event) {
  free(event->state);
}

typedef struct {
  int ttl;
  int freq;
  uint16_t phase;
} AEFreqState;

char ae_freq_fill(AudioEvent* event, sample_t* buffer, size_t len) {
  AEFreqState* state = (AEFreqState*)event->state;
  char alive = true;
  for(int i = 0; i < len; ++i) {
    if (state->ttl) {
      state->phase += 65536 * state->freq / 48000;
      buffer[i] = gen_pulse(state->phase);
      --state->ttl;
    }
    else {
      alive = false;
      buffer[i] = SAMPLE_0;
    }
  }
  return alive;
}

const AudioEventInterface AE_FREQ_INTERFACE = {
  ae_freq_fill,
  ae_state_destroy
};

AudioEvent* ae_freq_create(int freq) {
  AudioEvent* event = malloc(sizeof(AudioEvent));
  
  AEFreqState* state = malloc(sizeof(AEFreqState));
  state->ttl = 2000;
  state->freq = freq;
  state->phase = 0;
  
  event->interface = &AE_FREQ_INTERFACE;
  event->state = state;
  return event;
}

typedef struct {
  size_t hold;
  AudioEvent* event;
} AEHoldState;

char ae_hold_fill(AudioEvent* event, sample_t* buffer, size_t len) {
  AEHoldState* state = (AEHoldState*)event->state;
  if (state->hold == 0) {
    return ae_fill(state->event, buffer, len);
  }
  else if (state->hold < len) {
    for(int i = 0; i < state->hold; ++i)
      buffer[i] = SAMPLE_0;
    buffer += state->hold;
    len -= state->hold;
    state->hold = 0;
    return ae_fill(state->event, buffer, len);
  }
  else {
    state->hold -= len;
    for(int i = 0; i < len; ++i)
      buffer[i] = SAMPLE_0;
  }
}

void ae_hold_destroy(AudioEvent* event) {
  AEHoldState* state = (AEHoldState*)event->state;
  ae_destroy(state->event);
  free(state);
}

const AudioEventInterface AE_HOLD_INTERFACE = {
  ae_hold_fill,
  ae_hold_destroy
};

AudioEvent* ae_hold_create(size_t hold, AudioEvent* event) {
  AudioEvent* newEvent = malloc(sizeof(AudioEvent));
  AEHoldState* state = malloc(sizeof(AEHoldState));
  state->hold = hold;
  state->event = event;
  
  newEvent->interface = &AE_HOLD_INTERFACE;
  newEvent->state = state;
  return newEvent;
}

