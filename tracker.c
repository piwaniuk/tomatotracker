#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "instrument.h"
#include "pattern.h"
#include "pattern_screen.h"
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


int main() {
  Sequencer seq = {NULL, NULL, NULL, 120, 6000, PLAY_MODE_PATTERN, 0, 0};
  initialize_instruments();
  audio_output_initialize(&seq);

  initscr();
  if (getenv("ESCDELAY") == NULL) set_escdelay(25);
  timeout(30);
  noecho();
  keypad(stdscr, TRUE);
  
  
  PatternScreen screen = {FALSE, &PATTERN, 0, 0, &INSTRUMENT_1, &seq};
  pattern_screen(&screen);
  
  endwin();
  audio_output_finalize();
  return 0;
}
