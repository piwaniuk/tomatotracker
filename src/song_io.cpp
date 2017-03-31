#include <cstring>
#include <endian.h>

#include "song_io.h"

extern "C" {
#include "1osc.h"
#include "instrument.h"
}

// SongHeader definitions

SongHeader::SongHeader(const Song* song) :
 identifier(HEADER_IDENTIFIER),
 version(HEADER_VERSION),
 title(),
 bpm(120), tpb(4),
 patternSize(16),
 trackCount(5),
 instrumentCount(song_instrument_count(song)),
 patternCount(song_pattern_count(song)),
 phraseCount(song_phrase_count(song)) {
 // initialize strings
 strcpy(title, song->title);
}

// header field offsets
static const size_t HEADER_IDENT_OFF = 0;
static const size_t HEADER_VER_OFF = 4;
static const size_t HEADER_TITLE_OFF = 8;
static const size_t HEADER_BPM_OFF = 48;
static const size_t HEADER_TPB_OFF = 49;
static const size_t PATTERN_SIZE_OFF = 50;
static const size_t HEADER_TRK_CNT_OFF = 51;
static const size_t HEADER_INS_CNT_OFF = 52;
static const size_t HEADER_PAT_CNT_OFF = 56;
static const size_t HEADER_PHR_CNT_OFF = 60;

SongHeader::SongHeader(const uint8_t* buffer) :
  identifier(fromBuffer<uint32_t>(buffer + HEADER_IDENT_OFF)),
  version(fromBuffer<uint32_t>(buffer + HEADER_VER_OFF)),
  title(),
  bpm(fromBuffer<uint8_t>(buffer + HEADER_BPM_OFF)),
  tpb(fromBuffer<uint8_t>(buffer + HEADER_TPB_OFF)),
  patternSize(fromBuffer<uint8_t>(buffer + PATTERN_SIZE_OFF)),
  trackCount(fromBuffer<uint8_t>(buffer + HEADER_TRK_CNT_OFF)),
  instrumentCount(fromBuffer<uint32_t>(buffer + HEADER_INS_CNT_OFF)),
  patternCount(fromBuffer<uint32_t>(buffer + HEADER_PAT_CNT_OFF)),
  phraseCount(fromBuffer<uint32_t>(buffer + HEADER_PHR_CNT_OFF)) {
  // load strings
  stringFromBuffer<39>(buffer + HEADER_TITLE_OFF, title);
}

void SongHeader::toBuffer(unsigned char* buffer) const {
  fillBuffer(buffer + HEADER_IDENT_OFF, identifier);
  fillBuffer(buffer + HEADER_VER_OFF, version);
  fillBuffer(buffer + HEADER_INS_CNT_OFF, instrumentCount);
  fillBuffer(buffer + HEADER_PAT_CNT_OFF, patternCount);
  fillBuffer(buffer + HEADER_PHR_CNT_OFF, phraseCount);

  fillBuffer(buffer + HEADER_BPM_OFF, bpm);
  fillBuffer(buffer + HEADER_TPB_OFF, tpb);
  fillBuffer(buffer + PATTERN_SIZE_OFF, patternSize);
  fillBuffer(buffer + HEADER_TRK_CNT_OFF, trackCount);

  fillBuffer<40>(buffer + HEADER_TITLE_OFF, title);
}

void SongHeader::toSong(Song* song) const {
  strcpy(song->title, title);
  song->tempo = bpm;
  song->tpb = tpb;
  song->patternLength = patternSize;
  song->instruments = list_create();
  song->patterns = list_create();
  song->phrases = list_create();
  song->tracks =
    reinterpret_cast<TrackEntry**>(malloc(trackCount * sizeof(TrackEntry*)));
  song->lastPos =
    reinterpret_cast<uint16_t*>(malloc(trackCount * sizeof(uint16_t)));
  for(int i = 0; i < trackCount; ++i) {
    song->tracks[i] =
      reinterpret_cast<TrackEntry*>(malloc(TRACK_SIZE * sizeof(TrackEntry)));
    song->lastPos[i] = 0;
    for(int j = 0; j < TRACK_SIZE; ++j)
      song->tracks[i][j] = {nullptr, 0};
  }
}

uint8_t SongHeader::getPatternLength() const {
  return patternSize;
}

uint8_t SongHeader::getTrackCount() const {
  return trackCount;
}

uint32_t SongHeader::getInstrumentCount() const {
  return instrumentCount;
}

uint32_t SongHeader::getPatternCount() const {
  return patternCount;
}

uint32_t SongHeader::getPhraseCount() const {
  return phraseCount;
}

// Little endian buffer filling functions

void fillBuffer(uint8_t* buffer, uint32_t value) {
  *reinterpret_cast<uint32_t*>(buffer) = htole32(value);
}

void fillBuffer(uint8_t* buffer, uint16_t value) {
  *reinterpret_cast<uint16_t*>(buffer) = htole16(value);
}

void fillBuffer(uint8_t* buffer, uint8_t value) {
  *buffer = value;
}

// Little endian buffer reading functions

template<> uint32_t fromBuffer(const uint8_t* buffer) {
  return le32toh(*reinterpret_cast<const uint32_t*>(buffer));
}

template<> uint16_t fromBuffer(const uint8_t* buffer) {
  return le16toh(*reinterpret_cast<const uint16_t*>(buffer));
}

template<> uint8_t fromBuffer(const uint8_t* buffer){
  return buffer[0];
}

// Song object map definitions

template<typename T> using songMap = std::map<T*, uint32_t>;

// TODO: make static
template<typename T> BidirectionalIterator* songObjectsIterator(Song* song);

template<> BidirectionalIterator* songObjectsIterator<Instrument>(Song* song) {
  return song_instruments(song);
}

template<> BidirectionalIterator* songObjectsIterator<Pattern>(Song* song) {
  return song_patterns(song);
}

template<> BidirectionalIterator* songObjectsIterator<Phrase>(Song* song) {
  return song_phrases(song);
}

