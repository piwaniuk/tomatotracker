#include <stdlib.h>
#include <string.h>

#include "song.h"

#define DEFAULT_SONG_TITLE "New Song"
#define DEFAULT_TEMPO 120

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

