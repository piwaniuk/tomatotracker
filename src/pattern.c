#include <stdio.h>
#include <string.h>

#include "pattern.h"

Pattern* pattern_create(const char* name) {
  Pattern* ret = malloc(sizeof(Pattern));
  snprintf(ret->identifier, 7, "%s", name);
  ret->description[0] = '\0';
  ret->length = 16;
  for(int i = 0; i < 20; ++i)
    ret->steps[i] = (PatternStep){0, NULL, 0, 0};
  return ret;
}

void pattern_destroy(Pattern* pattern) {
  free(pattern);
}

int pattern_cmp_name(void* v1, void* v2) {
  Pattern* p1 = (Pattern*)v1;
  Pattern* p2 = (Pattern*)v2;
  return strcmp(p1->identifier, p2->identifier);
}

char* pattern_repr(void* v) {
  Pattern* pattern = (Pattern*)v;
  return pattern->identifier;
}
