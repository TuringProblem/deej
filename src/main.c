#include "noteapp.h"
#include <ncurses.h>

int main() {
  init_ncurses();

  display_deej_ascii_art(stdscr);

  clear();
  refresh();

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  WINDOW *title_win = newwin(1, max_x, 0, 0);
  WINDOW *main_win = newwin(max_y - 2, max_x - 20, 1,
                            20); // Reduced width for sidebar TODO: if user has
                                 // TODO -> allocate for the UI space
  WINDOW *status_win = newwin(1, max_x, max_y - 1, 0);
  WINDOW *sidebar_win = newwin(max_y - 2, 20, 1, 0);

  draw_title_bar(title_win, "DEEJ");

  Document doc;
  init_document(&doc);

  mvwprintw(main_win, 1, 1, "Enter filename: ");
  wrefresh(main_win);

  echo();
  wgetnstr(main_win, doc.filename, MAX_FILENAME);
  noecho();

  wclear(main_win);
  box(main_win, 0, 0);
  mvwprintw(main_win, 0, 2, " %s ", doc.filename);
  wrefresh(main_win);

  keypad(main_win, TRUE);

  bool quit = false;
  while (!quit) {
    render_document(main_win, &doc);
    draw_status_bar(status_win, &doc);
    draw_sidebar(sidebar_win);

    int ch = wgetch(main_win);
    handle_input(ch, &doc, &quit);

    wmove(main_win, doc.cursor_y + 1 - doc.scroll_offset, doc.cursor_x + 1);
    wrefresh(main_win);
  }

  free_document(&doc);
  cleanup_ncurses();
  return 0;
}

void draw_sidebar(WINDOW *win) {
  wclear(win);
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " Files ");
  // Implement file listing in the sidebar
  wrefresh(win);
}
