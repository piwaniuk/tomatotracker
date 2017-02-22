#ifndef TRACKER_H
#define TRACKER_H

#include "sequencer.h"
#include "audio_output.h"

typedef struct {
  Sequencer sequencer;
  AudioOutputContext* aoc;
} Tracker;

#endif
