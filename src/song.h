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
  uint16_t length;
  Pattern** patterns;
} Phrase;


Phrase* phrase_create(const char* name);
void phrase_destroy(Phrase* phrase);

/**
 * Set pattern at given position. Update length if necessary
 */
void phrase_set_pattern(Phrase* phrase, Pattern* pattern, uint16_t pos);
char* phrase_repr(void* v);

typedef struct {
  Phrase* phrase;
  uint16_t tail;
} TrackEntry;

typedef struct {
  char title[40];
  int tempo;
  LinkedList* instruments;
  LinkedList* phrases;
  LinkedList* patterns;
  TrackEntry** tracks;
  uint16_t length;
  uint16_t* lastPos;
} Song;

Song* song_create(void);
void song_destroy(Song* song);

void song_add_phrase(Song* song, Phrase* phrase);
bool song_has_phrase(Song* song, const char* name);
void song_set_phrase(Song* song, Phrase* phrase, uint8_t trackN, uint16_t pos);
void song_phrase_edited(Song* song, Phrase* phrase);
BidirectionalIterator* song_phrases(Song* song);

void song_add_pattern(Song* song, Pattern* pattern);
bool song_has_pattern(Song* song, const char* name);
BidirectionalIterator* song_patterns(Song* song);

void song_add_instrument(Song* song, Instrument* instrument);
bool song_has_instrument(Song* song, const char* name);
BidirectionalIterator* song_instruments(Song* song);
Instrument* song_first_instrument(Song* song);

#endif
