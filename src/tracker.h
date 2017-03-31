#ifndef TRACKER_H
#define TRACKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sequencer.h"
#include "audio_output.h"
#include "song.h"

typedef struct {
  Sequencer sequencer;
  AudioOutputContext* aoc;
  char* songFilename;
} Tracker;

bool tracker_save_song(Tracker* tracker, Song* song);

#ifdef __cplusplus
}
#endif

#endif
