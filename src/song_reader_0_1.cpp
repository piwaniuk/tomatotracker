#include "song_reader_0_1.h"
#include "song_format_0_1.h"

using Fmt = SongFormat<0x00000001>;

SongReader<0x00000001>::SongReader(std::ifstream&& file, std::unique_ptr<SongHeader> header) :
    SongReaderI(std::move(file), std::move(header)) {
}

// Instrument definitions

static Parameters1Osc* take1OscParams(uint8_t* buffer) {
  Parameters1Osc* params =
    reinterpret_cast<Parameters1Osc*>(malloc(sizeof(Parameters1Osc)));
  params->shape = fromBuffer<uint8_t>(buffer + Fmt::PARAMS_1OSC_SHAPE_OFF);
  params->ampAtt = fromBuffer<uint16_t>(buffer + Fmt::PARAMS_1OSC_A_OFF) / 48;
  params->ampDec = fromBuffer<uint16_t>(buffer + Fmt::PARAMS_1OSC_D_OFF) / 48;
  params->ampSus = fromBuffer<uint16_t>(buffer + Fmt::PARAMS_1OSC_S_OFF);
  params->ampRel = fromBuffer<uint16_t>(buffer + Fmt::PARAMS_1OSC_R_OFF) / 48;
  params->filter = 20000;
  params->res = 0;
  return params;
}

static void* takeInstrumentParams(uint8_t* buffer, uint16_t type) {
  void* ret;
  switch(type) {
    case INSTRUMENT_TYPE_1OSC:
      ret = take1OscParams(buffer);
      break;
    default:
      ret = nullptr;
  }
  return ret;
}

static Instrument* loadInstrument(std::ifstream& file) {
  Instrument* instrument =
    reinterpret_cast<Instrument*>(malloc(sizeof(Instrument)));
  uint8_t buffer1[4];
  file.read(reinterpret_cast<char*>(buffer1), 4);
  uint32_t buffer2Size = fromBuffer<uint32_t>(buffer1);
  uint8_t buffer2[buffer2Size];
  file.read(reinterpret_cast<char*>(buffer2), buffer2Size);
  
  stringFromBuffer<6>(buffer2 + Fmt::INS_NAME_OFF, instrument->identifier);
  stringFromBuffer<32>(buffer2 + Fmt::INS_DESCR_OFF, instrument->description);
  instrument->volume = fromBuffer<uint8_t>(buffer2 + Fmt::INS_VOLUME_OFF);
  instrument->type = fromBuffer<uint16_t>(buffer2 + Fmt::INS_TYPE_OFF);
  
  instrument->parameters =
    takeInstrumentParams(buffer2 + Fmt::INS_PARAMS_OFF, instrument->type);

  return instrument;
}

static void loadInstruments(std::ifstream& file, Song* song, uint32_t count) {
  BidirectionalIterator* iter = songObjectsIterator<Instrument>(song);
  for (uint32_t i = 0; i < count; ++i) {
    Instrument* newInstrument = loadInstrument(file);
    iter_insert(iter, newInstrument);
  }
}

// Pattern definitions

static void takePatternStep(
    uint8_t* buffer, PatternStep* step,
    songRevMap<Instrument> instrumentRevMap) {
  step->n = fromBuffer<uint8_t>(buffer + Fmt::PAT_STEP_NOTE_OFF);
  step->length = fromBuffer<uint8_t>(buffer + Fmt::PAT_STEP_LEN_OFF);
  step->inst = instrumentRevMap.at(fromBuffer<uint32_t>(buffer + Fmt::PAT_STEP_INS_OFF));
  step->cmd1 = fromBuffer<uint32_t>(buffer + Fmt::PAT_STEP_CMD1_OFF);
  step->cmd2 = fromBuffer<uint32_t>(buffer + Fmt::PAT_STEP_CMD2_OFF);
}

static Pattern* loadPattern(
    std::ifstream& file, Song* song, uint8_t length,
    songRevMap<Instrument> instrumentMap) {
  Pattern* pattern =
    reinterpret_cast<Pattern*>(malloc(sizeof(Pattern)));
  size_t bufferSize = Fmt::PAT_HEADER_SIZE + (length * Fmt::PAT_STEP_SIZE);
  uint8_t buffer[bufferSize];
  file.read(reinterpret_cast<char*>(buffer), bufferSize);
  
  stringFromBuffer<6>(buffer + Fmt::PAT_NAME_OFF, pattern->identifier);
  stringFromBuffer<32>(buffer + Fmt::PAT_DESCR_OFF, pattern->description);
  
  for(uint8_t i = 0; i < length; ++i)
    takePatternStep(buffer + Fmt::PAT_STEPS_OFF + i * Fmt::PAT_STEP_SIZE, &pattern->steps[i], instrumentMap);
  
  return pattern;
}

