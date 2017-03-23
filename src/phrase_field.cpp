#include "phrase_field.h"
#include "tracker_field_interface.h"

extern "C" {
#include "phrase_screen.h"
#include "widgets.h"
}

/**
 * Implementation of tracker field with pattern values
 */
struct PhraseField final : public TrackerField {
  PhraseField(SongScreen* screen)
    : TrackerField(phrase_repr), screen(screen) {
  }

  virtual BidirectionalIterator* getIterator() {
    return song_phrases(screen->song);
  }

  virtual void* getValue() {
    return screen->song->tracks[screen->col][screen->row].phrase;
  }

  virtual void setValue(void* newValue) {
    song_set_phrase(
      screen->song, reinterpret_cast<Phrase*>(newValue),
      screen->col, screen->row);
  }

  virtual void* getRecentValue(){
    return screen->lastPhrase;
  }

  virtual void setRecentValue(void* newValue) {
    screen->lastPhrase = reinterpret_cast<Phrase*>(newValue);
  }

  virtual bool isNameTaken(const char* name) {
    return song_has_phrase(screen->song, name);
  }

  virtual void editor() {
    Phrase* phrase = reinterpret_cast<Phrase*>(getValue());
    if (phrase != NULL) {
      PhraseScreen phraseScreen = {
        false, screen->song, phrase, 0, 0, nullptr, screen->tracker
      };
      phrase_screen(&phraseScreen);
    }
    else
      status_message("No phrase to edit here.");
  }

  virtual void* createNew(const char* name) {
    Phrase* ret = phrase_create(name);
    song_add_phrase(screen->song, ret);
    return ret;
  }

  virtual void destroy() {
    delete this;
  }

private:
  SongScreen* screen;
};


TrackerField* phrase_field_create(SongScreen* screen) {
  return new PhraseField(screen);
}

