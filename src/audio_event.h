#ifndef AUDIO_EVENT_H
#define AUDIO_EVENT_H

#include <stdlib.h>

#include "synth.h"

struct AudioEvent;
typedef struct AudioEvent AudioEvent;

AudioEvent* ae_freq_create(int freq);
AudioEvent* ae_hold_create(size_t hold, AudioEvent* event);

char ae_fill(AudioEvent* event, float* buffer, size_t len);
void ae_destroy(AudioEvent* event);

#endif
