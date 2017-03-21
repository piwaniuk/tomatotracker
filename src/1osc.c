#include "1osc.h"
#include "audio_event.h"
#include "audio_event_interface.h"
#include "synth.h"

enum {STAGE_ATTACK, STAGE_DECAY, STAGE_SUSTAIN, STAGE_RELEASE, STAGE_OFF};

typedef struct {
  Parameters1Osc* params;
  uint16_t phase;
  uint16_t phaseStep;
  uint32_t time;
} State1Osc;

uint16_t gen_ads_envelope(Parameters1Osc* params, uint32_t t) {
  if (t < params->ampAtt)
    return 65535 * t / params->ampAtt;
  else if (t < params->ampAtt + params->ampDec)
    return 65535 - (65535 - params->ampSus) * (t - params->ampAtt) / params->ampDec;
  else
    return params->ampSus;
}

uint16_t gen_rel_envelope(Parameters1Osc* params, uint32_t t) {
  if (t < 3000)
    return 65535;
  else if (t < 3000 + params->ampRel)
    return 65535 - 65535 * (t - 3000) / params->ampRel;
  else
    return 0;
}

uint16_t gen_cmb_envelope(Parameters1Osc* params, uint32_t t) {
  return gen_ads_envelope(params, t) * gen_rel_envelope(params, t) / 65536;
}

char ae_1osc_fill(AudioEvent* event, sample_t* buffer, size_t len) {
  State1Osc* state = (State1Osc*)event->state;
  for(int i = 0; i < len; ++i) {
    buffer[i] = gen_cmb_envelope(state->params, state->time + i) / 4;
    buffer[i] = buffer[i] * (gen_triangle(state->phase) - SAMPLE_0) / 65536 + SAMPLE_0;
    state->phase += state->phaseStep;
  }
  state->time += len;
  return state->time < 3000 + state->params->ampRel;
} 

AudioEventInterface AE_1OSC_INTERFACE = {
  ae_1osc_fill,
  ae_state_destroy
};

AudioEvent* ae_1osc_create(int freq, void* parameters) {
  State1Osc* state = malloc(sizeof(State1Osc));
  state->params = (Parameters1Osc*)parameters;
  state->phase = 0;
  state->phaseStep = freq * 65536 / SAMPLE_RATE;
  state->time = 0;
  
  AudioEvent* event = malloc(sizeof(AudioEvent));
  event->interface = &AE_1OSC_INTERFACE;
  event->state = state;
  return event;
}
