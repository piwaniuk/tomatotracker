#ifndef WIDGETS_H
#define WIDGETS_H

#include "stdlib.h"
#include "iterator.h"

typedef struct {
  int y, x;
} ScreenPos;

void print_header();
void status_message(const char*);

ScreenPos get_screen_pos();
void move_screen_pos(ScreenPos screenPos);

void* list_choice_widget(BidirectionalIterator* iter, char* (repr)(void*));
bool slug_edit_widget(ScreenPos screenPos, char* text, size_t len);

#endif
