#ifndef PATTERN_SCREEN_H
#define PATTERN_SCREEN_H

#include "song.h"
#include "tracker.h"

/**
 * Structure describing state of song screen
 */
typedef struct {
  char finished;
  Song* song;
  int row; // cursor position
  int col;
  Phrase* lastPhrase; // last entered phrase
  Tracker* tracker;
} SongScreen;

/**
 * Display song screen and process events
 */
void song_screen(SongScreen* screen);

#endif
