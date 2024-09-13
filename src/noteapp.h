#ifndef NOTE_APP_H
#define NOTE_APP_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME 256
#define MAX_LINE_LENGTH 1000

// Data structures
typedef struct {
    char *content;
    size_t length;
    size_t capacity;
} Buffer;

typedef struct {
    char filename[MAX_FILENAME];
    Buffer buffer;
    int cursor_x;
    int cursor_y;
} Document;

// Function prototypes
void init_ncurses();
void cleanup_ncurses();
void draw_title_bar(WINDOW *win, const char *title);
void init_document(Document *doc);
void free_document(Document *doc);
void insert_char(Document *doc, char c);
void delete_char(Document *doc);
void render_document(WINDOW *win, Document *doc);
void handle_input(int ch, Document *doc);

#endif // NOTE_APP_H
