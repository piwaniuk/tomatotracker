all: tracker

OBJECTS=tracker.o widgets.o audio_output.o iterator.o list.o pattern_screen.o \
  instrument.o audio_event.o sequencer.o synth.o 1osc.o song.o \
  audio_event_interface.c song_screen.o phrase_screen.o
CFLAGS=-Werror -g
LDFLAGS=-lncurses -lSDL2 -g

tracker: $(OBJECTS)

clean:
	-$(RM) tracker
	-$(RM) *.o
	-$(RM) depend

depend:
	gcc -M *.c > depend.inc

.PHONY: clean depend

-include depend.inc

