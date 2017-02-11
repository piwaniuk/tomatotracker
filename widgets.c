#include <stddef.h>
#include <stdbool.h>

#include <ncurses.h>

#include "widgets.h"
#include "iterator.h"

void* list_choice_widget(BidirectionalIterator* iter, char* (repr)(void*)) {
  char finished = FALSE;
  int x, y;
  getyx(stdscr, y, x);
  while (!finished) {
    char* itemRepr = repr(iter_get(iter));
    move(y, x);
    printw(itemRepr);
    int ch = getch();
    switch (ch) {
      case '\n':
        finished = TRUE;
        break;
      case KEY_DOWN:
        if (!iter_is_last(iter))
          iter_next(iter);
        break;
      case KEY_UP:
        if (!iter_is_first(iter))
          iter_prev(iter);
        break;
      case 27:
        finished = TRUE;
        break;
    }
  }
  return iter_get(iter);
}

