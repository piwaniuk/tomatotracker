#include "string.h"
#include "ncurses.h"

#include "widgets.h"
#include "instrument_screen.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

static const char* SHAPE_NAMES[] = {
  "sine",
  "triangle",
  "sawtooth",
  "square",
  "pulse",
  "noise",
  "carrot", // pow(triangle, 2)
  "plum", // pow(triangle, 0.5)
};

static HeaderFields make_header_fields(InstrumentScreen* screen) {
  return (HeaderFields){
    screen->song->title,
    seq_is_playing(&screen->tracker->sequencer),
    seq_play_mode(&screen->tracker->sequencer)
  };
}

static ScreenPos value_screen_pos(InstrumentScreen* screen) {
  return (ScreenPos){screen->row + 3, 10};
}

static void render_instrument_screen(InstrumentScreen* screen) {
  move(0, 0);
  print_header(make_header_fields(screen));
  char* labels[11] = {
    "name", "descr.", "volume", "type", "shape", "amp. A",
    "amp. D", "amp. S", "amp. R", "cutoff", "reso"
  };
  char values[11][40];
  strcpy(values[0], screen->instrument->identifier);
  strcpy(values[1], screen->instrument->description);
  sprintf(values[2], "%d", screen->instrument->volume);
  strcpy(values[3], "1OSC");


  Parameters1Osc* params = (Parameters1Osc*)screen->instrument->parameters;
  sprintf(values[4], "%s", SHAPE_NAMES[params->shape]);
  sprintf(values[5], "%d", params->ampAtt);
  sprintf(values[6], "%d", params->ampDec);
  sprintf(values[7], "%d", params->ampSus);
  sprintf(values[8], "%d", params->ampRel);
  sprintf(values[9], "%d", params->filter);
  sprintf(values[10], "%d", params->res);
  
  printw("Instrument:\n");
  for(int i = 0; i < 11; ++i) {
    printw("%-8s: %s\n", labels[i], values[i]);
  }
  for(int i = 9; i < 20; ++i)
    printw("\n");
  move(screen->row + 3, 0);
}

static bool shape_row_commands(InstrumentScreen* screen, int ch) {
  Parameters1Osc* params = (Parameters1Osc*)screen->instrument->parameters;
  int value = (int)params->shape;
  bool ret = enum_value_commands(&value, 0, LAST_SHAPE, ch);
  params->shape = (uint8_t)value;
  return ret;
}

static bool attack_row_commands(InstrumentScreen* screen, int ch) {
  Parameters1Osc* params = (Parameters1Osc*)screen->instrument->parameters;
  int value = (int)params->ampAtt;
  bool ret = numeric_value_commands(&value, 0, 65535, ch);
  params->ampAtt = (uint16_t)value;
  return ret;
}

static bool decay_row_commands(InstrumentScreen* screen, int ch) {
  Parameters1Osc* params = (Parameters1Osc*)screen->instrument->parameters;
  int value = (int)params->ampDec;
  bool ret = numeric_value_commands(&value, 0, 65535, ch);
  params->ampDec = (uint16_t)value;
  return ret;
}

static bool sustain_row_commands(InstrumentScreen* screen, int ch) {
  Parameters1Osc* params = (Parameters1Osc*)screen->instrument->parameters;
  int value = (int)params->ampSus;
  bool ret = numeric_value_commands(&value, 0, 65535, ch);
  params->ampSus = (uint16_t)value;
  return ret;
}

static bool release_row_commands(InstrumentScreen* screen, int ch) {
  Parameters1Osc* params = (Parameters1Osc*)screen->instrument->parameters;
  int value = (int)params->ampRel;
  bool ret = numeric_value_commands(&value, 0, 65535, ch);
  params->ampRel = (uint16_t)value;
  return ret;
}

static bool filter_row_commands(InstrumentScreen* screen, int ch) {
  Parameters1Osc* params = (Parameters1Osc*)screen->instrument->parameters;
  int value = (int)params->filter;
  bool ret = numeric_value_commands(&value, 20, 20000, ch);
  params->filter = (uint16_t)value;
  return ret;
}

