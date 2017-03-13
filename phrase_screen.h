#ifndef PHRASE_SCREEN_H
#define PHRASE_SCREEN_H

#include "song.h"
#include "pattern.h"
#include "tracker.h"

typedef struct {
  char finished;
  Phrase* phrase;
  int row;
  int col;
  Pattern* lastPattern;
  Tracker* tracker;
} PhraseScreen;

void phrase_screen(PhraseScreen* screen);

#endif
