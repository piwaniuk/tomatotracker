#ifndef SONG_READER_H
#define SONG_READER_H

#include <string>
#include <fstream>
#include <memory>

#include "song_io.h"

extern "C" {
#include "song.h"
}

class SongReader {
public:
  SongReader(const char* filename);
  SongReader(std::ifstream&& file);
  const SongHeader* getHeader();
  Song* loadSong();
  void close();
private:
  void loadHeader();
  std::string filename;
  std::ifstream file;
  std::unique_ptr<SongHeader> header;
  bool headerLoaded;
};

#endif

