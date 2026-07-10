#include "./components/editor/enums/modes.hpp"
#include <curses.h>
#include <iostream>

void displayScreen() {
  char c;
  while (true) {
    initscr();
    cbreak();
    noecho();

    WINDOW *menu = newwin(10, 30, 2, 5);
    curs_set(0);

    box(stdscr, 0, 0);

    box(menu, 2, 2);
    mvprintw(1, 1, "hello world!");

    refresh();
    wrefresh(menu);
    c = getch();

    switch (c) {
    case:
      'q' : break;
    }
  }
  endwin();
}

int main() {
  std::cout << "hello world!" << std::endl;

  EditorMode mode = EditorMode::NORMAL;

  displayScreen();
  std::cout << static_cast<int>(mode) << std::endl;
  return 0;
}
