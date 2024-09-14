#ifndef NOTE_APP_H
#define NOTE_APP_H

#include <ctype.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME 256
#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1000

typedef enum { MODE_NORMAL, MODE_INSERT, MODE_VISUAL, MODE_COMMAND } EditorMode;

typedef struct {
  char *lines[MAX_LINES];
  int num_lines;
} Buffer;

typedef struct {
  char filename[MAX_FILENAME];
  Buffer buffer;
  int cursor_x;
  int cursor_y;
  int visual_start_x;
  int visual_start_y;
  int scroll_offset;
  EditorMode mode;
} Document;

// Function prototypes
void init_ncurses();
void cleanup_ncurses();
void draw_title_bar(WINDOW *win, const char *title);
void draw_status_bar(WINDOW *win, Document *doc);
void draw_sidebar(WINDOW *win);
void init_document(Document *doc);
void free_document(Document *doc);
void insert_char(Document *dic, char c);
void insert_newline(Document *doc);
void delete_char(Document *doc);
void delete_char_forward(Document *doc);
void render_document(WINDOW *win, Document *doc);
void handle_input(int ch, Document *doc, bool *quit);
void move_cursor(Document *doc, int dx, int dy);
void move_word_forward(Document *doc);
void move_word_backward(Document *doc);
void copy_selection(Document *doc);
void paste_clipboard(Document *doc);
void handle_command(Document *doc, const char *command, bool *quit);

#endif // NOTE_APP_H
