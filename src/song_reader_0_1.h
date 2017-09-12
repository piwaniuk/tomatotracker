/**
 * Support for loading songs in format version 0.1
 */

#ifndef SONG_READER_0_1_H
#define SONG_READER_0_1_H

#include <string>
#include <fstream>
#include <memory>

#include "song_reader.h"
#include "song_io.h"

extern "C" {
#include "song.h"
}

template<>
class SongReader<0x00000001> : public SongReaderI {
public:
  SongReader(const char* filename);
  SongReader(std::ifstream&& file);
  SongReader(std::ifstream&& file, std::unique_ptr<SongHeader> header);
  Song* loadSong();
};

#endif

