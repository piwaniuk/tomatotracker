#include "sequencer.h"
#include "audio_event.h"
#include "synth.h"
#include "1osc.h"

AudioEvent* step_to_event(PatternStep* step) {
  Instrument* instr = step->inst;
  int freq = note_to_freq(step->n);
  if (instr->type == INSTRUMENT_1OSC)
    return ae_1osc_create(freq, instr->parameters);
  else
    return ae_freq_create(freq);
}

static void seq_trigger_tick(Sequencer* seq, size_t hold, BidirectionalIterator* events) {
  if (seq->pattern->steps[seq->tick].n != 0) {
    AudioEvent* event = step_to_event(&(seq->pattern->steps[seq->tick]));
    iter_insert(events, ae_hold_create(hold, event));
  }
  // TODO: support different pattern lengths
  seq->tick = (seq->tick + 1) % 16;
}

static void seq_forward_ticks(Sequencer* seq, size_t hold, size_t ticks, BidirectionalIterator* events) {
  while (ticks) {
    seq_trigger_tick(seq, hold, events);
    hold += seq->spt;
    --ticks;
  }
}

void seq_forward(Sequencer* seq, size_t len, BidirectionalIterator* events) {
  if (seq->isPlaying) {
    // this triggers a note at the starting position
    if (seq->sample == 0)
      seq_trigger_tick(seq, 0, events);
    size_t hold = seq->spt - seq->sample;
    // do not trigger a note from final position
    size_t ticks = (len + seq->sample - 1) / seq->spt;
    seq_forward_ticks(seq, hold, ticks, events);
    seq->sample = (seq->sample + len) % seq->spt;
  }
}

void seq_play_pattern(Sequencer* seq, Pattern* pattern, size_t pos) {
  seq->pattern = pattern;
  seq->mode = PLAY_MODE_PATTERN;
  seq->tick = pos;
  seq->isPlaying = true;
}

bool seq_is_playing(Sequencer* seq) {
  return seq->isPlaying;
}

bool seq_pattern_mark(Sequencer* seq, Pattern* pattern, size_t pos) {
  return seq->isPlaying && seq->pattern == pattern && seq->tick == pos;
}

void seq_stop(Sequencer* seq) {
  seq->isPlaying = false;
}
