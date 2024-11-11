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

enum EditorMode { MODE_NORMAL, MODE_INSERT, MODE_VISUAL, MODE_COMMAND };

/**
 * struct Buffer -> Handles the number of characters that are in the
 *text-editor. numLines -> is the number of lines (Vertically ? Let's see :
 *Should be...)
 *
 **/

struct Buffer {
  char *lines[MAX_LINES];
  int num_lines;
};

/*
 * struct Document -> handles the values The screens
 * it will contain the filename -> set at the beginning
 * it will also contain the x, y coords for the cursor (Might need to change
 *this) -> Might be better to pre-define when we build
 *
 **/

struct Document {
  char filename[MAX_FILENAME];
  struct Buffer buffer;
  int cursor_x;
  int cursor_y;
  int visual_start_x;
  int visual_start_y;
  int scroll_offset;
  enum EditorMode mode;
};

struct TodoItem {
  char title[MAX_TODO_TITLE_LENGTH];
  bool completed;
};

struct TodoList {
  struct TodoItem items[MAX_TODO_ITEMS];
  int count;
};

// Declare the global TodoList variable
extern struct TodoList todo_list;

/*
 * *********************
 * |Function prototypes|
 * *********************
 */

// C
int check_syntax(const char *line, int *indent_change);
void cleanup_ncurses();
void copy_selection(struct Document *doc);

// D
void draw_title_bar(WINDOW *win, const char *title);
void draw_status_bar(WINDOW *win, struct Document *doc);
void draw_sidebar(WINDOW *win);
void delete_char(struct Document *doc, WINDOW *win);
void delete_char_forward(struct Document *doc, WINDOW *win);
void display_deej_ascii_art(WINDOW *win);

// F
void free_document(struct Document *doc);

// H
void handle_input(int ch, struct Document *doc, WINDOW *win, bool *quit);
void handle_command(struct Document *doc, const char *command, bool *quit);

// I
void init_ncurses();
void init_document(struct Document *doc);
void insert_char(struct Document *doc, WINDOW *win, char c);
void insert_newline(struct Document *doc, WINDOW *win);

// L
bool line_contains_todo(const char *line);

// M
void move_cursor(struct Document *doc, WINDOW *win, int dx, int dy);
void move_word_backward(struct Document *doc);
void move_word_forward(struct Document *doc);

// P
void paste_clipboard(struct Document *doc);
void parse_todo(const char *line);

// R
void remove_todo(const char *line);
void render_document(WINDOW *win, struct Document *doc);
void render_todo_gui(WINDOW *win);

// S
bool should_render_todo_gui(void);

#endif // NOTE_APP_H
