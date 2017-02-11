#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "instrument.h"
#include "pattern.h"
#include "widgets.h"
#include "pattern_screen.h"

Instrument INSTRUMENT_1 = {
  "instr1",
  "default instument",
  0
};

Instrument INSTRUMENT_2 = {
  "instr2",
  "default instument",
  0
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
  initialize_instruments();

  initscr();
  if (getenv("ESCDELAY") == NULL) set_escdelay(25);
  timeout(50);
  noecho();
  keypad(stdscr, TRUE);
  
  PatternScreen screen = {FALSE, &PATTERN, 0, 0, &INSTRUMENT_1};
  pattern_screen(&screen);
  
  endwin();
  return 0;
}
