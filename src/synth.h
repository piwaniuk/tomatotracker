#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>

typedef uint16_t sample_t;
#define SAMPLE_0 32768
#define SAMPLE_MIN 0
#define SAMPLE_MAX USHRT_MAX
#define SAMPLE_RATE 48000

int note_to_freq(uint8_t note);

sample_t gen_triangle(uint16_t phase);
float gen_saw(float phase);
float gen_pulse(float phase);

inline uint32_t ms_to_samples(uint16_t t) {
  return t * SAMPLE_RATE / 1000;
}


#endif

