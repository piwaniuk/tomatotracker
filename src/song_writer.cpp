#include <cstring>
#include <map>
#include <endian.h>

#include "song_io.h"
#include "song_writer.h"
#include "song_format_current.h"

extern "C" {
#include "1osc.h"
#include "instrument.h"
}

using Fmt = SongFormat<HEADER_CURRENT_VERSION>;

// Instrument definitions

static void fill1OscParams(Parameters1Osc* params, uint8_t* buffer) {
  fillBuffer(buffer + Fmt::PARAMS_1OSC_SHAPE_OFF, params->shape);
  fillBuffer(buffer + Fmt::PARAMS_1OSC_A_OFF, params->ampAtt);
  fillBuffer(buffer + Fmt::PARAMS_1OSC_D_OFF, params->ampDec);
  fillBuffer(buffer + Fmt::PARAMS_1OSC_S_OFF, params->ampSus);
  fillBuffer(buffer + Fmt::PARAMS_1OSC_R_OFF, params->ampRel);
  fillBuffer(buffer + Fmt::PARAMS_1OSC_CUTOFF_OFF, params->filter);
  fillBuffer(buffer + Fmt::PARAMS_1OSC_RES_OFF, params->res);
}

static void fillInstrumentParams(const Instrument* instrument, uint8_t* buffer) {
  switch(instrument->type) {
    case INSTRUMENT_TYPE_1OSC:
      fill1OscParams(
        reinterpret_cast<Parameters1Osc*>(instrument->parameters), buffer);
      break;
  }
}

static void saveInstrument(std::ofstream& file, const Instrument* instrument) {
  uint32_t baseSize = Fmt::INS_BASE_SIZE;
  uint32_t paramsSize = Fmt::InstrParamSizes::at(instrument->type);

  uint8_t buffer1[4];
  fillBuffer(buffer1, baseSize + paramsSize);

  uint8_t buffer2[baseSize + paramsSize];
  fillBuffer<6>(buffer2 + Fmt::INS_NAME_OFF, instrument->identifier);
  fillBuffer<32>(buffer2 + Fmt::INS_DESCR_OFF, instrument->description);
  fillBuffer(buffer2 + Fmt::INS_VOLUME_OFF, instrument->volume);
  fillBuffer(buffer2 + Fmt::INS_TYPE_OFF, instrument->type);
  
  fillInstrumentParams(instrument, buffer2 + Fmt::INS_PARAMS_OFF);
  
  file.write(reinterpret_cast<char*>(buffer1), 4);
  file.write(reinterpret_cast<char*>(buffer2), baseSize + paramsSize);
}

static void saveInstruments(std::ofstream& file, Song* song) {
  BidirectionalIterator* iter = song_instruments(song);
  while (!iter_is_end(iter)) {
    saveInstrument(file, reinterpret_cast<Instrument*>(iter_get(iter)));
    iter_next(iter);
  }
}

// Pattern definitions

static void fillPatternStep(uint8_t* buffer, const PatternStep* step, const songMap<Instrument>& instrumentMap) {
  fillBuffer(buffer + Fmt::PAT_STEP_NOTE_OFF, step->n);
  fillBuffer(buffer + Fmt::PAT_STEP_LEN_OFF, step->length);
  fillBuffer(buffer + Fmt::PAT_STEP_INS_OFF, instrumentMap.at(step->inst));
  fillBuffer(buffer + Fmt::PAT_STEP_CMD1_OFF, step->cmd1); //TODO: separate args
  fillBuffer(buffer + Fmt::PAT_STEP_CMD2_OFF, step->cmd2);
}

static void savePattern(
    std::ofstream& file, const Pattern* pattern, uint8_t length,
    const songMap<Instrument>& instrumentMap) {
  size_t bufferSize = Fmt::PAT_HEADER_SIZE + (length * Fmt::PAT_STEP_SIZE);
  uint8_t buffer[bufferSize];
  fillBuffer<6>(buffer + Fmt::PAT_NAME_OFF, pattern->identifier);
  fillBuffer<32>(buffer + Fmt::PAT_DESCR_OFF, pattern->description);
  for(int i = 0; i < length; ++i) {
    fillPatternStep(
      buffer + Fmt::PAT_STEPS_OFF + i * Fmt::PAT_STEP_SIZE,
      &pattern->steps[i], instrumentMap);
  }

  file.write(reinterpret_cast<char*>(buffer), bufferSize);
}

