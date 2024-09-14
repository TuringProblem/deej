#include "noteapp.h"
#include <unistd.h>

char *clipboard = NULL;
size_t clipboard_length = 0;

void init_document(Document *doc) {
  for (int i = 0; i < MAX_LINES; i++) {
    doc->buffer.lines[i] = NULL;
  }
  doc->buffer.num_lines = 1;
  doc->buffer.lines[0] = malloc(MAX_LINE_LENGTH);
  doc->buffer.lines[0][0] = '\0';
  doc->cursor_x = 0;
  doc->cursor_y = 0;
  doc->visual_start_x = 0;
  doc->visual_start_y = 0;
  doc->scroll_offset = 0;
  doc->mode = MODE_NORMAL;
}

void free_document(Document *doc) {
  for (int i = 0; i < doc->buffer.num_lines; i++) {
    free(doc->buffer.lines[i]);
  }
}

int get_indentation(const char *line) {
  int indent = 0;
  while (line[indent] == ' ' || line[indent] == '\t') {
    indent++;
  }
  return indent;
}
void insert_char(Document *doc, WINDOW *win, char c) {
  char *line = doc->buffer.lines[doc->cursor_y];
  size_t len = strlen(line);

  if (len < MAX_LINE_LENGTH - 1) {
    memmove(&line[doc->cursor_x + 1], &line[doc->cursor_x],
            len - doc->cursor_x + 1);
    line[doc->cursor_x] = c;
    doc->cursor_x++;

    // Check for completed TODO syntax
    if (c == ')') {
      char *todo_start = strstr(line, "**TODO**(");
      char *todo_end = strrchr(line, ')');
      if (todo_start && todo_end && todo_end > todo_start) {
        char *semicolon = strchr(todo_start, ';');
        if (semicolon && semicolon < todo_end) {
          parse_todo(line);
        }
      }
    }

    // Update only the current line
    mvwprintw(win, doc->cursor_y - doc->scroll_offset + 1, 1, "%s", line);
    wmove(win, doc->cursor_y - doc->scroll_offset + 1, doc->cursor_x + 1);

    // Render TODO GUI if needed
    render_todo_gui(win);

    wrefresh(win);
  }
}

void insert_newline(Document *doc, WINDOW *win) {

  if (doc->buffer.num_lines < MAX_LINES - 1) {
    char *current_line = doc->buffer.lines[doc->cursor_y];
    char *new_line = malloc(MAX_LINE_LENGTH);

    int indent = get_indentation(current_line);

    // Copy the rest of the current line to the new line
    strcpy(new_line, &current_line[doc->cursor_x]);
    current_line[doc->cursor_x] = '\0';

    // Insert the new line
    memmove(&doc->buffer.lines[doc->cursor_y + 2],
            &doc->buffer.lines[doc->cursor_y + 1],
            (doc->buffer.num_lines - doc->cursor_y - 1) * sizeof(char *));

    doc->buffer.lines[doc->cursor_y + 1] = new_line;
    doc->buffer.num_lines++;
    doc->cursor_y++;

    // Apply indentation to the new line
    char indented_line[MAX_LINE_LENGTH] = {0};
    for (int i = 0; i < indent; i++) {
      indented_line[i] = ' ';
    }
    strcat(indented_line, new_line);
    strcpy(new_line, indented_line);

    doc->cursor_x = indent;

    // Refresh the entire window as line positions have changed
    render_document(win, doc);
  }
}

