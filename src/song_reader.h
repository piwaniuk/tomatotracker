#ifndef SONG_READER_H
#define SONG_READER_H

#include <string>
#include <fstream>
#include <memory>

#include "song_io.h"

extern "C" {
#include "song.h"
}

class SongReaderI {
public:
  SongReaderI(const char* filename);
  SongReaderI(std::ifstream&& file);
  SongReaderI(std::ifstream&& file, std::unique_ptr<SongHeader> header);
  const SongHeader* getHeader();
  Song* loadSong();
  void close();
protected:
  std::string filename;
  std::ifstream file;
private:
  void loadHeader();
  std::unique_ptr<SongHeader> header;
  bool headerLoaded;
};

template<int VERSION> class SongReader;

#endif

