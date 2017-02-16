#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include "audio_event.h"
#include "list.h"
#include "sequencer.h"

typedef struct {
  LinkedList* events;
  Sequencer* seq; 
} AudioOutputContext;

AudioOutputContext* aoc_create(Sequencer* seq);
void aoc_destroy(AudioOutputContext* aoc);

void audio_add_event_freq(int freq);
void audio_output_initialize(Sequencer* seq);
void audio_output_finalize();

#endif
