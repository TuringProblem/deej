#include "noteapp.h"
#include <ctype.h>

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

void insert_char(Document *doc, char c) {
  char *line = doc->buffer.lines[doc->cursor_y];
  size_t len = strlen(line);

  if (len < MAX_LINE_LENGTH - 1) {
    memmove(&line[doc->cursor_x + 1], &line[doc->cursor_x],
            len - doc->cursor_x + 1);
    line[doc->cursor_x] = c;
    doc->cursor_x++;
  }
}

void insert_newline(Document *doc) {
  if (doc->buffer.num_lines < MAX_LINES - 1) {
    char *current_line = doc->buffer.lines[doc->cursor_y];
    char *new_line = malloc(MAX_LINE_LENGTH);

    strcpy(new_line, &current_line[doc->cursor_x]);
    current_line[doc->cursor_x] = '\0';

    memmove(&doc->buffer.lines[doc->cursor_y + 2],
            &doc->buffer.lines[doc->cursor_y + 1],
            (doc->buffer.num_lines - doc->cursor_y - 1) * sizeof(char *));

    doc->buffer.lines[doc->cursor_y + 1] = new_line;
    doc->buffer.num_lines++;
    doc->cursor_y++;
    doc->cursor_x = 0;
  }
}

void delete_char(Document *doc) {
  if (doc->cursor_x > 0) {
    char *line = doc->buffer.lines[doc->cursor_y];
    memmove(&line[doc->cursor_x - 1], &line[doc->cursor_x],
            strlen(&line[doc->cursor_x]) + 1);
    doc->cursor_x--;
  } else if (doc->cursor_y > 0) {
    char *current_line = doc->buffer.lines[doc->cursor_y];
    char *prev_line = doc->buffer.lines[doc->cursor_y - 1];
    size_t prev_len = strlen(prev_line);

    strcat(prev_line, current_line);
    free(current_line);

    memmove(&doc->buffer.lines[doc->cursor_y],
            &doc->buffer.lines[doc->cursor_y + 1],
            (doc->buffer.num_lines - doc->cursor_y - 1) * sizeof(char *));

    doc->buffer.num_lines--;
    doc->cursor_y--;
    doc->cursor_x = prev_len;
  }
}

void delete_char_forward(Document *doc) {
  char *line = doc->buffer.lines[doc->cursor_y];
  if (line[doc->cursor_x] != '\0') {
    memmove(&line[doc->cursor_x], &line[doc->cursor_x + 1],
            strlen(&line[doc->cursor_x + 1]) + 1);
  } else if (doc->cursor_y < doc->buffer.num_lines - 1) {
    char *next_line = doc->buffer.lines[doc->cursor_y + 1];
    strcat(line, next_line);
    free(next_line);

    memmove(&doc->buffer.lines[doc->cursor_y + 1],
            &doc->buffer.lines[doc->cursor_y + 2],
            (doc->buffer.num_lines - doc->cursor_y - 2) * sizeof(char *));

    doc->buffer.num_lines--;
  }
}

void render_document(WINDOW *win, Document *doc) {
  int win_height, win_width;
  getmaxyx(win, win_height, win_width);

  wclear(win);
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

  // Position the cursor
  wmove(win, start_y + doc->cursor_y - doc->scroll_offset,
        start_x + doc->cursor_x);

  // Set cursor visibility based on mode
  if (doc->mode == MODE_NORMAL) {
    curs_set(2); // Very visible cursor (block)
  } else if (doc->mode == MODE_INSERT) {
    curs_set(1); // Visible cursor (underscore)
  } else {
    curs_set(1); // Visible cursor for VISUAL mode too
  }

  wrefresh(win);
}

void move_cursor(Document *doc, int dx, int dy) {
  int new_x = doc->cursor_x + dx;
  int new_y = doc->cursor_y + dy;

  if (new_y >= 0 && new_y < doc->buffer.num_lines) {
    doc->cursor_y = new_y;
    size_t line_length = strlen(doc->buffer.lines[doc->cursor_y]);
    doc->cursor_x =
        (new_x < 0) ? 0
                    : ((size_t)new_x > line_length ? (int)line_length : new_x);
  }
}

void move_word_forward(Document *doc) {
  char *line = doc->buffer.lines[doc->cursor_y];
  size_t line_length = strlen(line);

  while ((size_t)doc->cursor_x < line_length && !isspace(line[doc->cursor_x])) {
    doc->cursor_x++;
  }
  while ((size_t)doc->cursor_x < line_length && isspace(line[doc->cursor_x])) {
    doc->cursor_x++;
  }

  if ((size_t)doc->cursor_x >= line_length &&
      doc->cursor_y < doc->buffer.num_lines - 1) {
    doc->cursor_y++;
    doc->cursor_x = 0;
  }
}

void move_word_backward(Document *doc) {
  char *line = doc->buffer.lines[doc->cursor_y];

  while (doc->cursor_x > 0 && isspace(line[doc->cursor_x - 1])) {
    doc->cursor_x--;
  }
  while (doc->cursor_x > 0 && !isspace(line[doc->cursor_x - 1])) {
    doc->cursor_x--;
  }
}

void copy_selection(Document *doc) {
  (void)doc;
  // TODO: Implement copy functionality
}

void paste_clipboard(Document *doc) {
  (void)doc;
  // TODO: Implement paste functionality
}

void handle_input(int ch, Document *doc, bool *quit) {
  switch (doc->mode) {
  case MODE_NORMAL:
    switch (ch) {
    case 'q':
      *quit = true;
      break;
    case 'i':
      doc->mode = MODE_INSERT;
      curs_set(1);
      break;
    case 'v':
      doc->mode = MODE_VISUAL;
      doc->visual_start_x = doc->cursor_x;
      doc->visual_start_y = doc->cursor_y;
      break;
    case ':':
      doc->mode = MODE_COMMAND;

      break;
    case 'h':
      move_cursor(doc, -1, 0);
      break;
    case 'j':
      move_cursor(doc, 0, 1);
      break;
    case 'k':
      move_cursor(doc, 0, -1);
      break;
    case 'l':
      move_cursor(doc, 1, 0);
      break;
    case 'w':
      move_word_forward(doc);
      break;
    case 'b':
      move_word_backward(doc);
      break;
    case 'x':
      delete_char_forward(doc);
      break;
    case 'p':
      paste_clipboard(doc);
      break;
    }
    break;
  case MODE_INSERT:
    if (ch == 27) { // ESC key
      doc->mode = MODE_NORMAL;
      curs_set(2);
      if (doc->cursor_x > 0)
        doc->cursor_x--;
    } else if (ch == KEY_BACKSPACE || ch == 127) {
      delete_char(doc);
    } else if (ch == KEY_DC) {
      delete_char_forward(doc);
    } else if (ch == '\n' || ch == KEY_ENTER) {
      insert_newline(doc);
    } else if (ch >= 32 && ch <= 126) {
      insert_char(doc, ch);
    }
    break;
  case MODE_VISUAL:
    // ... (implement visual mode commands)
    break;
  case MODE_COMMAND:
    // ... (implement command mode input)
    break;
  }
}
void handle_command(Document *doc, const char *command, bool *quit) {
  if (strcmp(command, "q") == 0) {
    *quit = true;
  }
  // Implement other commands as needed
}
