#include "string.h"
#include "ncurses.h"

#include "widgets.h"
#include "song_options_screen.h"
#include "song_screen.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

static HeaderFields make_header_fields(SongOptionsScreen* screen) {
  return (HeaderFields){
    screen->song->title,
    seq_is_playing(&screen->tracker->sequencer),
    seq_play_mode(&screen->tracker->sequencer)
  };
}

static ScreenPos value_screen_pos(SongOptionsScreen* screen) {
  return (ScreenPos){screen->row + 3, 8};
}

static void render_instrument_screen(SongOptionsScreen* screen) {
  move(0, 0);
  print_header(make_header_fields(screen));
  char* labels[4] = {
    "title", "tempo", "TPB", "p. len."
  };
  char values[4][40];
  strcpy(values[0], screen->song->title);
  sprintf(values[1], "%d", screen->song->tempo);
  sprintf(values[2], "%d", screen->song->tpb);
  sprintf(values[3], "%d", screen->song->patternLength);
  
  printw("Song options:\n");
  for(int i = 0; i < 4; ++i) {
    printw("%-8s: %s\n", labels[i], values[i]);
  }
  move(screen->row + 3, 0);
}

static bool title_row_commands(SongOptionsScreen* screen, int ch) {
  if (ch == '\n') {
    char newTitle[40] = "";
    bool hasTitle;
    ScreenPos widgetPos = value_screen_pos(screen);
    status_message("Enter a title description");

    hasTitle = text_edit_widget(widgetPos, newTitle, 39);

    if (hasTitle)
      strcpy(screen->song->title, newTitle);

    return true;
  }
  else
    return false;
}

static bool tempo_row_commands(SongOptionsScreen* screen, int ch) {
  int value = (int)screen->song->tempo;
  bool ret = numeric_value_commands(&value, 15, 240, ch);
  screen->song->tempo = (uint8_t)value;
  return ret;
}

static bool tpb_row_commands(SongOptionsScreen* screen, int ch) {
  int value = (int)screen->song->tpb;
  bool ret = numeric_value_commands(&value, 1, 32, ch);
  screen->song->tpb = (uint8_t)value;
  return ret;
}

static bool pattern_length_row_commands(SongOptionsScreen* screen, int ch) {
  int value = (int)screen->song->patternLength;
  bool ret = numeric_value_commands(&value, 1, 19, ch);
  screen->song->patternLength = (uint8_t)value;
  return ret;
}

static bool common_row_commands(SongOptionsScreen* screen, int ch) {
  switch (screen->row) {
    case 0:
      return title_row_commands(screen, ch);
    case 1:
      return tempo_row_commands(screen, ch);
    case 2:
      return tpb_row_commands(screen, ch);
    case 3:
      return pattern_length_row_commands(screen, ch);
    default:
      return false;
  }
}

static int song_options_screen_rows(SongOptionsScreen* screen) {
  return 4;
}

static void command_quit(SongOptionsScreen* screen) {
  //if (widget_confirm("Type 'yes' to confirm quit: "))
  screen->finished = true;
}

static void command_save_song(SongOptionsScreen* screen) {
  if (tracker_save_song(screen->tracker, screen->song))
    status_message("Song has been saved");
}


static void command_edit(SongOptionsScreen* screen) {
  SongScreen nextScreen = {false, screen->song, 0, 0, NULL, screen->tracker};
  song_screen(&nextScreen);
}

static bool general_commands(SongOptionsScreen* screen, int ch) {
  switch (ch) {
    case KEY_DOWN:
      screen->row = min(song_options_screen_rows(screen) - 1, screen->row + 1);
      break;
    case KEY_UP:
      screen->row = max(0, screen->row - 1);
      break;
    case 'Q':
      command_quit(screen);
      break;
    case 'S':
      command_save_song(screen);
    case 'e':
      command_edit(screen);
      break;
    default:
      return false;
  }
  return true;
}

void song_options_screen(SongOptionsScreen* screen) {
  int ch;
  while (!screen->finished) {
    char noCommand = true;
    render_instrument_screen(screen);
    while (noCommand) {
      ch = getch();
      noCommand = noCommand && !common_row_commands(screen, ch);
      noCommand = noCommand && !general_commands(screen, ch);
    }
  }
}
