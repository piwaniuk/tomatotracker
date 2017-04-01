#include "1osc.h"
#include "audio_event.h"
#include "audio_event_interface.h"
#include "synth.h"

enum {STAGE_ATTACK, STAGE_DECAY, STAGE_SUSTAIN, STAGE_RELEASE, STAGE_OFF};

typedef struct {
  Parameters1Osc* params;
  uint8_t volume;
  uint16_t phase;
  uint16_t phaseStep;
  uint32_t time;
  uint32_t length;
} State1Osc;

uint16_t gen_ads_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
  if (t < params->ampAtt)
    return 65535 * t / params->ampAtt;
  else if (t < params->ampAtt + params->ampDec)
    return 65535 - (65535 - params->ampSus) * (t - params->ampAtt) / params->ampDec;
  else
    return params->ampSus;
}

uint16_t gen_rel_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
  if (t < len)
    return 65535;
  else if (t < len + params->ampRel)
    return 65535 - 65535 * (t - len) / params->ampRel;
  else
    return 0;
}

uint16_t gen_cmb_envelope(Parameters1Osc* params, uint32_t t, uint32_t len) {
  return gen_ads_envelope(params, t, len) * gen_rel_envelope(params, t, len) / 65536;
}

char ae_1osc_fill(AudioEvent* event, sample_t* buffer, size_t len) {
  State1Osc* state = (State1Osc*)event->state;
  for(int i = 0; i < len; ++i) {
    buffer[i] = gen_cmb_envelope(state->params, state->time + i, state->length);
    buffer[i] = buffer[i] * state->volume / 128;
    buffer[i] = buffer[i] * (gen_saw(state->phase) - SAMPLE_0) / 65536 + SAMPLE_0;
    state->phase += state->phaseStep;
  }
  state->time += len;
  return state->time < state->length * 2 + state->params->ampRel;
} 

AudioEventInterface AE_1OSC_INTERFACE = {
  ae_1osc_fill,
  ae_state_destroy
};

AudioEvent* ae_1osc_create(int freq, uint32_t length, uint8_t volume, void* parameters) {
  State1Osc* state = malloc(sizeof(State1Osc));
  state->params = (Parameters1Osc*)parameters;
  state->volume = volume;
  state->phase = 0;
  state->phaseStep = freq * 65536 / SAMPLE_RATE;
  state->time = 0;
  state->length = length;
  
  AudioEvent* event = malloc(sizeof(AudioEvent));
  event->interface = &AE_1OSC_INTERFACE;
  event->state = state;
  return event;
}