static void loadPatterns(
    std::ifstream& file, Song* song, uint32_t count, uint8_t length,
    songRevMap<Instrument> instrumentMap) {
  BidirectionalIterator* iter = songObjectsIterator<Pattern>(song);
  for (uint32_t i = 0; i < count; ++i) {
    Pattern* newPattern = loadPattern(file, song, length, instrumentMap);
    iter_insert(iter, newPattern);
  }
}

// Phrase definitions

static Pattern* takePhraseStep(
    uint8_t* buffer, songRevMap<Pattern> patternMap) {
  return patternMap.at(fromBuffer<uint32_t>(buffer));
}

static Phrase* loadPhrase(
    std::ifstream& file, Song* song, songRevMap<Pattern> patternMap) {
  Phrase* phrase =
    reinterpret_cast<Phrase*>(malloc(sizeof(Phrase)));
  phrase->patterns =
    reinterpret_cast<Pattern**>(malloc(sizeof(Pattern*) * TRACK_SIZE));
  for(int i = 0; i < TRACK_SIZE; ++i)
    phrase->patterns[i] = nullptr;

  uint8_t buffer1[Fmt::PHR_LENGTH_SIZE];
  file.read(reinterpret_cast<char*>(buffer1), Fmt::PHR_LENGTH_SIZE);
  phrase->length = fromBuffer<uint16_t>(buffer1);

  size_t buffer2Size = Fmt::PHR_HEADER_SIZE + (phrase->length) * Fmt::PHR_STEP_SIZE;
  uint8_t buffer2[buffer2Size];
  file.read(reinterpret_cast<char*>(buffer2), buffer2Size);
  stringFromBuffer<6>(buffer2 + Fmt::PHR_NAME_OFF, phrase->name);
  stringFromBuffer<32>(buffer2 + Fmt::PHR_DESCR_OFF, phrase->descr);

  for(uint16_t i = 0; i < phrase->length; ++i)
    phrase->patterns[i] =
      takePhraseStep(buffer2 + Fmt::PHR_STEPS_OFF + i * Fmt::PHR_STEP_SIZE, patternMap);

  return phrase;
}

static void loadPhrases(
    std::ifstream& file, Song* song, uint32_t count,
    songRevMap<Pattern> patternMap) {
  BidirectionalIterator* iter = songObjectsIterator<Phrase>(song);
  for (uint32_t i = 0; i < count; ++i) {
    Phrase* newPhrase = loadPhrase(file, song, patternMap);
    iter_insert(iter, newPhrase);
  }
}

// Track definitions

static Phrase* takeTrackEntry(uint8_t* buffer, songRevMap<Phrase> phraseMap) {
  return phraseMap.at(fromBuffer<uint32_t>(buffer));
}

static void rebuildTail(TrackEntry* track, uint16_t lastPos) {;
  Phrase* lastPhrase = track[lastPos].phrase;
  uint16_t lastTailPos;
  if (lastPhrase == nullptr)
    lastTailPos =  1;
  else
    lastTailPos = lastPos + lastPhrase->length;

  track[0].tail = 0;
  for(uint16_t i = 1; i < lastTailPos; ++i)
    if (track[i].phrase == nullptr)
      track[i].tail = track[i - 1].tail + 1;
    else
      track[i].tail = 0;
}

static void loadTrack(
    std::ifstream& file, Song* song, uint8_t index,
    songRevMap<Phrase> phraseMap) {
  uint8_t buffer1[Fmt::TRK_LAST_POS_SIZE];
  file.read(reinterpret_cast<char*>(buffer1), Fmt::TRK_LAST_POS_SIZE);
  song->lastPos[index] = fromBuffer<uint16_t>(buffer1);
  
  size_t buffer2Size = (song->lastPos[index] + 1) * Fmt::TRK_ENTRY_SIZE;
  uint8_t buffer2[buffer2Size];
  file.read(reinterpret_cast<char*>(buffer2), buffer2Size);
  for(uint16_t i = 0; i <= song->lastPos[index]; ++i)
    song->tracks[index][i].phrase =
      takeTrackEntry(buffer2 + i * Fmt::TRK_ENTRY_SIZE, phraseMap);
  
  rebuildTail(song->tracks[index], song->lastPos[index]);
}

static void loadTracks(
    std::ifstream& file, Song* song, uint8_t count,
    songRevMap<Phrase> phraseMap) {
  for(uint8_t i = 0; i < count; ++i)
    loadTrack(file, song, i, phraseMap);
  song_update_length(song);
}

Song* SongReader<0x00000001>::loadSong() {
  const SongHeader* header = getHeader();
  Song* song = reinterpret_cast<Song*>(malloc(sizeof(Song)));
  if (header == nullptr)
    return nullptr;
  header->toSong(song);
  loadInstruments(file, song, header->getInstrumentCount());
  loadPatterns(
    file, song, header->getPatternCount(), header->getPatternLength(),
    makeRevMap<Instrument>(song));
  loadPhrases(file, song, header->getPhraseCount(), makeRevMap<Pattern>(song));
  loadTracks(file, song, header->getTrackCount(), makeRevMap<Phrase>(song));
  return song;
}

