#ifndef TRACKER_FIELD_H
#define TRACKER_FIELD_H

#ifdef __cplusplus
extern "C" {
#endif

struct TrackerField;

/**
 * Interface for handling editing commands of some values in a song.
 */
typedef struct TrackerField TrackerField;

/**
 * Handle tracker field commands.
 */
bool tracker_field_commands(TrackerField* field, int ch);

/**
 * Destroy tracker field and free its memory.
 */
void tracker_field_destroy(TrackerField* field);

#ifdef __cplusplus
}
#endif

#endif
