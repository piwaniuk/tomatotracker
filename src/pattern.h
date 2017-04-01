#ifndef PATTERN_H
#define PATTERN_H

#include "instrument.h"

typedef struct {
  uint8_t n;
  uint8_t length;
  Instrument* inst;
  uint8_t cmd1;
  uint8_t cmd2;
} PatternStep;

typedef struct {
  char identifier[7];
  char description[32];
  PatternStep steps[20];
} Pattern;

/**
 * Create a new empty pattern with given name.
 */
Pattern* pattern_create(const char* name);

/**
 * Destroy the pattern and free all memory allocated for it.
 */
void pattern_destroy(Pattern* pattern);

/**
 * Compare two patterns by their names.
 */
int pattern_cmp_name(void* p1, void* p2);

/**
 * Return a string representation of the pattern
 */
char* pattern_repr(void* pattern);

#endif
