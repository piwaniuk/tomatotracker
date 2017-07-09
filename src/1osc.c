#include <math.h>
#include <string.h>

#include "1osc.h"
#include "audio_event.h"
#include "audio_event_interface.h"
#include "synth.h"

#define COMB_BUFFER_SIZE 2048

typedef struct {
  Parameters1Osc* params;
  uint8_t volume;
  float phase;
  float phaseStep;
  uint32_t time;
  uint32_t length;

  float combA;
  uint16_t combOffset;
  uint16_t combPos;
  float combBuffer[COMB_BUFFER_SIZE];

  float filterA;
  float filterOut;
} State1Osc;

static float gen_ads_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
  float floatSus = params->ampSus / 65535.0;
  if (t < ms_to_samples(params->ampAtt)) {
    float p = t;
    float c = params->ampAtt * (SAMPLE_RATE / 1000);
    return 1.0 * p / c;
  }
  else if (t < ms_to_samples(params->ampAtt + params->ampDec)) {
    float p = t - params->ampAtt * (SAMPLE_RATE / 1000);
    float c = params->ampDec * (SAMPLE_RATE / 1000);
    return 1.0 - (1.0 - floatSus) * p / c;
  }
  else
    return floatSus;
}

static float gen_rel_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
  if (t < len)
    return 1.0;
  else if (t < len + (uint64_t)ms_to_samples(params->ampRel)) {
    float p = t - len;
    float c = params->ampRel * (SAMPLE_RATE / 1000);
    return 1.0 - 1.0 * p / c;
  }
  else
    return 0.0;
}

static float gen_cmb_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
  return gen_ads_envelope(params, t, len) * gen_rel_envelope(params, t, len);
}

static float gen_signal(uint8_t shape, float phase) {
  switch (shape) {
    case SHAPE_SIN:
      return gen_sin(phase);
    case SHAPE_TRI:
      return gen_tri(phase);
    case SHAPE_SAW:
      return gen_saw(phase);
    case SHAPE_SQR:
      return gen_sqr(phase);
    case SHAPE_PUL:
      return gen_pul(phase);
    case SHAPE_NOI:
      return gen_noi(phase);
    case SHAPE_CAR:
      return gen_car(phase);
    case SHAPE_PLU:
      return gen_plu(phase);
    default:
      return 0;
  }
}

char ae_1osc_fill(AudioEvent* event, float* buffer, size_t len) {
  State1Osc* state = (State1Osc*)event->state;
  for(int i = 0; i < len; ++i) {
    buffer[i] = gen_cmb_envelope(state->params, state->time + i, state->length);
    buffer[i] = buffer[i] * state->volume / 128.0;
    buffer[i] = buffer[i] * gen_signal(state->params->shape, state->phase);

    // apply comb filter
    uint16_t oldPos =
      (state->combPos + COMB_BUFFER_SIZE - state->combOffset) % COMB_BUFFER_SIZE;
    state->combBuffer[state->combPos] =
      buffer[i] + state->combBuffer[oldPos] * state->combA;
    state->combPos = (state->combPos + 1) % COMB_BUFFER_SIZE;
    buffer[i] = state->combBuffer[state->combPos];

    // apply low pass filter
    state->filterOut = state->filterOut + state->filterA * (buffer[i] - state->filterOut);
    buffer[i] = state->filterOut;

    state->phase = fmod(state->phase + state->phaseStep, 1.0f);
  }
  state->time += len;
  return state->time < state->length + state->params->ampRel * 48;
} 

AudioEventInterface AE_1OSC_INTERFACE = {
  ae_1osc_fill,
  ae_state_destroy
};

static float freq_to_a(uint16_t freq) {
  float x = (2.0 * M_PI * freq / SAMPLE_RATE);
  return x / (x + 1);
}

static uint16_t freq_to_comb_offset(uint16_t freq) {
  if (freq == 0)
    return COMB_BUFFER_SIZE - 1;
  else
    return SAMPLE_RATE / (2 * freq);
}

AudioEvent* ae_1osc_create(float freq, uint32_t length, uint8_t volume, void* parameters) {
  State1Osc* state = malloc(sizeof(State1Osc));
  state->params = (Parameters1Osc*)parameters;
  state->volume = volume;
  state->phase = 0;
  state->phaseStep = freq / SAMPLE_RATE;
  state->time = 0;
  state->length = length;

  // comb filter state
  state->combA = pow(0.5, state->params->res * 0.5) - 1;
  state->combOffset = freq_to_comb_offset(state->params->filter);
  state->combPos = 0;
  memset(state->combBuffer, 0, sizeof(float) * COMB_BUFFER_SIZE);

  // lowpass filter state
  state->filterA = freq_to_a(state->params->filter);
  state->filterOut = 0.0;

  AudioEvent* event = malloc(sizeof(AudioEvent));
  event->interface = &AE_1OSC_INTERFACE;
  event->state = state;
  return event;
}
