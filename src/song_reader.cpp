#include "song_io.h"
#include "song_reader.h"

SongReaderI::SongReaderI(const char* filename) :
    filename(filename), header(), headerLoaded() {
  file.open(filename, std::ios_base::binary);
}

SongReaderI::SongReaderI(std::ifstream&& file) :
  filename(""), file(std::move(file)), header(), headerLoaded() {
}

SongReaderI::SongReaderI(std::ifstream&& file, std::unique_ptr<SongHeader> header) :
  filename(""), file(std::move(file)), header(std::move(header)), headerLoaded(true) {
  file.seekg(64);
}

const SongHeader* SongReaderI::getHeader() {
  if (!headerLoaded)
    loadHeader();
  return header.get();
}

void SongReaderI::close() {
  file.close();
}

void SongReaderI::loadHeader() {
  uint8_t buffer[64];
  file.seekg(0);
  file.read(reinterpret_cast<char*>(buffer), 64);
  if (file.good()) {
    header.reset(new SongHeader(buffer));
  }
  headerLoaded = true;
}

