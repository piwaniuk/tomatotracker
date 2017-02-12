#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "audio_event.h"

struct AudioEvent {
  int hold;
  int ttl;
  int freq;
  uint16_t phase;
};

static sample_t gen_triangle(uint16_t phase) {
  if (phase < 0x7FFF)
    return phase * 2;
  else 
    return -1 - (phase * 2);
}

AudioEvent* audio_event_create(int freq) {
  AudioEvent* event = malloc(sizeof(AudioEvent));
  event->hold = 0;
  event->ttl = 9600;
  event->freq = freq;
  event->phase = 0;
  return event;
}

char audio_event_fill(AudioEvent* event, sample_t* buffer, size_t len) {
  char alive = true;
  for(int i = 0; i < len; ++i) {
    if (event->hold) {
      buffer[i] = SAMPLE_0;
      --event->hold;
    }
    else if(event->ttl) {
      event->phase += 65536 * event->freq / 48000;
      buffer[i] = gen_triangle(event->phase);
      --event->ttl;
    }
    else {
      alive = false;
      buffer[i] = SAMPLE_0;
    }
  }
  return alive;
}

void audio_event_destroy(AudioEvent* event) {
  free(event);
}

