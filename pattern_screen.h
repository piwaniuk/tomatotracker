#ifndef PATTERN_SCREEN_H
#define PATTERN_SCREEN_H

#include "pattern.h"
#include "instrument.h"
#include "audio_output.h"
#include "tracker.h"

typedef struct {
  char finished;
  Pattern* pattern;
  int row;
  int col;
  Instrument* lastInstrument;
  Tracker* tracker;
} PatternScreen;

void pattern_screen(PatternScreen* screen);

#endif
