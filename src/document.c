#include "noteapp.h"

char* clipboard = NULL;
size_t clipboard_length = 0;

void init_document(Document* doc) {
    doc->buffer.content = malloc(MAX_LINE_LENGTH);
    doc->buffer.length = 0;
    doc->buffer.capacity = MAX_LINE_LENGTH;
    doc->cursor_x = 0;
    doc->cursor_y = 0;
    doc->visual_start_x = 0;
    doc->visual_start_y = 0;
    doc->mode = MODE_NORMAL;
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

void copy_selection(Document* doc) {
    int start = (doc->visual_start_x < doc->cursor_x) ? doc->visual_start_x : doc->cursor_x;
    int end = (doc->visual_start_x < doc->cursor_x) ? doc->cursor_x : doc->visual_start_x;
    
    free(clipboard);
    clipboard_length = end - start;
    clipboard = malloc(clipboard_length + 1);
    memcpy(clipboard, &doc->buffer.content[start], clipboard_length);
    clipboard[clipboard_length] = '\0';
}

void paste_clipboard(Document* doc) {
    if (clipboard && clipboard_length > 0) {
        if (doc->buffer.length + clipboard_length >= doc->buffer.capacity) {
            doc->buffer.capacity = doc->buffer.length + clipboard_length + 1;
            doc->buffer.content = realloc(doc->buffer.content, doc->buffer.capacity);
        }
        memmove(&doc->buffer.content[doc->cursor_x + clipboard_length], 
                &doc->buffer.content[doc->cursor_x], 
                doc->buffer.length - doc->cursor_x);
        memcpy(&doc->buffer.content[doc->cursor_x], clipboard, clipboard_length);
        doc->buffer.length += clipboard_length;
        doc->cursor_x += clipboard_length;
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

void move_cursor(Document *doc, int dx, int dy) {
    doc->cursor_x = (doc->cursor_x + dx < 0) ? 0 : 
                    ((size_t)(doc->cursor_x + dx) > doc->buffer.length) ? doc->buffer.length : 
                    doc->cursor_x + dx;
    doc->cursor_y += dy;  // Assuming single line for now
}

void move_word_forward(Document *doc) {
    while ((size_t)doc->cursor_x < doc->buffer.length && !isspace(doc->buffer.content[doc->cursor_x])) {
        doc->cursor_x++;
    }
    while ((size_t)doc->cursor_x < doc->buffer.length && isspace(doc->buffer.content[doc->cursor_x])) {
        doc->cursor_x++;
    }
}

void move_word_backward(Document *doc) {
    while (doc->cursor_x > 0 && isspace(doc->buffer.content[doc->cursor_x - 1])) {
        doc->cursor_x--;
    }
    while (doc->cursor_x > 0 && !isspace(doc->buffer.content[doc->cursor_x - 1])) {
        doc->cursor_x--;
    }
}



void handle_input(int ch, Document *doc) {
    switch (doc->mode) {
        case MODE_NORMAL:
            switch (ch) {
                case 'i': 
                    doc->mode = MODE_INSERT; 
                    curs_set(1);  // Make cursor visible (thin)
                    break;
                case 'v': 
                    doc->mode = MODE_VISUAL;
                    doc->visual_start_x = doc->cursor_x;
                    doc->visual_start_y = doc->cursor_y;
                    break;
                case 'p': 
                    paste_clipboard(doc); 
                    break;                case 'h': move_cursor(doc, -1, 0); break;
                case 'j': move_cursor(doc, 0, 1); break;
                case 'k': move_cursor(doc, 0, -1); break;
                case 'l': move_cursor(doc, 1, 0); break;
                case 'w': move_word_forward(doc); break;
                case 'b': move_word_backward(doc); break;
                case 'x':
                    if ((size_t)doc->cursor_x < doc->buffer.length) {
                        memmove(&doc->buffer.content[doc->cursor_x], &doc->buffer.content[doc->cursor_x + 1], doc->buffer.length - doc->cursor_x - 1);
                        doc->buffer.length--;
                    }
                    break;
            }
            break;
        case MODE_INSERT:
            if (ch == 27) { // ESC key
                doc->mode = MODE_NORMAL;
                curs_set(2);  // Make cursor very visible (block)
                if (doc->cursor_x > 0) doc->cursor_x--;  // Move cursor back one space when exiting INSERT mode
            } else if (ch == KEY_BACKSPACE || ch == 127) {
                delete_char(doc);
            } else if (ch >= 32 && ch <= 126) {  // Printable ASCII characters
                insert_char(doc, ch);
            }
            break;
        case MODE_VISUAL:
            switch (ch) {
                case 27: // ESC key
                    doc->mode = MODE_NORMAL; 
                    break;
                case 'y': 
                    copy_selection(doc);
                    doc->mode = MODE_NORMAL;
                    break;                case 'h': move_cursor(doc, -1, 0); break;
                case 'j': move_cursor(doc, 0, 1); break;
                case 'k': move_cursor(doc, 0, -1); break;
                case 'l': move_cursor(doc, 1, 0); break;
                case 'w': move_word_forward(doc); break;
                case 'b': move_word_backward(doc); break;
            }
            break;
      }
  }
