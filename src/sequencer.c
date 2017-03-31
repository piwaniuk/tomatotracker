#include "sequencer.h"
#include "audio_event.h"
#include "synth.h"
#include "1osc.h"

// TODO: this function does not seem to belong here after all
AudioEvent* step_to_event(PatternStep* step) {
  Instrument* instr = step->inst;
  int freq = note_to_freq(step->n);
  if (instr->type == INSTRUMENT_TYPE_1OSC)
    return ae_1osc_create(freq, instr->parameters);
  else
    return ae_freq_create(freq);
}

static Pattern** seq_feed_pattern(Sequencer* seq) {
  Pattern** ret = malloc(sizeof(Pattern*) * 2);
  ret[0] = seq->data;
  ret[1] = NULL;
  return ret;
}

static Pattern** seq_feed_phrase(Sequencer* seq) {
  Phrase* phrase = (Phrase*)seq->data;
  Pattern** ret = malloc(sizeof(Pattern*) * 2);
  ret[0] = phrase->patterns[seq->patternStep];
  ret[1] = NULL;
  return ret;
}

static Pattern** seq_feed_song(Sequencer* seq) {
  Pattern** ret = malloc(sizeof(Pattern*) * (TRACK_COUNT + 1));
  Song* song = (Song*)seq->data;
  int next = 0;
  for(int i = 0; i < TRACK_COUNT; ++i) {
    // use tail to determine current pattern and position in it
    uint16_t phrasePos = seq->patternStep - song->tracks[i][seq->patternStep].tail;
    Phrase* phrase = song->tracks[i][phrasePos].phrase;
    if (phrase != NULL) {
      uint16_t phraseStep = seq->patternStep - phrasePos;
      Pattern* pattern = phrase->patterns[phraseStep];
      if (pattern != NULL) {
        ret[next] = pattern;
        ++next;
      }
    }
  }
  ret[next] = NULL;
  return ret;
}

/**
 * Create source of patterns depending on play mode
 */
static Pattern** seq_feed_create(Sequencer* seq) {
  // play mode determines pattern feed interface
  if (seq->mode == PLAY_MODE_PATTERN)
    return seq_feed_pattern(seq);
  else if (seq->mode == PLAY_MODE_PHRASE)
    return seq_feed_phrase(seq);
  else
    return seq_feed_song(seq);
}

static void seq_next_tick(Sequencer* seq) {
  // TODO: support different pattern lengths
  seq->tick += 1;
  if (seq->tick == 16) {
    seq->patternStep += 1;
    seq->tick = 0;
    if (seq->mode == PLAY_MODE_PHRASE) {
      Phrase* phrase = (Phrase*)seq->data;
      if (seq->patternStep >= phrase->length)
        seq->patternStep = 0;
    }
    else if (seq->mode == PLAY_MODE_SONG) {
      Song* song = (Song*)seq->data;
      if (seq->patternStep >= song->length)
        seq->patternStep = 0;
    }
  }
}

static void seq_trigger_tick(Sequencer* seq, size_t hold, BidirectionalIterator* events) {
  Pattern** patterns = seq_feed_create(seq);
  for(int i = 0; patterns[i] != NULL; ++i) {
    if (patterns[i]->steps[seq->tick].n != 0) {
      AudioEvent* event = step_to_event(&(patterns[i]->steps[seq->tick]));
      iter_insert(events, ae_hold_create(hold, event));
    }
  }
  free(patterns);
  seq_next_tick(seq);
}

void seq_forward(Sequencer* seq, size_t len, BidirectionalIterator* events) {
  if (seq->isPlaying) {
    // this triggers a note at the starting position
    if (seq->sample == 0)
      seq_trigger_tick(seq, 0, events);
    size_t hold = seq->spt - seq->sample;
    // do not trigger a note from final position
    size_t ticks = (len + seq->sample - 1) / seq->spt;
    while (ticks) {
      seq_trigger_tick(seq, hold, events);
      hold += seq->spt;
      --ticks;
    }
    seq->sample = (seq->sample + len) % seq->spt;
  }
}

void seq_play_pattern(Sequencer* seq, Pattern* pattern, size_t pos) {
  seq->data = pattern;
  seq->mode = PLAY_MODE_PATTERN;
  seq->tick = pos;
  seq->isPlaying = true;
}

void seq_play_phrase(Sequencer* seq, Phrase* phrase, size_t pos) {
  seq->data = phrase;
  seq->mode = PLAY_MODE_PHRASE;
  seq->tick = 0;
  seq->patternStep = pos >= phrase->length ? 0 : pos;
  if (seq->patternStep >= phrase->length)
    seq->patternStep = 0;
  seq->isPlaying = true;
}

void seq_play_song(Sequencer* seq, Song* song, size_t pos) {
  seq->data = song;
  seq->mode = PLAY_MODE_SONG;
  seq->tick = 0;
  seq->patternStep = pos >= song->length ? 0 : pos;
  seq->isPlaying = true;
}

bool seq_is_playing(Sequencer* seq) {
  return seq->isPlaying;
}

uint8_t seq_play_mode(Sequencer* seq) {
  return (uint8_t)seq->mode;
}

bool seq_pattern_mark(Sequencer* seq, Pattern* pattern, size_t pos) {
  if (seq->isPlaying && seq->tick == pos) {
    Pattern** patterns = seq_feed_create(seq);
    bool found = false;
    for(int i = 0; patterns[i] != NULL && !found; ++i)
      found = (patterns[i] == pattern);
    free(patterns);
    return found;
  }
  else
    return false;
}

void seq_stop(Sequencer* seq) {
  seq->isPlaying = false;
}
