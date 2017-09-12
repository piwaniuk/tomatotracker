#include "song_loader.h"
#include "song_reader_current.h"
#include "song_reader_0_1.h"

SongLoader::SongLoader(std::ifstream&& file) :
    file(std::move(file)) {
}

std::unique_ptr<SongHeader> SongLoader::loadHeader() {
  char buffer[64];
  std::unique_ptr<SongHeader> ret;
  file.seekg(0);
  file.read(buffer, 64);
  if (file.good()) {
    ret.reset(new SongHeader(reinterpret_cast<uint8_t*>(buffer)));
  }
  return ret;
}

Song* SongLoader::loadSong() {
  std::unique_ptr<SongHeader> header = loadHeader();
  if (header.get() == nullptr) {
    return nullptr;
  }
  if (header->getVersion() == HEADER_CURRENT_VERSION) {
    SongReaderCurrent reader(std::move(file), std::move(header));
    Song* ret = reader.loadSong();
    reader.close();
    return ret;
  }
  else if (header->getVersion() == 0x00000001) {
    SongReader<0x00000001> reader(std::move(file), std::move(header));
    Song* ret = reader.loadSong();
    reader.close();
    return ret;
  }
  else {
    return nullptr;
  }
}

void SongLoader::close() {
  file.close();
}
