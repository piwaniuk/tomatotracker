#include <ncurses.h>

#include "phrase_screen.h"
#include "pattern_screen.h"
#include "widgets.h"
#include "sbuffer.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

static const int PHRASE_COL_POS[5] = {4, 11, 18, 25, 32};

static ScreenPos phrase_screen_current_pos(PhraseScreen* screen) {
  return (ScreenPos){screen->row + 4, PHRASE_COL_POS[screen->col]};
}

static Pattern* phrase_screen_get_pattern(PhraseScreen* screen) {
  return screen->phrase->patterns[screen->row];
}

static void phrase_screen_set_pattern(PhraseScreen* screen, Pattern* pattern) {
  screen->phrase->patterns[screen->row] = pattern;
  // last phrase is always updated, but shouldn't be NULL
  if (pattern != NULL)
    screen->lastPattern = pattern;
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

static void new_pattern_command(PhraseScreen* screen) {
  char patternName[7] = "";
  bool hasName;
  bool editing = true;
  status_message("Enter a new pattern name");
  while (editing) {
    hasName = slug_edit_widget(phrase_screen_current_pos(screen), patternName, 6);
    if (hasName) {
      if (song_has_pattern(screen->song, patternName))
        status_message("Pattern name already exists");
      else {
        editing = false;
      }
    }
    else
      editing = false;
  }
  if (hasName) {
    Pattern* newPattern = pattern_create(patternName);
    song_add_pattern(screen->song, newPattern);
    phrase_screen_set_pattern(screen, newPattern);
    status_message("Created a new pattern");
  }
}

static void choose_pattern_command(PhraseScreen* screen) {
  BidirectionalIterator* iter = song_patterns(screen->song);
  //only if there is any phrase
  if (!iter_is_end(iter)) {
    // navigate to current phrase or last used phrase
    if (phrase_screen_get_pattern(screen) != NULL)
      iter_find_forward(iter, phrase_screen_get_pattern(screen));
    else
      iter_find_forward(iter, screen->lastPattern);
    // run widget
    void* choice = list_choice_widget(iter, pattern_repr);
    // update song
    if (choice != NULL) {
      phrase_screen_set_pattern(screen, (Pattern*)choice);
    }
  }
  iter_destroy(iter);
}

static void last_pattern_command(PhraseScreen* screen) {
  if (screen->lastPattern)
    phrase_screen_set_pattern(screen, screen->lastPattern);
}

static void clear_pattern_command(PhraseScreen* screen) {
  phrase_screen_set_pattern(screen, NULL);
}

static void edit_pattern_command(PhraseScreen* screen) {
  Pattern* pattern = phrase_screen_get_pattern(screen);
  if (pattern != NULL) {
    PatternScreen patternScreen = {
      false, screen->song, pattern, 0, 0, song_first_instrument(screen->song), screen->tracker
    };
    pattern_screen(&patternScreen);
  }
  else
    status_message("No pattern to edit here.");
}

static char position_commands(PhraseScreen* screen, int ch) {
  switch (ch) {
    case 'n':
      new_pattern_command(screen);
      break;
    case '\n':
      choose_pattern_command(screen);
      break;
    case ' ':
      last_pattern_command(screen);
      break;
    case '.':
      clear_pattern_command(screen);
      break;
    case 'e':
      edit_pattern_command(screen);
      break;
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
