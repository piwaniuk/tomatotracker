#include "math.h"

#include "1osc.h"
#include "audio_event.h"
#include "audio_event_interface.h"
#include "synth.h"

enum {STAGE_ATTACK, STAGE_DECAY, STAGE_SUSTAIN, STAGE_RELEASE, STAGE_OFF};

typedef struct {
  Parameters1Osc* params;
  uint8_t volume;
  float phase;
  float phaseStep;
  uint32_t time;
  uint32_t length;
} State1Osc;

float gen_ads_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
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

float gen_rel_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
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

float gen_cmb_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
  return gen_ads_envelope(params, t, len) * gen_rel_envelope(params, t, len);
}

char ae_1osc_fill(AudioEvent* event, float* buffer, size_t len) {
  State1Osc* state = (State1Osc*)event->state;
  for(int i = 0; i < len; ++i) {
    buffer[i] = gen_cmb_envelope(state->params, state->time + i, state->length);
    buffer[i] = buffer[i] * state->volume / 128.0;
    buffer[i] = buffer[i] * gen_saw(state->phase);
    state->phase = fmod(state->phase + state->phaseStep, 1.0f);
  }
  state->time += len;
  return state->time < state->length + state->params->ampRel * 48;
} 

AudioEventInterface AE_1OSC_INTERFACE = {
  ae_1osc_fill,
  ae_state_destroy
};

AudioEvent* ae_1osc_create(float freq, uint32_t length, uint8_t volume, void* parameters) {
  State1Osc* state = malloc(sizeof(State1Osc));
  state->params = (Parameters1Osc*)parameters;
  state->volume = volume;
  state->phase = 0;
  state->phaseStep = freq / SAMPLE_RATE;
  state->time = 0;
  state->length = length;
  
  AudioEvent* event = malloc(sizeof(AudioEvent));
  event->interface = &AE_1OSC_INTERFACE;
  event->state = state;
  return event;
}
