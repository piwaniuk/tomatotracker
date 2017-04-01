#ifndef PATTERN_SCREEN_H
#define PATTERN_SCREEN_H

#include "song.h"
#include "pattern.h"
#include "instrument.h"
#include "audio_output.h"
#include "tracker.h"

typedef struct {
  char finished;
  Song* song;
  Pattern* pattern;
  int row;
  int col;
  int octave;
  Instrument* lastInstrument;
  Tracker* tracker;
} PatternScreen;

void pattern_screen(PatternScreen* screen);

#endif
