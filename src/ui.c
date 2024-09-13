#include "noteapp.h"

void init_ncurses() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
}


void cleanup_ncurses() { endwin(); }

void draw_title_bar(WINDOW* win, const char* title) {
  int width = getmaxx(win);
  wattron(win, COLOR_PAIR(1));
  mvwhline(win, 0, 0, ' ', width);
  mvwprintw(win, 0, (width - strlen(title)) / 2, "%s", title);
  wattroff(win, COLOR_PAIR(1));
  wrefresh(win);
}
