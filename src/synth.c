#include "synth.h"
#include <math.h>

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

float gen_sin(float phase) {
  return sin(M_PI * 2.0f * phase);
}

float gen_tri(float phase) {
  return fabs(phase - 0.5f) * 4.0f - 1.0f;
}

float gen_saw(float phase) {
  return phase * 2.0f - 1.0f;
}

float gen_sqr(float phase) {
  return phase < 0.5f ? -1.0f : 1.0f;
}

float gen_pul(float phase) {
  return phase < 0.25f ? -1.0f : 1.0f;
}

float gen_noi(float phase) {
  static const float A = 2.3, B = 1.9, C = 0.7;
  return fmod(A * phase * phase + B * phase + C, 1.0) * 2.0 - 1.0;
}

float gen_car(float phase) {
  float ret = gen_tri(phase);
  if (ret < 0.0)
    return -ret * ret;
  else
    return ret * ret;
}

float gen_plu(float phase) {
  float ret = gen_tri(phase);
  if (ret < 0.0)
    return -sqrt(-ret);
  else
    return sqrt(ret);
}

uint32_t ms_to_samples(uint16_t t);

