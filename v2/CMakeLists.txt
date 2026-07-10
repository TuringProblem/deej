
cmake_minimum_required(VERSION 3.31)
project(MidnightTutorial)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

file(GLOB_RECURSE source CONFIGURE_DEPENDS "./src/*.cpp" "./src/*.hpp")

add_executable(main ${source})


