#include "sequencer.h"
#include "audio_event.h"
#include "synth.h"

AudioEvent* step_to_event(PatternStep* step) {
  int freq = note_to_freq(step->n);
  return ae_freq_create(freq);
}

static void seq_forward_ticks(Sequencer* seq, size_t hold, size_t ticks, BidirectionalIterator* events) {
  while (ticks) {
    seq->tick = (seq->tick + 1) % 16;
    if (seq->pattern->steps[seq->tick].n != 0) {
      AudioEvent* event = step_to_event(&(seq->pattern->steps[seq->tick]));
      iter_insert(events, ae_hold_create(hold, event));
    }
    hold += seq->spt;
    --ticks;
  }
}

void seq_forward(Sequencer* seq, size_t len, BidirectionalIterator* events) {
  size_t hold = seq->spt - seq->sample;
  size_t ticks = (seq->sample + len) / seq->spt;
  seq_forward_ticks(seq, hold, ticks, events);
  seq->sample = (seq->sample + len) % seq->spt; 
}

void seq_play_pattern(Sequencer* seq, Pattern* pattern) {
  seq->pattern = pattern;
  seq->mode = PLAY_MODE_PATTERN;
}
