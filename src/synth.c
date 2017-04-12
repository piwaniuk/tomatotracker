#include "synth.h"

int note_to_freq(uint8_t note) {
  static int freqs[] = {
    262, 277, 294, 311, 330, 349,
    370, 392, 415, 440, 466, 494
  };
  int oct = note / 16;
  int sound = note % 16;
  int ret = freqs[sound];
  while (oct < 4) {
    ret /= 2;
    oct += 1;
  }
  while (oct > 4) {
    ret *= 2;
    oct -= 1;
  }
  return ret;
}

sample_t gen_triangle(uint16_t phase) {
  if (phase < 0x7FFF)
    return phase * 2;
  else 
    return -1 - (phase * 2);
}

float gen_saw(float phase) {
  return phase * 2.0 - 1.0;
}

float gen_pulse(float phase) {
  return phase < 0.25 ? -1.0 : 1.0;
}

uint32_t ms_to_samples(uint16_t t);

