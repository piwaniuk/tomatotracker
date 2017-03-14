#include <stdint.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "widgets.h"
#include "song_screen.h"
#include "audio_event.h"
#include "audio_output.h"
#include "sequencer.h"
#include "sbuffer.h"
#include "phrase_screen.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define MAX_PAT_ROW 18
#define MAX_PAT_COL 3
static const int SONG_COL_POS[5] = {4, 11, 18, 25, 32};

static ScreenPos song_screen_current_pos(SongScreen* screen) {
  return (ScreenPos){screen->row + 4, SONG_COL_POS[screen->col]};
}

static Phrase* song_screen_get_phrase(SongScreen* screen) {
  return screen->song->tracks[screen->col][screen->row].phrase;
}

static void song_screen_set_phrase(SongScreen* screen, Phrase* phrase) {
  screen->song->tracks[screen->col][screen->row].phrase = phrase;
  // last phrase is always updated, but shouldn't be NULL
  if (phrase != NULL)
    screen->lastPhrase = phrase;
}

static void render_song_screen(SongScreen* screen) {
  move(0, 0);
  print_header();
  printw("Tracks:\n");
  printw("    track1 track2 track3 track4 track5\n");
  for(int i = 0; i < 19; ++i) {
    SBuffer line = sbuffer_init(81);
    sbuffer_printf(&line, "%.3d%c", i + 1, ' ');
    for(int j = 0; j < TRACK_COUNT; ++j) {
      TrackEntry tmp = {NULL, 0};
      TrackEntry* pos = &screen->song->tracks[j][i];
      char* phraseName;
      if(pos->phrase != NULL)
        phraseName = pos->phrase->name;
      else if (pos->steps > 1)
        phraseName = "|    |";
      else if (pos->steps == 1)
        phraseName = "|____|";
      else
        phraseName = "......";
      
      sbuffer_printf(&line, "%-6s", phraseName);
      sbuffer_append_char(&line, ' ');
    }
    sbuffer_append_char(&line, '\n');
    printw(sbuffer_get(&line));
    sbuffer_free(&line);
  }
  move_screen_pos(song_screen_current_pos(screen));
  refresh();
}

static void new_phrase_command(SongScreen* screen) {
  char phraseName[7] = "";
  Phrase* newPhrase;
  bool hasName;
  bool editing = true;
  status_message("Enter a new phrase name");
  while (editing) {
    hasName = slug_edit_widget(song_screen_current_pos(screen), phraseName, 6);
    if (hasName) {
      if (song_has_phrase(screen->song, phraseName))
        status_message("Phrase name already exists");
      else {
        editing = false;
      }
    }
    else
      editing = false;
  }
  if (hasName) {
    newPhrase = phrase_create(phraseName);
    song_add_phrase(screen->song, newPhrase);
    song_screen_set_phrase(screen, newPhrase);
    status_message("Created a new phrase");
  }
}

static void choose_phrase_command(SongScreen* screen) {
  BidirectionalIterator* iter = song_phrases(screen->song);
  //only if there is any phrase
  if (!iter_is_end(iter)) {
    // navigate to current phrase or last used phrase
    if (song_screen_get_phrase(screen) != NULL)
      iter_find_forward(iter, song_screen_get_phrase(screen));
    else 
      iter_find_forward(iter, screen->lastPhrase);
    // run widget
    void* choice = list_choice_widget(iter, phrase_repr);
    // update song
    if (choice != NULL) {
      song_screen_set_phrase(screen, (Phrase*)choice);
    }
  }
  iter_destroy(iter);
}

static void last_phrase_command(SongScreen* screen) {
  if (screen->lastPhrase)
    song_screen_set_phrase(screen, screen->lastPhrase);
}

static void clear_phrase_command(SongScreen* screen) {
  song_screen_set_phrase(screen, NULL);
}

static void edit_phrase_command(SongScreen* screen) {
  Phrase* phrase = song_screen_get_phrase(screen);
  if (phrase != NULL) {
    PhraseScreen phraseScreen = {
      false,
      screen->song, song_screen_get_phrase(screen),
      0, 0,
      NULL,
      screen->tracker
    };
    phrase_screen(&phraseScreen);
  }
  else
    status_message("No phrase to edit here.");
}

static char position_commands(SongScreen* screen, int ch) {
  switch (ch) {
    case 'n':
      new_phrase_command(screen);
      break;
    case '\n':
      choose_phrase_command(screen);
      break;
    case ' ':
      last_phrase_command(screen);
      break;
    case '.':
      clear_phrase_command(screen);
      break;
    case 'e':
      edit_phrase_command(screen);
      break;
    default:
      return false;
  }
  return true;
}

static char general_commands(SongScreen* screen, int ch) {
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
      screen->col = min(4, screen->col + 1);
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

void song_screen(SongScreen* screen) {
  int ch;
  // main loop
  while (!screen->finished) {
    char noCommand = TRUE;
    render_song_screen(screen);
    // process a command before redrawing
    while (noCommand) {
      ch = getch();
      noCommand = !position_commands(screen, ch);
      if (noCommand)
        noCommand = !general_commands(screen, ch);
    }
  }
}

