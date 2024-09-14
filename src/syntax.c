#include "noteapp.h"

#define MAX_SYNTAX_RULES 10
#define MAX_RULE_LENGTH 50

typedef struct {
  char start[MAX_RULE_LENGTH];
  char end[MAX_RULE_LENGTH];
  int indent_change;
  bool single_per_file;
} SyntaxRule;

SyntaxRule syntax_rules[MAX_SYNTAX_RULES] = {{"**TODO**(", ");", 1, true},
                                             {"$$DONE$$", "{", 1, true},
                                             {"#!COLOR!#", "{", 1, false},
                                             {"**", "**", 0, false},
                                             {"[", "]", 0, false}};

int num_syntax_rules = 5;

TodoList todo_list = {0};

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
  if (todo_list.count >= MAX_TODO_ITEMS)
    return;

  char title[MAX_FILENAME] = {0};
  char description[MAX_LINE_LENGTH] = {0};

  // Check for the correct syntax
  if (sscanf(line, "**TODO**(%[^{]{%[^}]});", title, description) == 2) {
    // Correct syntax found, add the TODO item
    TodoItem new_item = {0};
    strncpy(new_item.title, title, MAX_FILENAME - 1);
    strncpy(new_item.description, description, MAX_LINE_LENGTH - 1);
    new_item.completed = false;
    todo_list.items[todo_list.count++] = new_item;
  }
}
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

void render_todo_gui(WINDOW *win) {
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
}
