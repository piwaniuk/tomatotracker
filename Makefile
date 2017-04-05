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

CFLAGS=-Wall -Werror -std=c99
CXXFLAGS=-Wall -Werror -std=c++14
LDFLAGS=-lncurses -lSDL2

# Build logic

DBG_SUFFIX=-dbg
TRACKER_REL=$(BIN_DIR)/$(EXECUTABLE)
TRACKER_DBG=$(BIN_DIR)/$(EXECUTABLE)$(DBG_SUFFIX)

SUB_DIRS=$(BIN_DIR) $(SRC_DIR) $(OBJ_DIR)

OBJECTS_REL=$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(MODULES)))
OBJECTS_DBG=$(addprefix $(OBJ_DIR)/, $(addsuffix $(DBG_SUFFIX).o, $(MODULES)))

DEPENDS_REL=$(addprefix $(DEP_DIR)/, $(addsuffix .depend, $(MODULES)))
DEPENDS_DBG=$(addprefix $(DEP_DIR)/, $(addsuffix $(DBG_SUFFIX).depend, $(MODULES)))
DEPENDS=$(DEPENDS_REL) $(DEPENDS_DBG)

# high-level rules

all: release

release: CFLAGS+= -Os
release: CXXFLAGS+= -Os
release: LDFLAGS+= -s
release: $(SUB_DIRS) $(TRACKER_REL)

debug: CFLAGS+= -g
debug: CXXFLAGS+= -g
debug: $(SUB_DIRS) $(TRACKER_DBG)

depend: $(DEPENDS)

clean:
	-$(RM) $(TRACKER_REL) $(TRACKER_DBG)
	-$(RM) $(OBJECTS_REL)
	-$(RM) $(OBJECTS_DBG)
	-$(RM) $(DEPENDS)
	-$(RM) -d $(OBJ_DIR)
	-$(RM) -d $(BIN_DIR)
	-$(RM) -d $(DEP_DIR)

.PHONY: clean depend debug release

# directory initialization rules

# creating a file later would update directory timestamp
$(DEP_DIR):
	@echo -e "\tDEPEND\t" $@
	@mkdir -p $@
	@touch $(DEPENDS)

$(SUB_DIRS):
	@echo -e "\tMKDIR\t" $@
	@mkdir -p $@

# compilation rules

$(OBJ_DIR)/%.o $(OBJ_DIR)/%$(DBG_SUFFIX).o: $(SRC_DIR)/%.c
	@echo -e "\tCC\t" $@
	@$(CC) $< -c $(CFLAGS) -o $@

$(OBJ_DIR)/%.o $(OBJ_DIR)/%$(DBG_SUFFIX).o: $(SRC_DIR)/%.cpp
	@echo -e "\tCXX\t" $@
	@$(CXX) $< -c $(CXXFLAGS) -o $@

# linking rules

$(TRACKER_REL): $(OBJECTS_REL)
	@echo -e "\tCCLD\t" $@
	@$(CXX) $^ $(LDFLAGS) -o $@

$(TRACKER_DBG): $(OBJECTS_DBG)
	@echo -e "\tCCLD\t" $@
	@$(CXX) $^ $(LDFLAGS) -o $@

# rules for generating dependency information

$(DEP_DIR)/%.depend $(DEP_DIR)/%$(DBG_SUFFIX).depend: $(SRC_DIR)/%.c $(DEP_DIR)
	@$(CC) $(CFLAGS) $< -MT $(patsubst $(DEP_DIR)/%.depend, $(OBJ_DIR)/%.o, $@) -MM > $@

$(DEP_DIR)/%.depend $(DEP_DIR)/%$(DBG_SUFFIX).depend: $(SRC_DIR)/%.cpp $(DEP_DIR)
	@$(CXX) $(CXXFLAGS) $< -MT $(patsubst $(DEP_DIR)/%.depend, $(OBJ_DIR)/%.o, $@) -MM > $@

# included dependencies are automatically updated with the rules above
-include $(DEPENDS)
