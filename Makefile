all: tracker

OBJECTS=tracker.o widgets.o audio_output.o iterator.o list.o pattern_screen.o \
  instrument.o audio_event.o sequencer.o synth.o 1osc.o \
  audio_event_interface.c
CFLAGS=-O2 -Werror
LDFLAGS=-lncurses -lSDL2

tracker: $(OBJECTS)

clean:
	-$(RM) tracker
	-$(RM) *.o
	-$(RM) depend

depend:
	gcc -M *.c > depend.inc

.PHONY: clean depend

-include depend.inc

