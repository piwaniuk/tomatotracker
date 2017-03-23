#ifndef PHRASE_FIELD_H
#define PHRASE_FIELD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "song_screen.h"
#include "tracker_field.h"

/**
 * Construct a tracker field for editing phrase values
 */
TrackerField* phrase_field_create(SongScreen* screen);

#ifdef __cplusplus
}
#endif

#endif

