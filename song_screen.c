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

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define MAX_PAT_ROW 18
#define MAX_PAT_COL 3
static const int SONG_COL_POS[5] = {4, 11, 18, 25, 32};

static void render_song_screen(SongScreen* screen) {
  move(0, 0);
  print_header();
  printw("Song P01:\n");
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
      
      sbuffer_append_string(&line, phraseName);
      sbuffer_append_char(&line, ' ');
    }
    sbuffer_append_char(&line, '\n');
    printw(sbuffer_get(&line));
    sbuffer_free(&line);
  }
  printw("...");
  move(screen->row + 4, SONG_COL_POS[screen->col]);
  refresh();
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
      screen->finished = TRUE;
      break;
    default:
      return FALSE;
  }
  return TRUE;
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
      noCommand = FALSE;
      noCommand = general_commands(screen, ch);
    }
  }
}

