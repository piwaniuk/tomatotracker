#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include "audio_event.h"
#include "list.h"
#include "sequencer.h"
#include "pthread.h"

typedef struct {
  LinkedList* events;
  Sequencer* seq;
  pthread_mutex_t events_mutex;
} AudioOutputContext;

AudioOutputContext* aoc_create(Sequencer* seq);
void aoc_destroy(AudioOutputContext* aoc);
void aoc_add_event(AudioOutputContext* aoc, AudioEvent* event);

void audio_output_initialize(AudioOutputContext* aoc);
void audio_output_finalize();

#endif
