#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <ncurses.h>

#include "widgets.h"
#include "iterator.h"

void print_header() {
  printw("Song Title\n");
  printw("stopped\n");
}

void status_message(const char* message) {
  move(23, 0);
  insertln();
  printw(message);
}


ScreenPos get_screen_pos() {
  ScreenPos ret;
  getyx(stdscr, ret.y, ret.x);
  return ret;
}

void move_screen_pos(ScreenPos screenPos) {
  move(screenPos.y, screenPos.x);
}

void* list_choice_widget(BidirectionalIterator* iter, char* (repr)(void*)) {
  bool finished = false;
  void* ret = NULL;
  int x, y;
  getyx(stdscr, y, x);
  while (!finished) {
    char* itemRepr = repr(iter_get(iter));
    move(y, x);
    printw(itemRepr);
    int ch = getch();
    switch (ch) {
      case '\n':
        ret = iter_get(iter);
        finished = true;
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
        finished = true;
        break;
    }
  }
  return ret;
}

bool slug_edit_widget(ScreenPos screenPos, char* text, size_t len) {
  bool finished = false;
  bool ret;
  size_t pos = strlen(text);
  char* buffer = malloc(len + 1);
  snprintf(buffer, len + 1, "%s", text);
  move_screen_pos(screenPos);
  for(int i = 0; i < len; ++i)
    addch(' ');
  move_screen_pos(screenPos);
  printw(text);
  while (!finished) {
    int ch = getch();
    if (ch == KEY_BACKSPACE && pos > 0) {
      --pos;
      buffer[pos] = '\0';
      printw("\b \b");
    }
    else if (ch == '\n' && pos > 0) {
      strcpy(text, buffer);
      ret = true;
      finished = true;
    }
    else if (ch == 27) {
      ret = false;
      finished = true;
    }
    else if ((ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) && pos < len) {
      buffer[pos] = ch;
      buffer[pos + 1] = '\0';
      ++pos;
      addch(ch);
    }
  }
  free(buffer);
  return ret;
}
