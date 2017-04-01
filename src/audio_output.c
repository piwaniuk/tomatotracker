#include <limits.h>

#include <SDL2/SDL.h>

#include "audio_event.h"
#include "audio_output.h"
#include "list.h"
#include "iterator.h"

void aoc_add_event(AudioOutputContext* aoc, AudioEvent* event) {
  pthread_mutex_lock(&aoc->events_mutex);
  BidirectionalIterator* iter = list_iterator(aoc->events);
  iter_insert(iter, event);
  iter_destroy(iter);
  pthread_mutex_unlock(&aoc->events_mutex);
}

AudioOutputContext* aoc_create(Sequencer* seq) {
  AudioOutputContext* aoc = malloc(sizeof(AudioOutputContext));
  aoc->events = list_create();
  aoc->seq = seq;
  pthread_mutex_init(&aoc->events_mutex, NULL);
  return aoc;
}

void aoc_destroy(AudioOutputContext* aoc) {
  pthread_mutex_destroy(&aoc->events_mutex);
  list_destroy(aoc->events);
  free(aoc);
}

void mix_buffers(BidirectionalIterator* bi, size_t len, sample_t* out) {
  // zero the output
  int32_t* mix = malloc(sizeof(int32_t) * len);
  uint16_t count = 0;
  
  for(int j = 0; j < len; ++j)
    mix[j] = SAMPLE_0;
  
  // sum the inputs
  for(; !iter_is_end(bi); iter_next(bi)) {
    sample_t* in = (sample_t*)iter_get(bi);
    for(int j = 0; j < len; ++j)
      mix[j] += in[j];
    ++count;
  }

  for(int j = 0; j < len; ++j) {
    mix[j] -= count * SAMPLE_0;
    if (mix[j] < 0)
      out[j] = 0;
    else if (mix[j] > 65535)
      out[j] = 65535;
    else
      out[j] = (sample_t)mix[j];
  }

  free(mix);
}

void audio_callback(void* userData, Uint8* s, int len) {
  size_t sampleLen = len / sizeof(sample_t);
  
  // push forward the sequencer
  AudioOutputContext* aoc = (AudioOutputContext*)userData;
  
  {
    pthread_mutex_lock(&aoc->events_mutex);
    BidirectionalIterator* i = list_iterator(aoc->events);
    seq_forward(aoc->seq, sampleLen, i);
    pthread_mutex_unlock(&aoc->events_mutex);
    iter_destroy(i);
  }
  
  // prepare buffers 
  LinkedList* bufferList = list_create();
  
  // process audio events
  {
    pthread_mutex_lock(&aoc->events_mutex);
    BidirectionalIterator* i = list_iterator(aoc->events);
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
    pthread_mutex_unlock(&aoc->events_mutex);
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
  spec.samples = 2048;
  spec.callback = audio_callback;
  spec.userdata = context;
  if ( SDL_OpenAudio(&spec, NULL) < 0 ) {
    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    exit(2);
  }
  SDL_PauseAudio(0);
}

void audio_output_initialize(AudioOutputContext* aoc) {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    #ifdef DEBUG
    exit(1);
    #endif
  }
  init_audio(aoc);
}

void audio_output_finalize(void) {
  SDL_Quit();
}
