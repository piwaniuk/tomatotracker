#ifndef _1OSC_H
#define _1OSC_H

#include "audio_event.h"

typedef struct {
  uint8_t shape;
  uint16_t amp;
  uint16_t ampAtt;
  uint16_t ampDec;
  uint16_t ampSus;
  uint16_t ampRel;
} Parameters1Osc;

AudioEvent* ae_1osc_create(int freq, uint32_t length, void* parameters);

#endif
