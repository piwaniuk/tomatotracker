#ifndef SONG_H
#define SONG_H

#include <stdint.h>

#include "list.h"
#include "pattern.h"

#define TRACK_COUNT 5
#define TRACK_SIZE 999

typedef struct{
  char* name;
  char* descr;
  Pattern* patterns;
} Phrase;

typedef struct {
  Phrase* phrase;
  uint16_t steps;
} TrackEntry;

typedef struct {
  char title[40];
  int tempo;
  LinkedList* instruments;
  LinkedList* phrases;
  LinkedList* patterns;
  TrackEntry** tracks;
} Song;

Song* song_create(void);
void song_destroy(Song* song);

#endif
