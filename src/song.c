#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ordered_list.h"
#include "song.h"

#define DEFAULT_SONG_TITLE "New Song"
#define DEFAULT_TEMPO 120
#define DEFAULT_TPB 4
#define DEFAULT_PATTERN_LENGTH 16


Phrase* phrase_create(const char* name) {
  Phrase* phrase = malloc(sizeof(Phrase));
  snprintf(phrase->name, 7, "%s", name);
  phrase->descr[0] = '\0';
  // phrase length is always at least 1
  phrase->length = 1;
  phrase->patterns = malloc(sizeof(Pattern*) * TRACK_SIZE);
  for(int i = 0; i < TRACK_SIZE; ++i)
    phrase->patterns[i] = NULL;
  return phrase;
}

void phrase_destroy(Phrase* phrase) {
  free(phrase->patterns);
  free(phrase);
}

void phrase_set_pattern(Phrase* phrase, Pattern* pattern, uint16_t pos) {
  phrase->patterns[pos] = pattern;

  // update phrase length
  if (pattern == NULL) {
    // decrease phrase length when pattern is cleared
    while (phrase->length > 1 && phrase->patterns[phrase->length - 1] == NULL)
      --phrase->length;
  }
  else if (pos + 1 > phrase->length) {
     phrase->length = pos + 1;
  }
}

char* phrase_repr(void* v) {
  Phrase* phrase = (Phrase*)v;
  return phrase->name;
}

int
 phrase_cmp_name(void* v1, void* v2) {
  Phrase* p1 = (Phrase*)v1;
  Phrase* p2 = (Phrase*)v2;
  return strcmp(p1->name, p2->name);
}

Song* song_create(void) {
  Song* song = malloc(sizeof(Song));
  strcpy(song->title, DEFAULT_SONG_TITLE);
  song->tempo = DEFAULT_TEMPO;
  song->tpb = DEFAULT_TPB;
  song->patternLength = DEFAULT_PATTERN_LENGTH;
  song->instruments = list_create();
  song_add_instrument(song, instrument_create_default());
  song->phrases = list_create();
  song->patterns = list_create();

  song->tracks = malloc(sizeof(TrackEntry*) * TRACK_COUNT);
  for(int i = 0; i < TRACK_COUNT; ++i) {
    song->tracks[i] = malloc(sizeof(TrackEntry) * TRACK_SIZE);
    for(int j = 0; j < TRACK_SIZE; ++j)
      song->tracks[i][j] = (TrackEntry){NULL, 0};
  }

  song->lastPos = malloc(sizeof(int32_t) * TRACK_COUNT);
  for(int i = 0; i < TRACK_COUNT; ++i)
    song->lastPos[i] = 0;
  song->length = 1;

  return song;
}

void song_destroy(Song* song) {
  //TODO: free contents first
  list_destroy(song->instruments);
  list_destroy(song->phrases);
  list_destroy(song->patterns);
  
  for(int i = 0; i < TRACK_COUNT; ++i)
    free(song->tracks[i]);

  free(song->tracks);
  free(song->lastPos);
  free(song);
}

void song_add_phrase(Song* song, Phrase* phrase) {
  ordered_list_insert_unique(song->phrases, phrase, phrase_cmp_name);
}

bool song_has_phrase(Song* song, const char* name) {
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

BidirectionalIterator* song_phrases(Song* song) {
  return list_iterator(song->phrases);
}

static uint16_t song_track_length(Song* song, uint8_t trackN) {
  uint16_t pos = song->lastPos[trackN];
  Phrase* phrase = song->tracks[trackN][pos].phrase;
  if (phrase == NULL)
    return 1;
  else
    return pos + phrase->length;
}

void song_update_length(Song* song) {
  song->length = 1;
  for(int i = 0; i < TRACK_COUNT; ++i) {
    if (song_track_length(song, i) > song->length)
      song->length = song_track_length(song, i);
  }
}

void song_set_phrase(Song* song, Phrase* phrase, uint8_t trackN, uint16_t pos) {
  // update track contents
  song->tracks[trackN][pos].phrase = phrase;

  // update last position for the track
  if (phrase == NULL && pos == song->lastPos[trackN]) {
    // clearing last phrase in track
    if (pos > 0) {
      song->lastPos[trackN] = (pos - 1) - song->tracks[trackN][pos - 1].tail;
    }
    song_update_length(song);
  }
  else if (phrase != NULL && pos >= song->lastPos[trackN]) {
    // adding or updating phrase at the end

    // update tail BEFORE pos
    uint16_t i = pos;
    while (song->tracks[trackN][i].tail == 0 && i > song->lastPos[trackN]) {
      song->tracks[trackN][i].tail = i - song->lastPos[trackN];
      --i;
    }

    // set new last phrase position
    song->lastPos[trackN] = pos;
    song_update_length(song);
  }

  // update tail AFTER pos
  if (phrase != NULL) {
    song->tracks[trackN][pos].tail = 0;
  }
  else if (pos > 0) {
    song->tracks[trackN][pos].tail = song->tracks[trackN][pos - 1].tail + 1;
  }
  ++pos;
  while (pos < song_track_length(song, trackN) && song->tracks[trackN][pos].phrase == NULL) {
    song->tracks[trackN][pos].tail = song->tracks[trackN][pos - 1].tail + 1;
    ++pos;
  }
}

void song_phrase_edited(Song* song, Phrase* phrase) {
  for(int i = 0; i < TRACK_COUNT; ++i)
    if (song->tracks[i][song->lastPos[i]].phrase == phrase)
      song_set_phrase(song, phrase, i, song->lastPos[i]);
}

void song_add_pattern(Song* song, Pattern* pattern) {
  ordered_list_insert_unique(song->patterns, pattern, pattern_cmp_name);
}

bool song_has_pattern(Song* song, const char* name) {
  int cmp = -1;
  BidirectionalIterator* iter = list_iterator(song->patterns);
  while (!iter_is_end(iter) && cmp < 0) {
    Pattern* pattern = (Pattern*)iter_get(iter);
    cmp = strcmp(pattern->identifier, name);
    iter_next(iter);
  }
  iter_destroy(iter);
  return cmp == 0;
}

BidirectionalIterator* song_patterns(Song* song) {
  return list_iterator(song->patterns);
}

void song_add_instrument(Song* song, Instrument* instrument) {
  ordered_list_insert_unique(
    song->instruments, instrument, instrument_cmp_name);
}

bool song_has_instrument(Song* song, const char* name) {
  int cmp = -1;
  BidirectionalIterator* iter = list_iterator(song->instruments);
  while (!iter_is_end(iter) && cmp < 0) {
    Instrument* instrument = (Instrument*)iter_get(iter);
    cmp = strcmp(instrument->identifier, name);
    iter_next(iter);
  }
  iter_destroy(iter);
  return cmp == 0;
}

BidirectionalIterator* song_instruments(Song* song) {
  return list_iterator(song->instruments);
}

Instrument* song_first_instrument(Song* song) {
  Instrument* ret;
  BidirectionalIterator* iter = song_instruments(song);
  ret = (Instrument*)iter_get(iter);
  iter_destroy(iter);
  return ret; 
}

uint32_t song_instrument_count(const Song* song) {
  return list_length(song->instruments);
}

uint32_t song_pattern_count(const Song* song) {
  return list_length(song->patterns);
}

uint32_t song_phrase_count(const Song* song) {
  return list_length(song->phrases);
}
