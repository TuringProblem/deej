#include "noteapp.h"

int main() {
    init_ncurses();

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    WINDOW *title_win = newwin(1, maxY, 0, 0);
    WINDOW *main_win = newwin(maxY - 1, maxX, 1, 0);

    draw_title_bar(title_win, "Note-Taking App");

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

    int ch;
    while ((ch = wgetch(main_win)) != 'q') {
        handle_input(ch, &doc);
        render_document(main_win, &doc);
    }

    free_document(&doc);
    cleanup_ncurses();
    return 0;
}
