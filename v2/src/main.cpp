#include "./components/editor/enums/modes.hpp"
#include <iostream>

int main() {
  std::cout << "hello world!" << std::endl;

  EditorMode mode = EditorMode::NORMAL;
  std::cout << static_cast<int>(mode) << std::endl;
  return 0;
}
