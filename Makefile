# Build configuration

BIN_DIR=bin
SRC_DIR=src
OBJ_DIR=obj
EXECUTABLE=tracker
MODULES=tracker widgets audio_output iterator list pattern_screen \
  instrument audio_event sequencer synth 1osc song \
  audio_event_interface song_screen phrase_screen pattern \
  tracker_field instrument_field pattern_field phrase_field song_io \
  song_writer song_reader instrument_screen song_options_screen

# Build logic

TRACKER=$(BIN_DIR)/$(EXECUTABLE)
SUB_DIRS=$(BIN_DIR) $(SRC_DIR) $(OBJ_DIR)
OBJECTS=$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(MODULES)))

all: $(BIN_DIR) $(TRACKER)

CFLAGS=-Wall -Werror -g -std=c99
CXXFLAGS=-Wall -Werror -g -std=c++17
LDFLAGS=-lncurses -lSDL2 -g

$(SUB_DIRS):
	@mkdir $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $< -c $(CXXFLAGS) -o $@

$(TRACKER): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	-$(RM) $(TRACKER)
	-$(RM) $(OBJECTS)
	-rmdir $(OBJ_DIR)
	-rmdir $(BIN_DIR)

depend:
	gcc -M $(SRC_DIR)/*.c > depend.inc

.PHONY: clean depend

-include depend.inc