void delete_char(Document *doc, WINDOW *win) {
  if (doc->cursor_x > 0) {
    char *line = doc->buffer.lines[doc->cursor_y];
    bool had_todo = line_contains_todo(line);
    memmove(&line[doc->cursor_x - 1], &line[doc->cursor_x],
            strlen(&line[doc->cursor_x]) + 1);
    doc->cursor_x--;

    if (had_todo && !line_contains_todo(line)) {
      remove_todo(line);
    }

    // Update only the current line
    mvwprintw(win, doc->cursor_y - doc->scroll_offset + 1, 1, "%s", line);
    wmove(win, doc->cursor_y - doc->scroll_offset + 1, doc->cursor_x + 1);
    wclrtoeol(win);
    wrefresh(win);
  } else if (doc->cursor_y > 0) {
    char *current_line = doc->buffer.lines[doc->cursor_y];
    char *prev_line = doc->buffer.lines[doc->cursor_y - 1];
    size_t prev_len = strlen(prev_line);

    if (line_contains_todo(current_line)) {
      remove_todo(current_line);
    }

    strcat(prev_line, current_line);
    free(current_line);

    memmove(&doc->buffer.lines[doc->cursor_y],
            &doc->buffer.lines[doc->cursor_y + 1],
            (doc->buffer.num_lines - doc->cursor_y - 1) * sizeof(char *));

    doc->buffer.num_lines--;
    doc->cursor_y--;
    doc->cursor_x = prev_len;

    // Refresh the entire window as line positions have changed
    render_document(win, doc);
  }
}
void delete_char_forward(Document *doc, WINDOW *win) {
  char *line = doc->buffer.lines[doc->cursor_y];
  if (line[doc->cursor_x] != '\0') {
    memmove(&line[doc->cursor_x], &line[doc->cursor_x + 1],
            strlen(&line[doc->cursor_x + 1]) + 1);

    // Update only the current line
    mvwprintw(win, doc->cursor_y - doc->scroll_offset + 1, 1, "%s", line);
    wmove(win, doc->cursor_y - doc->scroll_offset + 1, doc->cursor_x + 1);
    wclrtoeol(win);
    wrefresh(win);
  } else if (doc->cursor_y < doc->buffer.num_lines - 1) {
    char *next_line = doc->buffer.lines[doc->cursor_y + 1];
    strcat(line, next_line);
    free(next_line);

    memmove(&doc->buffer.lines[doc->cursor_y + 1],
            &doc->buffer.lines[doc->cursor_y + 2],
            (doc->buffer.num_lines - doc->cursor_y - 2) * sizeof(char *));

    doc->buffer.num_lines--;

    // Refresh the entire window as line positions have changed
    render_document(win, doc);
  }
}
void render_document(WINDOW *win, Document *doc) {
  int win_height, win_width;
  getmaxyx(win, win_height, win_width);

  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " %s ", doc->filename);

  int start_y = 1;
  int start_x = 1;

  for (int i = doc->scroll_offset;
       i < doc->buffer.num_lines && i - doc->scroll_offset < win_height - 2;
       i++) {
    mvwprintw(win, start_y + i - doc->scroll_offset, start_x, "%.*s",
              win_width - 2, doc->buffer.lines[i]);
  }

  // Highlight selected text in VISUAL mode
  if (doc->mode == MODE_VISUAL) {
    // ... (implement visual selection highlighting)
  }

  // Render TODO GUI if there are items
  if (todo_list.count > 0) {
    render_todo_gui(win);
  }

  // Position the cursor
  wmove(win, start_y + doc->cursor_y - doc->scroll_offset,
        start_x + doc->cursor_x);

  wrefresh(win);
}

void move_cursor(Document *doc, WINDOW *win, int dx, int dy) {
  int new_x = doc->cursor_x + dx;
  int new_y = doc->cursor_y + dy;

  if (new_y >= 0 && new_y < doc->buffer.num_lines) {
    doc->cursor_y = new_y;
    size_t line_length = strlen(doc->buffer.lines[doc->cursor_y]);
    doc->cursor_x =
        (new_x < 0) ? 0
                    : ((size_t)new_x > line_length ? (int)line_length : new_x);
  }

  // Move the cursor without refreshing the entire screen
  wmove(win, doc->cursor_y - doc->scroll_offset + 1, doc->cursor_x + 1);
  wrefresh(win);
}

