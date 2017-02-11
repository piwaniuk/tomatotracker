#ifndef WIDGETS_H
#define WIDGETS_H

#include "iterator.h"

void* list_choice_widget(BidirectionalIterator* iter, char* (repr)(void*));

#endif
