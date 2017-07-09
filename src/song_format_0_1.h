#ifndef SONG_FORMAT_H
#define SONG_FORMAT_H

#include <map>

#include "const_map.h"
#include "song_io.h"

template<>
struct SongFormat<0x00000001> {
  static const size_t INSTRUMENT_BASE_SIZE = 44;
  
  using InstrParamSizes =
    ConstMapping<uint16_t, uint32_t,
      CMEntry<INSTRUMENT_TYPE_1OSC, 9>>;

  // 1OSC parameters field offsets
  static const size_t PARAMS_1OSC_SHAPE_OFF = 0;
  static const size_t PARAMS_1OSC_A_OFF = 1;
  static const size_t PARAMS_1OSC_D_OFF = 3;
  static const size_t PARAMS_1OSC_S_OFF = 5;
  static const size_t PARAMS_1OSC_R_OFF = 7;
  // instrument field offsets
  static const size_t INS_NAME_OFF = 0;
  static const size_t INS_DESCR_OFF = 6;
  static const size_t INS_VOLUME_OFF = 38;
  static const size_t INS_TYPE_OFF = 39;
  static const size_t INS_PARAMS_OFF = 41;
  static const size_t INS_BASE_SIZE = 41;

  // pattern step field offsets
  static const size_t PAT_STEP_NOTE_OFF = 0;
  static const size_t PAT_STEP_LEN_OFF = 1;
  static const size_t PAT_STEP_INS_OFF = 2;
  static const size_t PAT_STEP_CMD1_OFF = 6;
  static const size_t PAT_STEP_CMD2_OFF = 10;

  // pattern field offsets
  static const size_t PAT_NAME_OFF = 0;
  static const size_t PAT_DESCR_OFF = 6;
  static const size_t PAT_STEPS_OFF = 38;
  static const size_t PAT_HEADER_SIZE = 38;
  static const size_t PAT_STEP_SIZE = 14;

  // phrase field offsets
  static const size_t PHR_NAME_OFF = 0;
  static const size_t PHR_DESCR_OFF = 6;
  static const size_t PHR_STEPS_OFF = 38;
  static const size_t PHR_LENGTH_SIZE = 2;
  static const size_t PHR_HEADER_SIZE = 38;
  static const size_t PHR_STEP_SIZE = 4;

  // track field offsets
  static const size_t TRK_LAST_POS_SIZE = 2;
  static const size_t TRK_ENTRY_SIZE = 4;
};

#endif

