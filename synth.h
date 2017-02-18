#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>

typedef uint16_t sample_t;
#define SAMPLE_0 32768
#define SAMPLE_MIN 0
#define SAMPLE_MAX USHRT_MAX
#define SAMPLE_RATE 48000;

int note_to_freq(uint8_t note);

sample_t gen_triangle(uint16_t phase);
sample_t gen_saw(uint16_t phase);
sample_t gen_pulse(uint16_t phase);


#endif

