#ifndef PATTERN_FIELD_H
#define PATTERN_FIELD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "phrase_screen.h"
#include "tracker_field.h"

/**
 * Construct a tracker field for editing pattern values
 */
TrackerField* pattern_field_create(PhraseScreen* screen);

#ifdef __cplusplus
}
#endif

#endif

