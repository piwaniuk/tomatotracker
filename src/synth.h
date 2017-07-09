#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>

# define M_PI		3.14159265358979323846	/* pi */

typedef uint16_t sample_t;
#define SAMPLE_0 32768
#define SAMPLE_MIN 0
#define SAMPLE_MAX USHRT_MAX
#define SAMPLE_RATE 48000

int note_to_freq(uint8_t note);

float gen_sin(float phase);
float gen_tri(float phase);
float gen_saw(float phase);
float gen_sqr(float phase);
float gen_pul(float phase);
float gen_noi(float phase);
float gen_car(float phase);
float gen_plu(float phase);

inline uint32_t ms_to_samples(uint16_t t) {
  return t * SAMPLE_RATE / 1000;
}

#endif

