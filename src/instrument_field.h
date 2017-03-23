#ifndef INSTRUMENT_FIELD_H
#define INSTRUMENT_FIELD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pattern_screen.h"
#include "tracker_field.h"

/**
 * Construct a tracker field for editing instrument values
 */
TrackerField* instrument_field_create(PatternScreen* screen);

#ifdef __cplusplus
}
#endif

#endif

