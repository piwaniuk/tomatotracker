#ifndef AUDIO_EVENT_H
#define AUDIO_EVENT_H

#include "audio_output.h"

struct AudioEvent;
typedef struct AudioEvent AudioEvent;

AudioEvent* audio_event_create(int freq);
char audio_event_fill(AudioEvent* event, sample_t* buffer, size_t len);
void audio_event_destroy(AudioEvent* event);

#endif
