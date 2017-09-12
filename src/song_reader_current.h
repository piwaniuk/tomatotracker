#ifndef SONG_READER_CURRENT_H
#define SONG_READER_CURRENT_H

#include <string>
#include <fstream>
#include <memory>

#include "song_reader.h"
#include "song_io.h"

extern "C" {
#include "song.h"
}

template<>
class SongReader<HEADER_CURRENT_VERSION> : public SongReaderI {
public:
  SongReader(const char* filename);
  SongReader(std::ifstream&& file);
  SongReader(std::ifstream&& file, std::unique_ptr<SongHeader> header);
  Song* loadSong();
};

typedef SongReader<HEADER_CURRENT_VERSION> SongReaderCurrent;

#endif

