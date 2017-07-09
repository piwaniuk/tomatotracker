#ifndef _1OSC_H
#define _1OSC_H

#include "audio_event.h"

enum {
  SHAPE_SIN, SHAPE_TRI, SHAPE_SAW, SHAPE_SQR,
  SHAPE_PUL, SHAPE_NOI, SHAPE_CAR, SHAPE_PLU
};

#define LAST_SHAPE SHAPE_PLU

typedef struct {
  uint8_t shape;
  uint16_t ampAtt;
  uint16_t ampDec;
  uint16_t ampSus;
  uint16_t ampRel;
  uint16_t filter;
  uint16_t res;
} Parameters1Osc;

AudioEvent* ae_1osc_create(float freq, uint32_t length, uint8_t volume, void* parameters);

#endif
