#ifndef NOTE_APP_H
#define NOTE_APP_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_FILENAME 256
#define MAX_LINE_LENGTH 1000

typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_VISUAL
} EditorMode;

typedef struct {
    char* content;
    size_t length;
    size_t capacity;
} Buffer;

typedef struct {
    char filename[MAX_FILENAME];
    Buffer buffer;
    int cursor_x;
    int cursor_y;
    int visual_start_x;
    int visual_start_y;
    EditorMode mode;
} Document;

// Function prototypes
void init_ncurses();
void cleanup_ncurses();
void draw_title_bar(WINDOW* win, const char* title);
void draw_status_bar(WINDOW* win, Document* doc);
void init_document(Document* doc);
void free_document(Document* doc);
void insert_char(Document* doc, char c);
void delete_char(Document* doc);
void render_document(WINDOW* win, Document* doc);
void handle_input(int ch, Document* doc);
void move_cursor(Document* doc, int dx, int dy);
void move_word_forward(Document* doc);
void move_word_backward(Document* doc);
void copy_selection(Document* doc);
void paste_clipboard(Document* doc);

#endif // NOTE_APP_H
