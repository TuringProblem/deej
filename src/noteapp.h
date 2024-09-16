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
#define MAX_TODO_ITEMS 15
#define MAX_TODO_TITLE_LENGTH 50

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

typedef struct {
  char title[MAX_TODO_TITLE_LENGTH];
  bool completed;
} TodoItem;

typedef struct {
  TodoItem items[MAX_TODO_ITEMS];
  int count;
} TodoList;

// Declare the global TodoList variable
extern TodoList todo_list;

/*
 * *********************
 * |Function prototypes|
 * *********************
 */

void parse_todo(const char *line);
void render_todo_gui(WINDOW *win);
bool should_render_todo_gui(void);

// C
int check_syntax(const char *line, int *indent_change);
void cleanup_ncurses();
void copy_selection(Document *doc);

// D
void draw_title_bar(WINDOW *win, const char *title);
void draw_status_bar(WINDOW *win, Document *doc);
void draw_sidebar(WINDOW *win);
void delete_char(Document *doc, WINDOW *win);
void delete_char_forward(Document *doc, WINDOW *win);
void display_deej_ascii_art(WINDOW *win);

// F
void free_document(Document *doc);

// H
void handle_input(int ch, Document *doc, WINDOW *win, bool *quit);
void handle_command(Document *doc, const char *command, bool *quit);

// I
void init_ncurses();
void init_document(Document *doc);
void insert_char(Document *doc, WINDOW *win, char c);
void insert_newline(Document *doc, WINDOW *win);

// L
bool line_contains_todo(const char *line);

// M
void move_cursor(Document *doc, WINDOW *win, int dx, int dy);
void move_word_backward(Document *doc);
void move_word_forward(Document *doc);

// P
void paste_clipboard(Document *doc);
void parse_todo(const char *line);

// R
void remove_todo(const char *line);
void render_document(WINDOW *win, Document *doc);
void render_todo_gui(WINDOW *win);

// S
bool should_render_todo_gui(void);

#endif // NOTE_APP_H
