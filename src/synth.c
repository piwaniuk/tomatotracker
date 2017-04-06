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

sample_t gen_saw(uint16_t phase) {
  return phase;
}

sample_t gen_pulse(uint16_t phase) {
  return phase < 0x1000 ? 0 : 0xFFFF;
}

uint32_t ms_to_samples(uint16_t t);

