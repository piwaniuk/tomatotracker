#ifndef PATTERN_H
#define PATTERN_H

#include "instrument.h"

typedef struct {
  uint8_t n;
  Instrument* inst;
  uint8_t cmd1;
  uint8_t cmd2;
} PatternStep;

typedef struct {
  char identifier[7];
  char description[32];
  uint8_t length;
  PatternStep steps[20];
} Pattern;

#endif
