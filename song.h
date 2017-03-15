#ifndef SONG_H
#define SONG_H

#include <stdint.h>
#include <stdbool.h>

#include "list.h"
#include "pattern.h"
#include "instrument.h"

#define TRACK_COUNT 5
#define TRACK_SIZE 999

typedef struct{
  char name[7];
  char descr[32];
  Pattern** patterns;
} Phrase;


Phrase* phrase_create(char* name);
void phrase_destroy(Phrase* phrase);
char* phrase_repr(void* v);

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

void song_add_phrase(Song* song, Phrase* phrase);
bool song_has_phrase(Song* song, char* name);
BidirectionalIterator* song_phrases(Song* song);

void song_add_pattern(Song* song, Pattern* pattern);
bool song_has_pattern(Song* song, char* name);
BidirectionalIterator* song_patterns(Song* song);

void song_add_instrument(Song* song, Instrument* instrument);
bool song_has_instrument(Song* song, char* name);
BidirectionalIterator* song_instruments(Song* song);
Instrument* song_first_instrument(Song* song);

#endif
