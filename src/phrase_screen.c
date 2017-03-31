#include <ncurses.h>

#include "phrase_screen.h"
#include "pattern_screen.h"
#include "widgets.h"
#include "sbuffer.h"
#include "tracker_field.h"
#include "pattern_field.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

static const int PHRASE_COL_POS[5] = {4, 11, 18, 25, 32};

static ScreenPos phrase_screen_current_pos(PhraseScreen* screen) {
  return (ScreenPos){screen->row + 4, PHRASE_COL_POS[screen->col]};
}

void render_phrase_screen(PhraseScreen* screen) {
  move(0, 0);
  print_header();
  printw("Phrase:\n");
  printw("    patt.\n");
  for(int i = 0; i < 19; ++i) {
    SBuffer line = sbuffer_init(81);
    char* patternName;
    Pattern* pattern = screen->phrase->patterns[i];
    if (pattern != NULL)
      patternName = pattern_repr(pattern);
    else
      patternName = "......";

    sbuffer_printf(&line, "%.3d%c", i + 1, ' ');
    sbuffer_printf(&line, "%-6s", patternName);
    sbuffer_append_char(&line, '\n');
    printw(sbuffer_get(&line));
    sbuffer_free(&line);
  }
  move_screen_pos(phrase_screen_current_pos(screen));
  refresh();
}

static void command_toggle_play_this(PhraseScreen* screen) {
  if (seq_is_playing(&screen->tracker->sequencer))
    seq_stop(&screen->tracker->sequencer);
  else
    seq_play_phrase(&screen->tracker->sequencer, screen->phrase, screen->row);
}

static char general_commands(PhraseScreen* screen, int ch) {
  switch (ch) {
    case KEY_DOWN:
      screen->row = min(19 - 1, screen->row + 1);
      break;
    case KEY_UP:
      screen->row = max(0, screen->row - 1);
      break;
    case KEY_LEFT:
      screen->col = max(0, screen->col - 1);
      break;
    case KEY_RIGHT:
      screen->col = min(0, screen->col + 1);
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

void phrase_screen(PhraseScreen* screen) {
  int ch;
  // main loop
  while (!screen->finished) {
    char noCommand = true;
    render_phrase_screen(screen);
    // process a command before redrawing
    while (noCommand) {
      ch = getch();
      TrackerField* field = pattern_field_create(screen);
      noCommand = !tracker_field_commands(field, ch);
      tracker_field_destroy(field);
      noCommand = noCommand && !general_commands(screen, ch);
    }
  }
}