void handle_input(int ch, Document *doc, WINDOW *win, bool *quit) {
  switch (doc->mode) {
  case MODE_NORMAL:
    switch (ch) {
    case 'q':
      *quit = true;
      return;
    case 'i':
      doc->mode = MODE_INSERT;
      curs_set(1);
      break;
    case 'd':
      if (doc->cursor_y < doc->buffer.num_lines) {
        char *current_line = doc->buffer.lines[doc->cursor_y];
        if (line_contains_todo(current_line)) {
          remove_todo(current_line);
        }
        free(current_line);
        memmove(&doc->buffer.lines[doc->cursor_y],
                &doc->buffer.lines[doc->cursor_y + 1],
                (doc->buffer.num_lines - doc->cursor_y - 1) * sizeof(char *));

        doc->buffer.num_lines--;
        if (doc->cursor_y == doc->buffer.num_lines) {
          doc->cursor_y--;
        }
        doc->cursor_x = 0;
        render_document(win, doc);
      }
      break;
    case 'v':
      doc->mode = MODE_VISUAL;
      doc->visual_start_x = doc->cursor_x;
      doc->visual_start_y = doc->cursor_y;
      break;
    case 'h':
      move_cursor(doc, win, -1, 0);
      break;
    case 'j':
      move_cursor(doc, win, 0, 1);
      break;
    case 'k':
      move_cursor(doc, win, 0, -1);
      break;
    case 'l':
      move_cursor(doc, win, 1, 0);
      break;
    case 'x':
      delete_char_forward(doc, win);
      break;
    }
    break;
  case MODE_INSERT:
    if (ch == 27) { // ESC key
      doc->mode = MODE_NORMAL;
      curs_set(2);
      move_cursor(doc, win, -1, 0);
    } else if (ch == KEY_BACKSPACE || ch == 127) {
      delete_char(doc, win);
    } else if (ch == KEY_DC) {
      delete_char_forward(doc, win);
    } else if (ch == '\n' || ch == KEY_ENTER) {
      insert_newline(doc, win);
    } else if (ch >= 32 && ch <= 126) {
      insert_char(doc, win, ch);
    }
    break;
  case MODE_VISUAL:
    // ... (implement visual mode commands)
    break;
  }
}

void display_art(WINDOW *win, const char **deej_ascii, int height, int width,
                 int color_start, int color_end) {
  int start_y = (LINES - height) / 2;
  int start_x = (COLS - width) / 2;

  for (int i = 0; i < height; i++) {
    int color = color_start + (color_end - color_start) * i / (height - 1);
    wattron(win, COLOR_PAIR(color) | A_BOLD);
    mvwprintw(win, start_y + i, start_x, "%s", deej_ascii[i]);
    wattroff(win, COLOR_PAIR(color) | A_BOLD);
  }
  wrefresh(win);
}

void display_deej_ascii_art(WINDOW *win) {
  const char *deej_ascii[] = {
      "    ____  ___________     __",   "   / __ \\/ ____/ ____/    / /",
      "  / / / / __/ / __/ __   / / ",  " / /_/ / /___/ /___/ /_ / /  ",
      "/_____/_____/_____/\\__//_/   ", "                             ",
      "  Text Editor Extraordinaire ",  "                             ",
      "   Press ENTER to continue   ",  "      Press 'q' to quit      "};

  int height = sizeof(deej_ascii) / sizeof(deej_ascii[0]);
  int width = strlen(deej_ascii[0]);

  // Set up color pairs
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);

  int ch;
  bool quit = false;
  int frame = 0;
  while (!quit) {
    display_art(win, deej_ascii, height, width, 1 + frame % 3, 3 - frame % 3);
    frame++;

    // Check for user input
    timeout(100); // Set a timeout for getch
    ch = wgetch(win);
    if (ch == '\n') {
      break; // Exit the loop and continue to the filename input
    } else if (ch == 'q' || ch == 'Q') {
      quit = true; // Set quit flag to true
    }

    usleep(100000); // Sleep for 100ms
  }

  // Reset the timeout
  timeout(-1);

  if (quit) {
    endwin();
    exit(0);
  }
}
