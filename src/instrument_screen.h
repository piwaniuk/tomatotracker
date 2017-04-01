#ifndef INSTRUMENT_SCREEN_H
#define INSTRUMENT_SCREEN_H

#include "song.h"
#include "instrument.h"
#include "tracker.h"

typedef struct {
  char finished;
  Song* song;
  Instrument* instrument;
  int row;
  Tracker* tracker;
} InstrumentScreen;

void instrument_screen(InstrumentScreen* screen);

#endif
