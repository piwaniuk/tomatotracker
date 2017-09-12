#include <ncurses.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "tracker.h"
#include "song_writer.h"
#include "song_loader.h"
#include "song_reader_current.h"

extern "C" {
#include "instrument.h"
#include "pattern.h"
#include "song_options_screen.h"
#include "audio_output.h"
#include "sequencer.h"
}

bool tracker_save_song(Tracker* tracker, Song* song) {
  SongWriter writer(tracker->songFilename);
  writer.saveSong(song);
  writer.close();
  return true;
}

void print_usage() {
  std::cerr << "Usage:" << std::endl;
  std::cerr << "    tracker FILENAME" << std::endl;
}

Song* initialize_song(Tracker* tracker, int argc, char** argv) {
  if (argc != 2) {
    print_usage();
    exit(1);
  }
  tracker->songFilename = argv[1];
  std::ifstream file(tracker->songFilename, std::ios_base::binary);
  Song* ret;
  if (!file.good()) {
    file.close();
    // TODO: optimistically assuming it's a new song
    ret = song_create();
  }
  else {
    // loading existing song
    SongLoader loader(std::move(file));
    ret = loader.loadSong();
    loader.close();
    if (ret == nullptr) {
      std::cerr << "Failed to load song." << std::endl;
      exit(4);
    }
  }
  return ret;
}

void initialize_ui() {
  initscr();
  if (getenv("ESCDELAY") == NULL) set_escdelay(25);
  timeout(30);
  noecho();
  keypad(stdscr, TRUE);
}

void initialize_audio(Tracker* tracker) {
  tracker->sequencer =
    (Sequencer){NULL, 6000, 16, PLAY_MODE_PATTERN, false, 0, 0, 0};
  tracker->aoc = aoc_create(&tracker->sequencer);
  audio_output_initialize(tracker->aoc);
}

int main(int argc, char** argv) {
  Tracker tracker;
  Song* song = initialize_song(&tracker, argc, argv);

  initialize_ui();
  initialize_audio(&tracker);

  SongOptionsScreen screen = {false, song, 0, &tracker};
  song_options_screen(&screen);
  
  audio_output_finalize();
  aoc_destroy(tracker.aoc);

  endwin();

  song_destroy(song);

  return 0;
}