static bool reso_row_commands(InstrumentScreen* screen, int ch) {
  Parameters1Osc* params = (Parameters1Osc*)screen->instrument->parameters;
  int value = (int)params->res;
  bool ret = numeric_value_commands(&value, 0, 20, ch);
  params->res = (uint16_t)value;
  return ret;
}

static bool type_1osc_row_commands(InstrumentScreen* screen, int ch) {
  switch (screen->row - 4) {
    case 0:
      return shape_row_commands(screen, ch);
    case 1:
      return attack_row_commands(screen, ch);
    case 2:
      return decay_row_commands(screen, ch);
    case 3:
      return sustain_row_commands(screen, ch);
    case 4:
      return release_row_commands(screen, ch);
    case 5:
      return filter_row_commands(screen, ch);
    case 6:
      return reso_row_commands(screen, ch);
    default:
      return false;
  }
}

static bool type_row_commands(InstrumentScreen* screen, int ch) {
  switch (screen->instrument->type) {
    case INSTRUMENT_TYPE_1OSC:
      return type_1osc_row_commands(screen, ch);
    default:
      return false;
  }
}

static bool name_row_commands(InstrumentScreen* screen, int ch) {
  if (ch == '\n') {
      char name[7] = "";
      ScreenPos widgetPos = value_screen_pos(screen);
      bool hasName;
      bool editing = true;
  
      status_message("Enter a new name");
      while (editing) {
        hasName = slug_edit_widget(widgetPos, name, 6);
        if (hasName) {
          if (song_has_instrument(screen->song, name))
            status_message("Name already in use");
          else
            editing = false;
        }
        else
          editing = false;
      }
      if (hasName) {
        song_rename_instrument(screen->song, screen->instrument, name);
        status_message("Renamed");
      }
    return true;
  }
  else
    return false;
}

static bool description_row_commands(InstrumentScreen* screen, int ch) {
  if (ch == '\n') {
    char newDescr[32] = "";
    bool hasDescr;
    ScreenPos widgetPos = value_screen_pos(screen);
    status_message("Enter a new description");

    hasDescr = text_edit_widget(widgetPos, newDescr, 31);

    if (hasDescr)
      strcpy(screen->instrument->description, newDescr);

    return true;
  }
  else
    return false;
}

static bool volume_row_commands(InstrumentScreen* screen, int ch) {
  int value = (int)screen->instrument->volume;
  bool ret = numeric_value_commands(&value, 0, 128, ch);
  screen->instrument->volume = (uint8_t)value;
  return ret;
}

static bool choose_type_row_commands(InstrumentScreen* screen, int ch) {
  if (ch == '\n') {
    status_message("Only 1OSC is available");
    return true;
  }
  else
    return false;
}

static bool common_row_commands(InstrumentScreen* screen, int ch) {
  switch (screen->row) {
    case 0:
      return name_row_commands(screen, ch);
    case 1:
      return description_row_commands(screen, ch);
    case 2:
      return volume_row_commands(screen, ch);
    case 3:
      return choose_type_row_commands(screen, ch);
    default:
      return type_row_commands(screen, ch);
  }
}

static int instrument_screen_rows(InstrumentScreen* screen) {
  switch( screen->instrument->type) {
    case INSTRUMENT_TYPE_1OSC:
      return 11;
    default:
      return 4;
  }
}

static bool general_commands(InstrumentScreen* screen, int ch) {
  switch (ch) {
    case KEY_DOWN:
      screen->row = min(instrument_screen_rows(screen) - 1, screen->row + 1);
      break;
    case KEY_UP:
      screen->row = max(0, screen->row - 1);
      break;
    case 'q':
      screen->finished = true;
      break;
    default:
      return false;
  }
  return true;
}

void instrument_screen(InstrumentScreen* screen) {
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
