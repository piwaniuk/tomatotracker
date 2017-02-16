#ifndef PATTERN_SCREEN_H
#define PATTERN_SCREEN_H

#include "pattern.h"
#include "instrument.h"
#include "sequencer.h"

typedef struct {
  char finished;
  Pattern* pattern;
  int row;
  int col;
  Instrument* lastInstrument;
  Sequencer* seq;
} PatternScreen;

void pattern_screen(PatternScreen* screen);

#endif
