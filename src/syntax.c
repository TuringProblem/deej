#include "noteapp.h"
#include <ctype.h>
#include <string.h>

#define MAX_SYNTAX_RULES 10
#define MAX_RULE_LENGTH 50

static bool todo_active = false;

typedef struct {
  char start[MAX_RULE_LENGTH];
  char end[MAX_RULE_LENGTH];
  int indent_change;
  bool single_per_file;
} SyntaxRule;

// NOTE: ->

SyntaxRule syntax_rules[MAX_SYNTAX_RULES] = {{";todo(", ");", 1, true},
                                             {";done", "{", 1, true},
                                             {";color", "{", 1, false},
                                             {"**", "**", 0, false},
                                             {"[", "]", 0, false}};

int num_syntax_rules = 5;

struct TodoList todo_list = {0};

int check_syntax(const char *line, int *indent_change) {
  *indent_change = 0;
  for (int i = 0; i < num_syntax_rules; i++) {
    if (strstr(line, syntax_rules[i].start) != NULL) {
      if (strstr(line, syntax_rules[i].end) == NULL) {
        *indent_change = syntax_rules[i].indent_change;
        return i;
      }
    }
  }
  return -1;
}

void parse_todo(const char *line) {

  if (line == NULL) {
    return;
  }
  // TODO: CHECK cursor within the define block -> as soon as it leaves the
  // block, you no longer see the block NOTE: ;todo()
  if (strstr(line, ";todo(") != NULL) {
    todo_active = true;
    todo_list.count = 0; // Clear existing TODOs

    char *start = strstr(line, ";todo(") + 9;
    char *end = strchr(start, ')');

    if (end != NULL) {
      char todos[MAX_LINE_LENGTH] = {0};
      size_t len = end - start;
      if (len >= sizeof(todos)) {
        len = sizeof(todos) - 1;
      }
      strncpy(todos, start, len);
      todos[len] = '\0';

      char *token = strtok(todos, ",");
      while (token != NULL && todo_list.count < MAX_TODO_ITEMS) {
        // Trim leading and trailing whitespace
        while (isspace(*token))
          token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace(*end))
          end--;
        *(end + 1) = '\0';

        if (strlen(token) > 0) {
          strncpy(todo_list.items[todo_list.count].title, token,
                  MAX_TODO_TITLE_LENGTH - 1);
          todo_list.items[todo_list.count].title[MAX_TODO_TITLE_LENGTH - 1] =
              '\0';
          todo_list.items[todo_list.count].completed = false;
          todo_list.count++;
        }
        token = strtok(NULL, ",");
      }
    }
  } else {
    todo_active = false;
    todo_list.count = 0; // Clear TODOs if **TODO**( is not found
  }
}

void render_todo_gui(WINDOW *win) {
  if (win == NULL) {
    return;
  }

  int win_height, win_width;
  getmaxyx(win, win_height, win_width);

  int start_y = 1;
  int start_x = win_width - 30; // Keep the width at 30
  int height = win_height - 2;  // Match the height of the sidebar

  // Draw TODO box
  mvwvline(win, start_y, start_x, ACS_VLINE, height);
  mvwvline(win, start_y, start_x + 29, ACS_VLINE, height);
  mvwhline(win, start_y, start_x, ACS_HLINE, 29);
  mvwhline(win, start_y + height, start_x, ACS_HLINE, 29);
  mvwaddch(win, start_y, start_x, ACS_ULCORNER);
  mvwaddch(win, start_y, start_x + 29, ACS_URCORNER);
  mvwaddch(win, start_y + height, start_x, ACS_LLCORNER);
  mvwaddch(win, start_y + height, start_x + 29, ACS_LRCORNER);

  // Draw title
  mvwprintw(win, start_y, start_x + 1, " TODO List ");

  // Draw TODO items
  for (int i = 0; i < todo_list.count && i < height - 2; i++) {
    mvwprintw(win, start_y + 1 + i, start_x + 1, "%c %.*s",
              todo_list.items[i].completed ? 'X' : ' ', 26,
              todo_list.items[i].title); // Limit title to 26 chars
  }

  wrefresh(win);
}

bool should_render_todo_gui() { return todo_active; }

void remove_todo(const char *line) {
  char title[MAX_FILENAME] = {0};
  if (sscanf(line, "**TODO**(%[^{]", title) == 1) {
    for (int i = 0; i < todo_list.count; i++) {
      if (strcmp(todo_list.items[i].title, title) == 0) {
        // Remove this item by shifting the rest of the array
        memmove(&todo_list.items[i], &todo_list.items[i + 1],
                (todo_list.count - i - 1) * sizeof(TodoItem));
        todo_list.count--;
        break;
      }
    }
  }
}

// Add this function to check if a line contains a TODO
bool line_contains_todo(const char *line) {
  return strstr(line, "**TODO**(") != NULL;
}
