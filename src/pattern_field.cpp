#include "pattern_field.h"
#include "tracker_field_interface.h"

extern "C" {
#include "pattern_screen.h"
#include "widgets.h"
}

/**
 * Implementation of tracker field with pattern values
 */
struct PatternField final : public TrackerField {
  PatternField(PhraseScreen* screen)
    : TrackerField(pattern_repr), screen(screen) {
  }

  virtual BidirectionalIterator* getIterator() {
    return song_patterns(screen->song);
  }

  virtual void* getValue() {
    return screen->phrase->patterns[screen->row];
  }

  virtual void setValue(void* newValue) {
    phrase_set_pattern(
      screen->phrase, reinterpret_cast<Pattern*>(newValue), screen->row);
    // notify song about phrase update
    song_phrase_edited(screen->song, screen->phrase);
  }

  virtual void* getRecentValue(){
    return screen->lastPattern;
  }

  virtual void setRecentValue(void* newValue) {
    screen->lastPattern = reinterpret_cast<Pattern*>(newValue);
  }

  virtual bool isNameTaken(const char* name) {
    return song_has_pattern(screen->song, name);
  }

  virtual void editor() {
    Pattern* pattern = reinterpret_cast<Pattern*>(getValue());
    if (pattern != NULL) {
      PatternScreen patternScreen = {
        false, screen->song, pattern, 0, 0, 4, song_first_instrument(screen->song), screen->tracker
      };
      pattern_screen(&patternScreen);
    }
    else
      status_message("No pattern to edit here.");
  }

  virtual void* createNew(const char* name) {
    Pattern* ret = pattern_create(name);
    song_add_pattern(screen->song, ret);
    return ret;
  }

  virtual void destroy() {
    delete this;
  }

private:
  PhraseScreen* screen;
};


TrackerField* pattern_field_create(PhraseScreen* screen) {
  return new PatternField(screen);
}

