#include "noteapp.h"

void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
}

void cleanup_ncurses() {
    endwin();
}

void draw_title_bar(WINDOW* win, const char* title) {
    int width = getmaxx(win);
    wattron(win, COLOR_PAIR(1));
    mvwhline(win, 0, 0, ' ', width);
    mvwprintw(win, 0, (width - strlen(title)) / 2, "%s", title);
    wattroff(win, COLOR_PAIR(1));
    wrefresh(win);
}

void draw_status_bar(WINDOW* win, Document* doc) {
    int width = getmaxx(win);
    int height = getmaxy(win);
    
    // Clear the status bar
    wmove(win, height - 1, 0);
    wclrtoeol(win);
    
    int color_pair;
    const char* mode_str;
    switch (doc->mode) {
        case MODE_NORMAL: 
            color_pair = 2; 
            mode_str = "NORMAL"; 
            break;
        case MODE_INSERT: 
            color_pair = 3; 
            mode_str = "INSERT"; 
            break;
        case MODE_VISUAL: 
            color_pair = 1; 
            mode_str = "VISUAL"; 
            break;
        default: 
            color_pair = 2; 
            mode_str = "UNKNOWN"; 
            break;
    }
    
    wattron(win, COLOR_PAIR(color_pair));
    
    mvwprintw(win, height - 1, 0, " %s ", mode_str);
    
    mvwprintw(win, height - 1, width - 20, "%s", doc->filename);
    mvwprintw(win, height - 1, width - 10, "%d,%d", doc->cursor_y + 1, doc->cursor_x + 1);
    
    wattroff(win, COLOR_PAIR(color_pair));
    wrefresh(win);
}
