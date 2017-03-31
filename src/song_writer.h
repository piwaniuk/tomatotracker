#ifndef SONG_WRITER_H
#define SONG_WRITER_H

#include <string>
#include <fstream>

extern "C" {
#include "song.h"
}

class SongWriter {
public:
  SongWriter(const char* filename);
  void saveSong(Song* song);
  void close();
private:
  std::string filename;
  std::ofstream file;
};

#endif