static void savePatterns(
    std::ofstream& file, Song* song,
    const songMap<Instrument>& instrumentMap) {
  BidirectionalIterator* iter = songObjectsIterator<Pattern>(song);
  while (!iter_is_end(iter)) {
    savePattern(
      file, reinterpret_cast<Pattern*>(iter_get(iter)),
      song->patternLength, instrumentMap);
    iter_next(iter);
  }
  iter_destroy(iter);
}

// Phrase definitions



static void fillPhraseStep(uint8_t* buffer, Pattern* pattern, songMap<Pattern> patternMap) {
  fillBuffer(buffer, patternMap.at(pattern));
}

static void savePhrase(std::ofstream& file, Phrase* phrase, songMap<Pattern> patternMap) {
  uint8_t buffer1[Fmt::PHR_LENGTH_SIZE];
  fillBuffer(buffer1, phrase->length);

  size_t buffer2Size = Fmt::PHR_HEADER_SIZE + (phrase->length * Fmt::PHR_STEP_SIZE);
  uint8_t buffer2[buffer2Size];
  fillBuffer<6>(buffer2 + Fmt::PHR_NAME_OFF, phrase->name);
  fillBuffer<32>(buffer2 + Fmt::PHR_DESCR_OFF, phrase->descr);
  for(int i = 0; i < phrase->length; ++i)
    fillPhraseStep(buffer2 + Fmt::PHR_STEPS_OFF + i * Fmt::PHR_STEP_SIZE, phrase->patterns[i], patternMap);

  file.write(reinterpret_cast<char*>(buffer1), Fmt::PHR_LENGTH_SIZE);
  file.write(reinterpret_cast<char*>(buffer2), buffer2Size);
}

static void savePhrases(std::ofstream& file, Song* song, songMap<Pattern> patternMap) {
  BidirectionalIterator* iter = songObjectsIterator<Phrase>(song);
  while (!iter_is_end(iter)) {
    savePhrase(file, reinterpret_cast<Phrase*>(iter_get(iter)), patternMap);
    iter_next(iter);
  }
  iter_destroy(iter);
}

// Track definitions

static void fillTrackStep(uint8_t* buffer, TrackEntry* entry, songMap<Phrase> phraseMap) {
  fillBuffer(buffer, phraseMap.at(entry->phrase));
}

static void saveTrack(std::ofstream& file, uint16_t lastPos, TrackEntry* entries, songMap<Phrase> phraseMap) {
  uint8_t buffer1[Fmt::TRK_LAST_POS_SIZE];
  fillBuffer(buffer1, lastPos);
  
  size_t buffer2Size = Fmt::TRK_ENTRY_SIZE * (lastPos + 1);
  uint8_t buffer2[buffer2Size];
  for(int i = 0; i <= lastPos; ++i)
    fillTrackStep(buffer2 + i * Fmt::TRK_ENTRY_SIZE, &entries[i], phraseMap);
  
  file.write(reinterpret_cast<char*>(buffer1), Fmt::TRK_LAST_POS_SIZE);
  file.write(reinterpret_cast<char*>(buffer2), buffer2Size);
}

static void saveTracks(std::ofstream& file, Song* song, songMap<Phrase> phraseMap) {
  for(int i = 0; i < TRACK_COUNT; ++i)
    saveTrack(file, song->lastPos[i], song->tracks[i], phraseMap);
}

// SongWriter definitions

SongWriter::SongWriter(const char* filename) : filename(filename), file() {
  file.open(filename, std::ios_base::binary);
}

void SongWriter::saveSong(Song* song) {
  SongHeader header(song);
  uint8_t headerBuffer[HEADER_SIZE];
  
  header.toBuffer(headerBuffer);
  file.write(reinterpret_cast<char*>(headerBuffer), HEADER_SIZE);
  
  saveInstruments(file, song);
  savePatterns(file, song, makeMap<Instrument>(song));
  savePhrases(file, song, makeMap<Pattern>(song));
  saveTracks(file, song, makeMap<Phrase>(song));
}

void SongWriter::close() {
  file.close();
}

