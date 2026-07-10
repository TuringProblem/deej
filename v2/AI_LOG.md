# AI Assist Log (v2)

Purpose: running log of when/why Claude was pulled in on v2. User is learning C++ in school, wants AI used sparingly — as a check, not a crutch. Update this file (append entry) at end of any session where I touch v2.

Rules for me:
- Don't write code unless asked. Default to explaining/reviewing.
- Keep entries short: date, what was asked, what changed/learned.

---

## 2026-07-10
- Reviewed v1 (C) codebase, wrote `/deej/ANALYSIS.md`: found the segfault (delete-last-line → `cursor_y = -1`, `document.c:253`), catalogued dead/unfinished stubs (visual mode, command mode, clipboard, word motions, no file I/O).
- v2 status at this point: skeleton only — `CMakeLists.txt`, `main.cpp` (hello world), `modes.hpp` (`EditorMode` enum class).
- Flagged bug already in v2: `modes.hpp` include guard missing `#define MODES_HPP` — will break on second include.
- No code written in v2 this session — analysis only.
