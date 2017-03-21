#ifndef SBUFFER_H
#define SBUFFER_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct {
  char* start;
  char* end;
  size_t space;
} SBuffer;

static inline SBuffer sbuffer_init(size_t space) {
  char* buffer_start = malloc(space + 1);
  //for(int i = 0; i < space + 1; ++i)
  //  buffer_start[i] = '\0';
  return (SBuffer){buffer_start, buffer_start, space};
}

static inline void sbuffer_append_string(SBuffer* sbuffer, const char* string) {
  size_t len = strlen(string);
  if (len < sbuffer->space) {
    strcat(sbuffer->end, string);
    sbuffer->end += len;
    sbuffer->space -= len;
  }
  else {
    strncat(sbuffer->end, string, sbuffer->space);
    sbuffer->end += sbuffer->space;
    sbuffer->space = 0;
  }
}

static inline void sbuffer_append_char(SBuffer* sbuffer, char ch) {
  if (sbuffer->space > 0) {
    sbuffer->end[0] = ch;
    sbuffer->end[1] = '\0';
    ++sbuffer->end;
    --sbuffer->space;
  }
}

static inline int sbuffer_printf(SBuffer* sbuffer, const char* format, ...) {
  va_list args;
  va_start(args, format);
  int length = vsnprintf(sbuffer->end, sbuffer->space, format, args);
  va_end(args);
  if (length < 0) {
    sbuffer->end[0] = '\0';
  }
  else if (length < sbuffer->space) {
    sbuffer->end += length;
    sbuffer->space -= length;
  }
  else {
    sbuffer->end += sbuffer->space;
    sbuffer->space = 0;
  }
  return length;
}

static inline char* sbuffer_get(SBuffer* sbuffer) {
  return sbuffer->start;
}

static inline void sbuffer_free(SBuffer* sbuffer) {
  free(sbuffer->start);
  sbuffer->start = sbuffer->end = NULL;
}

#endif
