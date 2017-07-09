#ifndef SONG_IO_H
#define SONG_IO_H

#include <string>
#include <cstring>
#include <map>

extern "C" {
#include "song.h"
}

// Format constants

static const uint32_t HEADER_IDENTIFIER = 0x52544d54;
static const uint32_t HEADER_CURRENT_VERSION = 0x00000002;

static const size_t HEADER_SIZE = 64;

template<int VERSION> class SongFormat;

// Song header declaration

class SongHeader {
public:
  SongHeader(const Song* song);
  SongHeader(const uint8_t* buffer);
  void toBuffer(uint8_t* buffer) const ;
  void toSong(Song* song) const;

  uint32_t getVersion() const;
  uint8_t getPatternLength() const;
  uint8_t getTrackCount() const;
  uint32_t getInstrumentCount() const;
  uint32_t getPatternCount() const;
  uint32_t getPhraseCount() const;
  
private:
  uint32_t identifier;
  uint32_t version;
  char title[40];
  uint8_t bpm;
  uint8_t tpb;
  uint8_t patternSize;
  uint8_t trackCount;
  uint32_t instrumentCount;
  uint32_t patternCount;
  uint32_t phraseCount;
};

// Little endian buffer filling functions

void fillBuffer(uint8_t* buffer, uint32_t value);
void fillBuffer(uint8_t* buffer, uint16_t value);
void fillBuffer(uint8_t* buffer, uint8_t value);

template<size_t N> void fillBuffer(uint8_t* buffer, const char value[N]) {
  memset(buffer, 0, N);
  strncpy(reinterpret_cast<char*>(buffer), value, N);
}

// Little endian buffer reading functions

template<typename T> T fromBuffer(const uint8_t* buffer);
template<> uint32_t fromBuffer(const uint8_t* buffer);
template<> uint16_t fromBuffer(const uint8_t* buffer);
template<> uint8_t fromBuffer(const uint8_t* buffer);

template<size_t N> void stringFromBuffer(const uint8_t* buffer, char* string) {
  snprintf(string, N + 1, "%s", buffer);
}

// Song object map definitions

template<typename T> using songMap = std::map<T*, uint32_t>;
template<typename T> using songRevMap = std::map<uint32_t, T*>;

template<typename T> BidirectionalIterator* songObjectsIterator(Song* song);

template<> BidirectionalIterator* songObjectsIterator<Instrument>(Song* song);
template<> BidirectionalIterator* songObjectsIterator<Pattern>(Song* song);
template<> BidirectionalIterator* songObjectsIterator<Phrase>(Song* song);

template<typename T> songMap<T> makeMap(Song* song) {
  songMap<T> ret = {{nullptr, 0}};
  BidirectionalIterator* iter = songObjectsIterator<T>(song);
  uint32_t index = 1;
  while (!iter_is_end(iter)) {
    ret[reinterpret_cast<T*>(iter_get(iter))] = index;
    iter_next(iter);
    ++index;
  }
  iter_destroy(iter);
  return ret;
}

template<typename T> songRevMap<T> makeRevMap(Song* song) {
  songRevMap<T> ret = {{0, nullptr}};
  BidirectionalIterator* iter = songObjectsIterator<T>(song);
  uint32_t index = 1;
  while (!iter_is_end(iter)) {
    ret[index] = reinterpret_cast<T*>(iter_get(iter));
    iter_next(iter);
    ++index;
  }
  iter_destroy(iter);
  return ret;
}

#endif
