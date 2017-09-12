#ifndef SONG_LOADER_H
#define SONG_LOADER_H

#include <string>
#include <fstream>
#include <memory>

#include "song_io.h"

extern "C" {
#include "song.h"
}

class SongLoader {
public:
  //SongReader(const char* filename);
  SongLoader(std::ifstream&& file);
  //const SongHeader* getHeader();
  Song* loadSong();
  void close();
private:
  std::unique_ptr<SongHeader> loadHeader();
  std::string filename;
  std::ifstream file;
};

#endif

