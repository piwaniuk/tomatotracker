#include <ncurses.h>

#include "phrase_screen.h"
#include "widgets.h"
#include "sbuffer.h"

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
    sbuffer_printf(&line, "%.3d%c", i + 1, ' ');
    sbuffer_append_string(&line, "......");
    sbuffer_append_char(&line, '\n');
    printw(sbuffer_get(&line));
    sbuffer_free(&line);
  }
  move_screen_pos(phrase_screen_current_pos(screen));
  refresh();
}

static char position_commands(PhraseScreen* screen, int ch) {
  switch (ch) {
    default:
      return false;
  }
  return true;
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
      //TODO: toggle play
      break;
    case 'Q':
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
      noCommand = !position_commands(screen, ch);
      if (noCommand)
        noCommand = !general_commands(screen, ch);
    }
  }
}
