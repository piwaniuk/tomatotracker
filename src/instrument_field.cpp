#include "instrument_field.h"
#include "tracker_field_interface.h"

/**
 * Implementation of tracker field with instrument values
 */
struct InstrumentField final : public TrackerField {
  InstrumentField(PatternScreen* screen)
    : TrackerField(instrument_repr), screen(screen) {
  }

  virtual BidirectionalIterator* getIterator() {
    return song_instruments(screen->song);
  }

  virtual void* getValue() {
    return screen->pattern->steps[screen->row].inst;
  }

  virtual void setValue(void* newValue) {
    // this field ignores clear command
    if (newValue != nullptr)
      screen->pattern->steps[screen->row].inst =
        reinterpret_cast<Instrument*>(newValue);
  }

  virtual void* getRecentValue(){
    return screen->lastInstrument;
  }

  virtual void setRecentValue(void* newValue) {
    screen->lastInstrument = reinterpret_cast<Instrument*>(newValue);
  }

  virtual bool isNameTaken(const char* name) {
    return song_has_instrument(screen->song, name);
  }

  virtual void editor() {
    // TODO: instrument screen
  }

  virtual void* createNew(const char* name) {
    Instrument* ret = instrument_create(name);
    song_add_instrument(screen->song, ret);
    return ret;
  }

  virtual void destroy() {
    delete this;
  }

private:
  PatternScreen* screen;
};


TrackerField* instrument_field_create(PatternScreen* screen) {
  return new InstrumentField(screen);
}

