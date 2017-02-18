all: tracker

OBJECTS=tracker.o widgets.o audio_output.o iterator.o list.o pattern_screen.o \
  instrument.c audio_event.c sequencer.c synth.c 1osc.c \
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

