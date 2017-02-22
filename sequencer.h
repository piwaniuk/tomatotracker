#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <stdlib.h>

#include "iterator.h"
#include "pattern.h"
#include "audio_event.h"

typedef enum {PLAY_MODE_SONG, PLAY_MODE_CHAIN, PLAY_MODE_PATTERN} PlayMode;

typedef struct {
  void* song;
  void* chain;
  Pattern* pattern;
  
  int bpm;
  int spt; // = (SR * 60) / (4 * BPM)
  
  PlayMode mode;
  int tick;
  int sample;
} Sequencer;

AudioEvent* step_to_event(PatternStep* step);

void seq_forward(Sequencer* seq, size_t len, BidirectionalIterator* events);

void seq_play_pattern(Sequencer* seq, Pattern* pattern);

#endif
