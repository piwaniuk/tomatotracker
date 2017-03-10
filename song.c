#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ordered_list.h"
#include "song.h"

#define DEFAULT_SONG_TITLE "New Song"
#define DEFAULT_TEMPO 120

Phrase* phrase_create(char* name) {
  Phrase* phrase = malloc(sizeof(Phrase));
  snprintf(phrase->name, 7, "%s", name);
  phrase->descr[0] = '\0';
  phrase->patterns = NULL;
  return phrase;
}

void phrase_destroy(Phrase* phrase) {
  free(phrase);
}

char phrase_cmp_name(void* v1, void* v2) {
  Phrase* p1 = (Phrase*)v1;
  Phrase* p2 = (Phrase*)v2;
  return strcmp(p1->name, p2->name);
}

Song* song_create(void) {
  Song* song = malloc(sizeof(Song));
  strcpy(song->title, DEFAULT_SONG_TITLE);
  song->tempo = DEFAULT_TEMPO;
  song->instruments = list_create();
  song->phrases = list_create();
  song->patterns = list_create();
  song->tracks = malloc(sizeof(TrackEntry*) * TRACK_COUNT);
  for(int i = 0; i < TRACK_COUNT; ++i) {
    song->tracks[i] = malloc(sizeof(TrackEntry) * TRACK_SIZE);
    for(int j = 0; j < TRACK_SIZE; ++j)
      song->tracks[i][j] = (TrackEntry){NULL, 0};
  }
  return song;
}

void song_destroy(Song* song) {
  list_destroy(song->instruments);
  list_destroy(song->phrases);
  list_destroy(song->patterns);
  
  for(int i = 0; i < TRACK_COUNT; ++i)
    free(song->tracks[i]);
  free(song->tracks);
  
  free(song);
}

void song_add_phrase(Song* song, Phrase* phrase) {
  ordered_list_insert_unique(song->phrases, phrase, phrase_cmp_name);
}

bool song_has_phrase(Song* song, char* name) {
  int cmp = -1;
  BidirectionalIterator* iter = list_iterator(song->phrases);
  while (!iter_is_end(iter) && cmp < 0) {
    Phrase* phrase = (Phrase*)iter_get(iter);
    cmp = strcmp(phrase->name, name);
    iter_next(iter);
  }
  iter_destroy(iter);
  return cmp == 0;
}
