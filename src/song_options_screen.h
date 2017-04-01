#ifndef SONG_OPTIONS_SCREEN_H
#define SONG_OPTIONS_SCREEN_H

#include "song.h"
#include "tracker.h"

typedef struct {
  char finished;
  Song* song;
  int row;
  Tracker* tracker;
} SongOptionsScreen;

void song_options_screen(SongOptionsScreen* screen);

#endif
