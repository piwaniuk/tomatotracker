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
#include "tracker_field.h"
#include "phrase_field.h"


#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

static const int SONG_COL_POS[5] = {4, 11, 18, 25, 32};

static ScreenPos song_screen_current_pos(SongScreen* screen) {
  return (ScreenPos){screen->row + 4, SONG_COL_POS[screen->col]};
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
      TrackEntry* pos = &screen->song->tracks[j][i];
      char* phraseName;
      if(pos->phrase != NULL)
        phraseName = pos->phrase->name;
      else {
        phraseName = "......";
        // check preceding phrase's tail
        if (i > 0) {
          int prec = (i - 1) - screen->song->tracks[j][i - 1].tail;
          Phrase* precPhrase = screen->song->tracks[j][prec].phrase;
          if (precPhrase != NULL) {
            int drawTail = precPhrase->length - (i - prec);
            if (drawTail > 1)
              phraseName = "|    |";
            else if (drawTail == 1)
              phraseName = "|___/ ";
          }
        }
      }
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

static void command_toggle_play_this(SongScreen* screen) {
  if (seq_is_playing(&screen->tracker->sequencer))
    seq_stop(&screen->tracker->sequencer);
  else
    seq_play_song(&screen->tracker->sequencer, screen->song, screen->row);
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
      command_toggle_play_this(screen);
      break;
    default:
      return false;
  }
  return true;
}

void command_quit(SongScreen* screen) {
  //if (widget_confirm("Type 'yes' to confirm quit: "))
  screen->finished = true;
}

void command_save_song(SongScreen* screen) {
  if (tracker_save_song(screen->tracker, screen->song))
    status_message("Song has been saved");
}

static bool song_commands(SongScreen* screen, int ch) {
  switch (ch) {
    case 'Q':
      command_quit(screen);
      break;
    case 'S':
      command_save_song(screen);
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
      TrackerField* field = phrase_field_create(screen);
      noCommand = !tracker_field_commands(field, ch);
      tracker_field_destroy(field);
      noCommand = noCommand && !general_commands(screen, ch);
      noCommand = noCommand && !song_commands(screen, ch);
    }
  }
}

