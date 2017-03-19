#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "tracker.h"
#include "instrument.h"
#include "pattern.h"
//#include "pattern_screen.h"
#include "song_screen.h"
#include "audio_output.h"
#include "sequencer.h"

Parameters1Osc PARAMS_1 = {
  0,
  65535,
  240,
  240,
  40000,
  800,
};

Instrument INSTRUMENT_1 = {
  "instr1",
  "default instument",
  INSTRUMENT_1OSC,
  &PARAMS_1
};

Parameters1Osc PARAMS_2 = {
  0,
  65535,
  720,
  240,
  52000,
  240
};

Instrument INSTRUMENT_2 = {
  "instr2",
  "default instument",
  INSTRUMENT_1OSC,
  &PARAMS_2
};

Pattern PATTERN = {
  "PAT001",
  "demo pattern",
  16,
  {}
};

void initialize_instruments() {  
  INSTRUMENTS = list_create();
  BidirectionalIterator* i = list_iterator(INSTRUMENTS);
  iter_insert(i, &INSTRUMENT_1);
  iter_insert(i, &INSTRUMENT_2);
  iter_destroy(i);
}

void initialize_audio(Tracker* tracker) {
  initialize_instruments();
  tracker->sequencer = (Sequencer){NULL, 120, 6000, PLAY_MODE_PATTERN, false, 0, 0};
  tracker->aoc = aoc_create(&tracker->sequencer);
  audio_output_initialize(tracker->aoc);
}


int main() {
  Tracker tracker;
  initialize_audio(&tracker);
  initscr();
  if (getenv("ESCDELAY") == NULL) set_escdelay(25);
  timeout(30);
  noecho();
  keypad(stdscr, TRUE);
  
  
  //PatternScreen screen = {FALSE, &PATTERN, 0, 0, &INSTRUMENT_1, &tracker};
  Song* song = song_create(); // TODO: place it somewhere
  SongScreen screen = {false, song, 0, 0, NULL, &tracker};
  song_screen(&screen);
  song_destroy(song);
  
  endwin();
  audio_output_finalize();
  aoc_destroy(tracker.aoc);
  return 0;
}
