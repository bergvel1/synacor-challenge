cmake_minimum_required(VERSION 3.12)
project (synacor C)
set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB SOURCES "src/*.c")

add_executable(synacor ${SOURCES})
target_link_libraries(synacor Threads::Threads ${CURSES_LIBRARIES} form)