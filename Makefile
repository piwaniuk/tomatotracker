# Build configuration

BIN_DIR=bin
SRC_DIR=src
OBJ_DIR=obj
DEP_DIR=dep
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
DEPENDS=$(addprefix $(DEP_DIR)/, $(addsuffix .depend, $(MODULES)))

all: $(SUB_DIRS) $(TRACKER)

CFLAGS=-Wall -Werror -g -std=c99
CXXFLAGS=-Wall -Werror -g -std=c++14
LDFLAGS=-lncurses -lSDL2 -g

$(DEP_DIR):
	@echo -e "\tDEPEND\t" $@
	@mkdir -p $@
	@touch $(DEPENDS)

$(SUB_DIRS):
	@echo -e "\tMKDIR\t" $@
	@mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo -e "\tCC\t" $@
	@$(CC) $< -c $(CFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo -e "\tCXX\t" $@
	@$(CXX) $< -c $(CXXFLAGS) -o $@

$(TRACKER): $(OBJECTS)
	@echo -e "\tCCLD\t" $@
	@$(CXX) $^ $(LDFLAGS) -o $@

clean:
	-$(RM) $(TRACKER)
	-$(RM) $(OBJECTS)
	-$(RM) $(DEPENDS)
	-$(RM) -d $(OBJ_DIR)
	-$(RM) -d $(BIN_DIR)
	-$(RM) -d $(DEP_DIR)

$(DEP_DIR)/%.depend: $(SRC_DIR)/%.c $(DEP_DIR)
	@$(CC) $(CFLAGS) $< -MT $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $<) -MM > $@

$(DEP_DIR)/%.depend: $(SRC_DIR)/%.cpp $(DEP_DIR)
	@$(CXX) $(CXXFLAGS) $< -MT $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $<) -MM > $@

depend: $(DEPENDS)

.PHONY: clean depend

# included dependencies are automagically updated with the rules above
-include $(DEPENDS)

