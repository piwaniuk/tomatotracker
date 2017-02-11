#include <stdlib.h>

#include "instrument.h"
#include "list.h"

LinkedList* INSTRUMENTS;

BidirectionalIterator* song_instruments(void* song) {
  return list_iterator(INSTRUMENTS);
  //return malloc(sizeof(BidirectionalIterator));
}

char* instrument_repr(void* data) {
  //return "foobar";
  Instrument* instrument = (Instrument*)data;
  return instrument->identifier;
}
