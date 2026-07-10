# DEEJ — v1 (C) Breakdown

## What it is
NCurses TUI text editor, vim-modal (NORMAL/INSERT/VISUAL/COMMAND), custom `;todo(...)` scripting syntax that renders a floating TODO panel. ~760 LOC across `main.c`, `document.c`, `ui.c`, `syntax.c`, `noteapp.h`.

## Architecture
- `struct Document` = filename + `struct Buffer` (fixed `char* lines[1000]`) + cursor state + mode enum. One global-ish doc passed by pointer everywhere.
- `document.c` — buffer editing (insert/delete char, newline, cursor move), input dispatch (`handle_input`), rendering.
- `ui.c` — chrome: title bar, status bar, sidebar.
- `syntax.c` — `;todo(...)` parser → `TodoList` (global), TODO panel renderer.
- No file I/O anywhere. Filename prompt is cosmetic — nothing is ever read from or written to disk.

## Confirmed segfault source
`document.c:253-271` — `d` in NORMAL mode deletes current line with **no floor check**. Delete the last remaining line → `num_lines` hits 0, then `cursor_y--` → `cursor_y = -1`. Every subsequent access (`move_cursor`, `insert_char`, `render_document`'s `wmove`) indexes `buffer.lines[-1]` or negative curses coords. That's your crash.

## Other real bugs / landmines
- `insert_newline` (`document.c:64`): `strcpy` into a freshly malloc'd `MAX_LINE_LENGTH` buffer with no length check against the source — overflow possible if indent + tail content ≥ 1000 chars.
- VISUAL mode: entered, but `handle_input`'s `MODE_VISUAL` case and the render-time highlight are both empty stubs. Selection state is tracked, never used.
- COMMAND mode: enum exists, `handle_command` is declared in the header, never defined, never wired to any keybind. Dead.
- `copy_selection`, `paste_clipboard`, `move_word_forward`, `move_word_backward` — declared in `noteapp.h`, never implemented, never called anywhere. Pure dead prototypes.
- Global mutable state (`todo_list`, `todo_active` in `syntax.c`, `clipboard` in `document.c`) — fine for single-threaded TUI, but couples every function to hidden state instead of passing it through `Document`.
- Fixed-size everything (1000 lines, 1000 chars/line) — no dynamic growth, silently truncates/breaks past that.
- Build system currently gone: `Makefile` and `obj/*.o` show deleted in git status — v1 doesn't currently build from a clean checkout.

## Verdict
Core edit loop (insert/delete/move/newline) is functionally sound. Everything past that (visual mode, command mode, clipboard, word motions, file save/load) is scaffolding that was declared and never finished. The one actual crash bug is narrow and specific (empty-buffer delete), not systemic — codebase isn't "disgusting," it's a rough MVP with several unfinished branches left in.

---

# v2 (C++) — current state
`v2/CMakeLists.txt` (C++23, glob-built) + `v2/src/main.cpp` (hello world) + `v2/src/components/editor/enums/modes.hpp` (`EditorMode` enum class).

**Bug already present:** `modes.hpp` include guard is broken — `#ifndef MODES_HPP` has no matching `#define MODES_HPP`. Harmless now (one TU), will double-include and fail to compile the moment a second file includes it. Fix before it bites you.

No editor logic ported yet — this is a blank slate, which is the right place to be learning from.
