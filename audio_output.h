#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

typedef uint16_t sample_t;
#define SAMPLE_0 32768
#define SAMPLE_MIN 0
#define SAMPLE_MAX USHRT_MAX

void audio_add_event_freq(int freq);
void audio_output_initialize();
void audio_output_finalize();

#endif
