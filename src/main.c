#include "noteapp.h"

int main() {
  init_ncurses();

  // Display the ASCII art
  display_deej_ascii_art(stdscr);

  // Clear the screen after displaying the ASCII art
  clear();
  refresh();

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  WINDOW *title_win = newwin(1, max_x, 0, 0);
  WINDOW *main_win = newwin(max_y - 2, max_x - 20, 1, 20);
  WINDOW *status_win = newwin(1, max_x, max_y - 1, 0);
  WINDOW *sidebar_win = newwin(max_y - 2, 20, 1, 0);

  draw_title_bar(title_win, "DEEJ - Text Editor8=D");

  struct Document doc;
  init_document(&doc);

  // Prompt for filename
  mvwprintw(main_win, 1, 1, "Enter filename: ");
  wrefresh(main_win);
  echo();
  wgetnstr(main_win, doc.filename, MAX_FILENAME);
  noecho();

  wclear(main_win);
  box(main_win, 0, 0);
  keypad(main_win, TRUE);

  bool quit = false;
  render_document(main_win, &doc);
  draw_status_bar(status_win, &doc);
  draw_sidebar(sidebar_win);

  while (!quit) {
    int ch = wgetch(main_win);
    handle_input(ch, &doc, main_win, &quit);

    // Update status bar and sidebar when necessary
    draw_status_bar(status_win, &doc);
    // draw_sidebar(sidebar_win);
  }

  free_document(&doc);
  cleanup_ncurses();
  return 0;
}
