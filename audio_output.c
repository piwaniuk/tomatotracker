#include <limits.h>

#include <SDL2/SDL.h>

#include "audio_event.h"
#include "audio_output.h"
#include "list.h"
#include "iterator.h"

AudioOutputContext* AOC;

void audio_add_event(AudioEvent* event) {
  BidirectionalIterator* iter = list_iterator(AOC->events);
  iter_insert(iter, event);
  iter_destroy(iter);
}

AudioOutputContext* aoc_create(Sequencer* seq) {
  AudioOutputContext* aoc = malloc(sizeof(AudioOutputContext));
  aoc->events = list_create();
  aoc->seq = seq;
  return aoc;
}

void aoc_destroy(AudioOutputContext* aoc) {
  
}

void audio_add_event_freq(int freq) {
  audio_add_event(ae_freq_create(freq));
}

void mix_buffers(BidirectionalIterator* bi, size_t len, sample_t* out) {
  // zero the output
  for(int j = 0; j < len; ++j) {
    out[j] = SAMPLE_0;
  }
  
  // sum the inputs
  for(; !iter_is_end(bi); iter_next(bi)) {
    sample_t* in = (sample_t*)iter_get(bi);
    for(int j = 0; j < len; ++j) {
      out[j] += in[j] - SAMPLE_0;
    }
  }
  
  // TODO: clipping
  /*for(int j = 0; j < len; ++j) {
    if (mix < SAMPLE_MIN)
      mix = SAMPLE_MIN;
    else if (mix > SAMPLE_MAX)
      mix = SAMPLE_MAX;
    out[i] = mix;
  }*/
}

void audio_callback(void* userData, Uint8* s, int len) {
  size_t sampleLen = len / sizeof(sample_t);
  
  // push forward the sequencer
  AudioOutputContext* context = (AudioOutputContext*)userData;
  
  {
    BidirectionalIterator* i = list_iterator(context->events);
    seq_forward(context->seq, sampleLen, i);
    iter_destroy(i);
  }
  
  // prepare buffers 
  LinkedList* bufferList = list_create();
  
  // process audio events
  {
    BidirectionalIterator* i = list_iterator(context->events);
    BidirectionalIterator* bi = list_iterator(bufferList);
    while (!iter_is_end(i)) {
      AudioEvent* event = (AudioEvent*)iter_get(i);
      sample_t* buffer = malloc(sizeof(sample_t) * sampleLen);
      if (!ae_fill(event, (sample_t*)buffer, sampleLen)) {
        // remove the event
        iter_remove(i);
        ae_destroy(event);
      }
      else
        iter_next(i);
      iter_insert(bi, buffer);
    }
    iter_destroy(i);
    iter_destroy(bi);
  }
  
  // mix created buffers
  {
    BidirectionalIterator* bi = list_iterator(bufferList);
    mix_buffers(bi, sampleLen, (sample_t*)s);
    iter_destroy(bi);
  }
  list_destroy(bufferList);
}

static void init_audio(AudioOutputContext* context) {
  SDL_AudioSpec spec;
  spec.freq = 48000;
  spec.format = AUDIO_U16;
  spec.channels = 1;
  spec.samples = 1024;
  spec.callback = audio_callback;
  spec.userdata = context;
  if ( SDL_OpenAudio(&spec, NULL) < 0 ) {
    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    exit(2);
  }
  SDL_PauseAudio(0);
}

void audio_output_initialize(Sequencer* seq) {
  AOC = aoc_create(seq);
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    #ifdef DEBUG
    exit(1);
    #endif
  }
  init_audio(AOC);
}

void audio_output_finalize(void) {
  SDL_Quit();
  aoc_destroy(AOC);
}
