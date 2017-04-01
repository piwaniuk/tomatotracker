#include <stdint.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "widgets.h"
#include "pattern_screen.h"
#include "audio_event.h"
#include "audio_output.h"
#include "sequencer.h"
#include "instrument_field.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define MAX_PAT_ROW 18
#define MAX_PAT_COL 2
const int PAT_COL_POS[4] = {4, 8, 15};

char NOTE_NAMES[12][3] = {
  "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};

void decode_note(uint8_t n, char note[4]) {
  if (n == 0)
    strcpy(note, "...");
  else {
    uint8_t m = n % 16;
    uint8_t o = n / 16;
    sprintf(note, "%s%d", NOTE_NAMES[m], o); 
  }
}

static HeaderFields make_header_fields(PatternScreen* screen) {
  return (HeaderFields){
    screen->song->title,
    seq_is_playing(&screen->tracker->sequencer),
    seq_play_mode(&screen->tracker->sequencer)
  };
}

void render_pattern_screen(PatternScreen* screen) {
  move(0, 0);
  print_header(make_header_fields(screen));
  printw("Pattern P01:\n");
  printw("    n   instr  len\n");
  for(int i = 0; i < screen->song->patternLength; ++i) {
    char line[40];
    char note[4];
    decode_note(screen->pattern->steps[i].n, note);
    char* instrument = "......";
    if (screen->pattern->steps[i].n)
      instrument = screen->pattern->steps[i].inst->identifier;
    char mark = ' ';
    if (seq_pattern_mark(&screen->tracker->sequencer, screen->pattern, i))
      mark = '>';
    int length = screen->pattern->steps[i].length;
    sprintf(line, "%.3d%c%s %-6s %.3d\n", i + 1, mark, note, instrument, length);
    printw(line);
  }
  for(int i = screen->song->patternLength; i < 19; ++i)
    printw("\n");
  move(screen->row + 4, PAT_COL_POS[screen->col]);
  refresh();
}

#define PIANO_KEYS_SIZE 14
int PIANO_KEYS[PIANO_KEYS_SIZE][2] = {
  {'z', 0}, {'s', 1}, {'x', 2}, {'d', 3}, {'c', 4}, {'v', 5},
  {'g', 6}, {'b', 7}, {'h', 8}, {'n', 9}, {'j', 10}, {'m', 11},
  {',', 16}, {'l', 17}
};

int char_to_piano(int ch) {
  for(int i = 0; i < PIANO_KEYS_SIZE; ++i)
    if (ch == PIANO_KEYS[i][0])
      return PIANO_KEYS[i][1];
  return -1;
}

char note_column_commands(PatternScreen* screen, int ch) {
  int pianoKey;
  if (ch == '.') {
    screen->pattern->steps[screen->row] = (PatternStep){0, 0, NULL, 0, 0};
    return TRUE;
  }
  else if ((pianoKey = char_to_piano(ch)) != -1) {
    uint8_t note = 64 + pianoKey;
    screen->pattern->steps[screen->row].n = note;
    if (screen->pattern->steps[screen->row].inst == NULL) {
      screen->pattern->steps[screen->row].inst = screen->lastInstrument;
      screen->pattern->steps[screen->row].length = 1;
    }
    aoc_add_event(screen->tracker->aoc, step_to_event(&screen->pattern->steps[screen->row], 1500));
    return TRUE;
  }
  else {
    return FALSE;
  }
}

static void command_toggle_play_this(PatternScreen* screen) {
  if (seq_is_playing(&screen->tracker->sequencer))
    seq_stop(&screen->tracker->sequencer);
  else
    seq_play_pattern(&screen->tracker->sequencer, screen->pattern, screen->row);
}

static char length_column_commands(PatternScreen* screen, int ch) {
  // no editing length in empty steps
  if (screen->pattern->steps[screen->row].n == 0)
    return false;
  switch (ch) {
    case ',':
      if (screen->pattern->steps[screen->row].length > 0)
        --screen->pattern->steps[screen->row].length;
      break;
    case '.':
      if (screen->pattern->steps[screen->row].length < 128)
        ++screen->pattern->steps[screen->row].length;
      break;
    case '<':
        screen->pattern->steps[screen->row].length /= 2;
      break;
    case '>':
      if (screen->pattern->steps[screen->row].length < 64)
        screen->pattern->steps[screen->row].length *= 2;
      else
        screen->pattern->steps[screen->row].length = 128;
      break;
    default:
      return false;
  }
  return true;
}

static char general_commands(PatternScreen* screen, int ch) {
  switch (ch) {
    case KEY_DOWN:
      screen->row = min(screen->song->patternLength - 1, screen->row + 1);
      break;
    case KEY_UP:
      screen->row = max(0, screen->row - 1);
      break;
    case KEY_LEFT:
      screen->col = max(0, screen->col - 1);
      break;
    case KEY_RIGHT:
      screen->col = min(MAX_PAT_COL, screen->col + 1);
      break;
    case 'p':
      command_toggle_play_this(screen);
      break;
    case 'q':
      screen->finished = true;
      break;
    default:
      return false;
  }
  return true;
}

void pattern_screen(PatternScreen* screen) {
  int ch;
  while (!screen->finished) {
    char noCommand = true;
    render_pattern_screen(screen);
    while (noCommand) {
      ch = getch();
      if (screen->col == 0) {
        noCommand = !note_column_commands(screen, ch);
      }
      else if (screen->col == 1) {
        TrackerField* field = instrument_field_create(screen);
        noCommand = !tracker_field_commands(field, ch);
        tracker_field_destroy(field);
      }
      else if (screen->col == 2) {
        noCommand = !length_column_commands(screen, ch);
      }
      noCommand = noCommand && !general_commands(screen, ch);
    }
  }
}

