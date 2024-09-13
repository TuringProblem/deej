#include "noteapp.h"

void init_document(Document *doc) {
    doc->buffer.content = malloc(MAX_LINE_LENGTH);
    doc->buffer.length = 0;
    doc->buffer.capacity = MAX_LINE_LENGTH;
    doc->cursor_x = 0;
    doc->cursor_y = 0;
}

void free_document(Document *doc) {
    free(doc->buffer.content);
}

void insert_char(Document *doc, char c) {
    if (doc->buffer.length >= doc->buffer.capacity - 1) {
        doc->buffer.capacity *= 2;
        doc->buffer.content = realloc(doc->buffer.content, doc->buffer.capacity);
    }

    memmove(&doc->buffer.content[doc->cursor_x + 1], &doc->buffer.content[doc->cursor_x], doc->buffer.length - doc->cursor_x);
    doc->buffer.content[doc->cursor_x] = c;
    doc->buffer.length++;
    doc->cursor_x++;
}

void delete_char(Document *doc) {
    if (doc->cursor_x > 0) {
        memmove(&doc->buffer.content[doc->cursor_x - 1], &doc->buffer.content[doc->cursor_x], doc->buffer.length - doc->cursor_x);
        doc->buffer.length--;
        doc->cursor_x--;
    }
}

void render_document(WINDOW *win, Document *doc) {
    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " %s ", doc->filename);

    int start_y = 1;
    int start_x = 1;

    mvwprintw(win, start_y, start_x, "%s", doc->buffer.content);
    wmove(win, start_y + doc->cursor_y, start_x + doc->cursor_x);

    wrefresh(win);
}

void handle_input(int ch, Document *doc) {
    switch (ch) {
        case KEY_LEFT:
            if (doc->cursor_x > 0) doc->cursor_x--;
            break;
        case KEY_RIGHT:
            if ((size_t)doc->cursor_x < doc->buffer.length) doc->cursor_x++;
            break;
        case KEY_BACKSPACE:
        case 127:
            delete_char(doc);
            break;
        default:
            if (ch >= 32 && ch <= 126) {  // Printable ASCII characters
                insert_char(doc, ch);
            }
            break;
    }
}
