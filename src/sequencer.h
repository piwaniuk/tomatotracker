#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <stdlib.h>
#include <stdbool.h>

#include "iterator.h"
#include "pattern.h"
#include "audio_event.h"
#include "song.h"

typedef enum {PLAY_MODE_SONG, PLAY_MODE_PHRASE, PLAY_MODE_PATTERN} PlayMode;

typedef struct {
  void* data;
  
  int bpm;
  int spt; // = (SR * 60) / (4 * BPM)
  
  PlayMode mode;
  char isPlaying;
  int patternStep;
  int tick;
  int sample;
} Sequencer;

AudioEvent* step_to_event(PatternStep* step);

/**
 * Move forward the sequencer a number of samples.
 */
void seq_forward(Sequencer* seq, size_t len, BidirectionalIterator* events);

void seq_play_pattern(Sequencer* seq, Pattern* pattern, size_t pos);
void seq_play_phrase(Sequencer* seq, Phrase* phrase, size_t pos);
void seq_play_song(Sequencer* seq, Song* song, size_t pos);

bool seq_pattern_mark(Sequencer* seq, Pattern* pattern, size_t pos);

bool seq_is_playing(Sequencer* seq);
uint8_t seq_play_mode(Sequencer* seq);
void seq_stop(Sequencer* seq);

#endif
